///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.cpp
//  Liason process for communication between GUI,CLI,Filesystem
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>                               /* System Stuff */
#include <string>                                 /* Strings */
#include <iostream>                               /* Input & Output */
#include <vector>                                 /* Vectors */
#include <errno.h>                                /* errno Definitions */
#include <stdio.h>                                /* C-Std Input/Output */
#include <unistd.h>                               /* Unix Std. Stuff */
#include <sys/socket.h>                           /* Socket Handling */
#include <sys/un.h>                               /* Unix Domain Socket Stuff */
#include <sys/ipc.h>                              /* Inter Process Communication Stds. */
#include <sys/shm.h>                              /* Shared Memory Handling */
#include <chrono>                                 /* System Time */
#include <ctime>                                  /* Time Utilities */
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include "Parser.h"
#include "DebugMessages.h"
#include "../Filesystem/Arboreal_Exceptions.h"    /* Exception Handling */
#include "../Filesystem/ErrorCodes.h"


static const int Permissions = 0666;
static const int MaxBufferSize = 4096;            /* Maximum Size a FS Command Buffer Can Be */
static const int SharedMemorySize = 1;            /* The Size of a Shared Memory Segment */
static const int Backlog = 10;                    /* Number of Connection Requests that the Server Can Queue */
static const int Flag = 0;                        /* Flag for Send/Recv. Operations */
static const int DaemonPort = 70777;              /* Port On Which The FS Daemon Is Listening */
static const int Timeout = 10;                    /* How Many Seconds More To Continue Trying If An Operation Fails */
DebugMessages Debug;
Parser* Parser = 0;
#include "liason_helper.hpp"                      /* Helper Functions */





int main(int argc, char** argv)
{
  signal(SIGABRT,bad_clean);
  signal(SIGTERM,clean);
  signal(SIGINT,clean);
  signal(SIGQUIT,clean);
  signal(SIGSEGV,seg_fault);


  /* Turn on debug printing */
  if(argc == 7){Debug.ON();}
  else{Debug.OFF();}

  Debug.log(liaison_start);

  std::string client_sockpath = argv[0];
  std::string liaison_sockpath = argv[1];
  std::string hostname = argv[3];
  std::string partition = argv[4];
  std::string cwd = argv[5];

  Debug.log(liaison_hostname,hostname);
  Debug.log(liaison_partiton,partition);
  Debug.log(liaison_client_sock_path,client_sockpath);
  Debug.log(liaison_sock_path,liaison_sockpath);
  Debug.log(liaison_daemon_port,DaemonPort);
  Debug.log(liaison_cwd,cwd);


  int max_string_size; // Will be set via handshake
  struct sockaddr_un liaison_sockaddr;
  struct sockaddr_un client_sockaddr;
  struct sockaddr_in daemon_addr;
  int liaison_fid;
  int liaison_sock;
  int client_sock;
  socklen_t length;

  try
  {

    // Connect to CLI

    key_t shm_key = atoi(argv[2]);
    int shm_id;

    Debug.log(liaison_get_shm);
    char* shm = get_shm_seg(shm_key,shm_id);
    Debug.log(liaison_get_shm_success);

    /* Zero the structure buffers */
    memset(&liaison_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    Debug.log(liaison_init_socket);
    liaison_sock = set_up_socket(liaison_sockpath,liaison_sockaddr);
    Debug.log(liaison_init_sock_success);


    Debug.log(liaison_signal);
    /* Signal CLI that it is ok to continue */
    shm[0] = 1;

    Debug.log(liaison_listen);
    listen_for_client(liaison_sock,liaison_sockpath);
    Debug.log(liaison_listen_success);

    length = sizeof(liaison_sockaddr);

    Debug.log(liaison_accept);
    /* Wait until CLI is ready, if this is not done you will get a
     * "Connection Refused Error" every once in a while */
    while(shm[0] == 1)
    {
      client_sock = accept_client(liaison_sock,client_sockaddr,length,liaison_sockpath);
    }
    Debug.log(liaison_accept_success);


    Debug.log(liaison_unattch_shm);
    unat_shm(shm_id,shm);
    Debug.log(liaison_unattch_success);

    Debug.log(liaison_get_peer);
    get_peername(client_sock,client_sockaddr,liaison_sock,liaison_sockpath);
    Debug.log(liaison_get_peer_success,client_sockaddr.sun_path);




     // Connect To Daemon

    // Debug.log...
    /* Initialize a connection to the File System, The parameters passed here are purely in the case
     * of failure since we need to send a message to the CLI process notifying of the failure */
    liaison_fid = create_daemon_sock(client_sock, client_sockpath, liaison_sock, liaison_sockpath);
     // Debug.log...

    memset(&daemon_addr, '\0', sizeof(daemon_addr));
    daemon_addr.sin_family = AF_INET;
    daemon_addr.sin_port = htons(DaemonPort);

    /* Attempt to connect to File System. The parameters passed here are purely in the case
     * of failure since we need to send a message to the CLI process notifying of the failure */
    connect_to_daemon(liaison_fid, daemon_addr, client_sock, client_sockpath, liaison_sock, liaison_sockpath);



    /* Begin communication */
    do
    {
      // TO DO: Place More debug statements around here somewhere

      char* msg = recv_msg(client_sock,MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
      Debug.log(liaison_command_received);
      std::string sender = (std::to_string(client_sock) + " @ " + client_sockpath);
      Debug.log(liaison_command_sender, sender);
      Debug.log(liaison_command, get_command_string(msg,MaxBufferSize));

      auto end = std::chrono::system_clock::now();
      std::time_t end_time = std::chrono::system_clock::to_time_t(end);
      std::string data = std::ctime(&end_time);
      Debug.log(liaison_received_time,data);



      int command_id = get_cmnd_id(msg);

      if(command_id == QUIT)
      {
        /* 999 is QUIT command */
        break;
      }
      else if(command_id == CD_ABS)
      {
        /* This is a change in working directory */
        std::string temp;
        int index = (int)sizeof(int);
        while(msg[index] != '\0'){ temp += msg[index]; index += 1;}
        cwd = temp;
        Parser->set_cwd(cwd);
        std::string success = "Working Directory Switched To: [ " + cwd + " ]";
        send_response(client_sock,success.c_str(),MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
        continue;
      }
      else if(command_id == CD_RLP)
      {
        std::string temp;
        int index = (int)sizeof(int) + 1;
        while(msg[index] != '\0'){ temp += msg[index]; index += 1;}
        cwd += temp;
        Parser->set_cwd(cwd);
        std::string success = "Working Directory Switched To: [ " + cwd + " ]";
        send_response(client_sock,success.c_str(),MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
        continue;
      }
      else if(command_id == HANDSHK)
      {
        /* This is attempt HANDSHAKE */
        int rval = send(liaison_fid,msg,MaxBufferSize,0);
        if(rval < 0)
        {
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "HANDSHAKE Send Failed -- ";
          what += strerror(errno);
          what += "\n\n";
          throw arboreal_liaison_error(where,what);
        }
        char temp[MaxBufferSize];
        memset(temp,'\0',MaxBufferSize);
        rval = recv(liaison_fid,temp,MaxBufferSize,0);
        if(rval < 0)
        {
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "HANDSHAKE Response Receive Failed -- ";
          what += strerror(errno);
          what += "\n\n";
          throw arboreal_liaison_error(where,what);
        }

        if(get_cmnd_id(temp) == FTL_ERR)
        {
          /* 9999 is Failure (usually fatal) */
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "Requested Partition Does Not Exist\n\n";
          throw arboreal_liaison_error(where,what);
        }
        max_string_size = get_cmnd_id(temp);
        Parser = new class Parser("",cwd,max_string_size);
        send_response(client_sock,temp,MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
        continue;
      }
      else
      {
        /* All other commands don't have any special considerations */
        char temp[MaxBufferSize];
        memset(temp,'\0',MaxBufferSize);
        memcpy(temp,(msg + sizeof(int)), MaxBufferSize - sizeof(int));
        Parser->reset(temp,cwd);
        std::vector<std::string> vec;
        try
        {
          vec = Parser->parse(command_id);
        }
        catch(ParseError& e)
        {
          std::cerr << "\n\n" << e.where() << e.what() << std::endl << std::endl;
          std::string s = "One Of Your Tag/Filenames Is Too Long; Command Ignored\n";
          s += "Maximum Tag/Filename Size For This Partition (" + partition + ") Is: ";
          s += std::to_string(max_string_size);
          s += "\n";
          s = pad_string(s, MaxBufferSize - s.length(), '\0');
          send_response(client_sock,s.c_str(),MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
          continue;
        }
        //print_vector(vec);


        if(command_id != FIND_TS && command_id != FIND_FS)
        {
          std::string data = "\n";
          for(unsigned int i = 0; i < vec.size(); i++)
          {
            std::string command = pad_string(vec[i],(MaxBufferSize - vec[i].length()),'\0');
            int rval = send(liaison_fid,command.c_str(),MaxBufferSize,Flag);
  
            char response[MaxBufferSize];
            memset(response,'\0',MaxBufferSize);
            rval = recv(liaison_fid,response,MaxBufferSize,Flag);
            std::string r = response;
            if(r == "Command Accepted"){continue;}
            if(rval > 0)
            {
              data += response;
              data += "\n";
            }
            memset(response,'\0',MaxBufferSize);
          }

          std::size_t found = data.find("Successfully");
          if((command_id == DEL_TS || command_id == DEL_FP) && found != std::string::npos && cwd != "/")
          {
            cwd = "/";
            Parser->set_cwd(cwd); 
          }
          data = pad_string(data, MaxBufferSize - data.length(), '\0');
          send_response(client_sock,data.c_str(),MaxBufferSize,Flag,
                        liaison_sock,liaison_sockpath,client_sockpath);
        }
        else
        {
          for(unsigned int i = 0; i < vec.size(); i++)
          {
            std::string command = pad_string(vec[i],(MaxBufferSize - vec[i].length()),'\0');
            send(liaison_fid,command.c_str(),MaxBufferSize,Flag);
          }

          std::string data = "\n";
          std::string stemp;
          char response[MaxBufferSize];
          memset(response,'\0',MaxBufferSize);
          while(stemp != "DONE")
          {
            data += (stemp + "\n");
            stemp = "";
            int rval = recv(liaison_fid,response,MaxBufferSize,Flag);
            std::string r = response;
            if(r == "Command Accepted"){continue;}
            if(rval > 0){stemp = response;}
          }

          std::string wait = "WAIT";
          wait = pad_string(wait, MaxBufferSize - wait.length(), '\0');
          send_response(client_sock,wait.c_str(),MaxBufferSize,Flag,
                        liaison_sock,liaison_sockpath,client_sockpath);

          char read_size[MaxBufferSize];
          int r_size = data.length();

          memset(read_size, '\0', MaxBufferSize);
          memcpy(read_size, &r_size, sizeof(int));

          send_response(client_sock, read_size, MaxBufferSize, Flag,
                        liaison_sock,liaison_sockpath, client_sockpath);

          send_response(client_sock, data.c_str(), r_size, Flag,
                        liaison_sock,liaison_sockpath, client_sockpath);

          //printf("Data: %s\n", data.c_str());
        }
      }

    }while(true);

    shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);


  } // End try{}
  catch(arboreal_liaison_error e)
  {
    std::cerr << e.where() << e.what() << std::endl;
    shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);
  }
  return 0;
}
