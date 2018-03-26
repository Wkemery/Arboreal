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
#include "../SharedHeaders/Parser.h"
#include "../SharedHeaders/DebugMessages.hpp"
#include "../SharedHeaders/Arboreal_Exceptions.h"    /* Exception Handling */
#include "../SharedHeaders/Print.h"


static const int Permissions = 0666;
static const int MaxBufferSize = 4096;            /* Maximum Size a FS Command Buffer Can Be */
static const int SharedMemorySize = 1;            /* The Size of a Shared Memory Segment */
static const int Backlog = 10;                    /* Number of Connection Requests that the Server Can Queue */
static const int Flag = 0;                        /* Flag for Send/Recv. Operations */
static const int DaemonPort = 70777;              /* Port On Which The FS Daemon Is Listening */
static const int Timeout = 10;                    /* How Many Seconds More To Continue Trying If An Operation Fails */
DebugMessages Debug;
Parser* Parser = 0;
#include "LiaisonDependancies/liason_helper.hpp"                      /* Helper Functions */
/*************************************************************************************************/




int main(int argc, char** argv)
{
  /* Catch Termination Signals */
  signal(SIGABRT,bad_clean);
  signal(SIGTERM,clean);
  signal(SIGINT,clean);
  signal(SIGQUIT,clean);
  signal(SIGSEGV,seg_fault);


  /* Turn on debug printing */
  if(argc == 5){Debug.ON();}
  else{Debug.OFF();}

  
  Debug.log("L: Liaison Process Initiated");
  std::string client_sockpath = argv[1];
  std::string liaison_sockpath = argv[2];;
  std::string partition = argv[3];
  std::string cwd = argv[4];


  Debug.log(("L: Client Socket Path: " + client_sockpath));
  Debug.log(("L: Liaison Socket Path: " + liaison_sockpath));
  Debug.log(("L: File System Partition: " + partition));
  Debug.log(("L: Current Working Directory: " + cwd));
  /***********************************************************************************************/


  int max_string_size; // Will be set via handshake
  struct sockaddr_un liaison_sockaddr;
  struct sockaddr_un client_sockaddr;
  struct sockaddr_in daemon_addr;
  int liaison_fid;
  int liaison_sock;
  int client_sock;
  socklen_t length;
  /***********************************************************************************************/

  try
  {

    // Connect to CLI

    key_t shm_key = atoi(argv[0]);
    int shm_id;

    Debug.log(("L: Shared Memory Segment Key: " + std::to_string((int)shm_key)));


    Debug.log("L: Attaching to Shared Memory Segment...");
    char* shm = get_shm_seg(shm_key,shm_id);
    Debug.log("L: Success");
    /*********************************************************************************************/
    

    /* Zero the structure buffers */
    memset(&liaison_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    
    Debug.log("L: Initializing [Liaison --> Command Line] Socket...");
    liaison_sock = set_up_socket(liaison_sockpath,liaison_sockaddr);
    Debug.log("L: Success");
    /*********************************************************************************************/
    


    Debug.log("L: Signaling Command Line");
    /* Signal CLI that it is ok to continue */
    shm[0] = 1;
    /*********************************************************************************************/
    

    Debug.log("L: Listening For Clients...");
    listen_for_client(liaison_sock,liaison_sockpath);
  

    length = sizeof(liaison_sockaddr);
    /* Wait until CLI is ready, if this is not done you will get a
     * "Connection Refused Error" every once in a while */
    while(shm[0] == 1)
    {
      client_sock = accept_client(liaison_sock,client_sockaddr,length,liaison_sockpath);
    }
    Debug.log("L: Client Connection Accepted");
    /*********************************************************************************************/


    Debug.log("L: Unattaching Shared Memory Segment...");
    unat_shm(shm_id,shm);
    Debug.log("L: Success");
    /*********************************************************************************************/


    Debug.log("L: Retrieving Client Information");
    get_peername(client_sock,client_sockaddr,liaison_sock,liaison_sockpath);
    Debug.log("L: Connection To Client Successful");
    /*********************************************************************************************/


    /* Connect To File System */

    Debug.log("L: Attempting To Connect To File System...");


    Debug.log("L: Initializing [Liaison --> File System] Socket...");
    /* Initialize a connection to the File System, The parameters passed here are purely in the case
     * of failure since we need to send a message to the CLI process notifying of the failure */
    liaison_fid = create_daemon_sock(client_sock, client_sockpath, liaison_sock, liaison_sockpath); 
    Debug.log("L: Success");

    memset(&daemon_addr, '\0', sizeof(daemon_addr));
    daemon_addr.sin_family = AF_INET;
    daemon_addr.sin_port = htons(DaemonPort);
    /*********************************************************************************************/


    Debug.log("L: Connecting To File System...");
    /* Attempt to connect to File System. The parameters passed here are purely in the case
     * of failure since we need to send a message to the CLI process notifying of the failure */
    connect_to_daemon(liaison_fid, daemon_addr, 
                      client_sock, client_sockpath, liaison_sock, liaison_sockpath);
    Debug.log("L: Success");
    /*********************************************************************************************/


    /* Begin communication */
    do
    {
      /* Receive Command From Command Line */
      char* msg = recv_msg(client_sock,MaxBufferSize,
                           Flag,liaison_sock,liaison_sockpath,client_sockpath);
      /*******************************************************************************************/


      /* Some Debug Info */
      Debug.log(("L: Received: " + get_command_string(msg,MaxBufferSize)));
      std::string sender = (std::to_string(client_sock) + " @ " + client_sockpath);
      auto end = std::chrono::system_clock::now();
      std::time_t end_time = std::chrono::system_clock::to_time_t(end);
      std::string datetime = std::ctime(&end_time);
      Debug.log(("L: Received From: " + sender));
      Debug.log(("L: Received @ " + datetime));
      /*******************************************************************************************/


      int command_id = get_cmnd_id(msg);
      Debug.log(("L: Command ID: " + std::to_string(command_id)));
      /*******************************************************************************************/

      if(command_id == QUIT)
      {
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
        /*****************************************************************************************/


        std::string success = "Working Directory Switched To: [ " + cwd + " ]";
        send_response(client_sock,success.c_str(),MaxBufferSize,
                      Flag,liaison_sock,liaison_sockpath,client_sockpath);
        Debug.log(success);
        continue;
      }
      else if(command_id == CD_RLP)
      {
        /* Change directory using a relative path */
        std::string temp;
        int index = (int)sizeof(int) + 1;
        while(msg[index] != '\0'){ temp += msg[index]; index += 1;}
        cwd += temp;
        Parser->set_cwd(cwd);
        /*****************************************************************************************/


        std::string success = "Working Directory Switched To: [ " + cwd + " ]";
        send_response(client_sock,success.c_str(),MaxBufferSize,
                      Flag,liaison_sock,liaison_sockpath,client_sockpath);
        Debug.log(success);
        continue;
      }
      else if(command_id == HANDSHK)
      {

        Debug.log("L: Iniating Handshake With File System...");
        int rval = send(liaison_fid,msg,MaxBufferSize,0);
        /*****************************************************************************************/


        if(rval < 0)
        {
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "HANDSHAKE Send Failed -- ";
          what += strerror(errno);
          what += "\n\n";
          throw arboreal_liaison_error(where,what);
        }
        /*****************************************************************************************/


        char temp[MaxBufferSize];
        memset(temp,'\0',MaxBufferSize);
        rval = recv(liaison_fid,temp,MaxBufferSize,0);
        /*****************************************************************************************/


        if(rval < 0)
        {
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "HANDSHAKE Response Receive Failed -- ";
          what += strerror(errno);
          what += "\n\n";
          throw arboreal_liaison_error(where,what);
        }
        /*****************************************************************************************/


        if(get_cmnd_id(temp) == FTL_ERR)
        {
          /* 9999 is Failure (usually fatal) */
          std::string where = "\n\n[liason.cpp::main()]: ";
          std::string what = "Requested Partition Does Not Exist\n\n";
          throw arboreal_liaison_error(where,what);
        }
        /*****************************************************************************************/


        max_string_size = get_cmnd_id(temp);
        Parser = new class Parser("",cwd,max_string_size);
        send_response(client_sock,temp,MaxBufferSize,
                      Flag,liaison_sock,liaison_sockpath,client_sockpath);
        /*****************************************************************************************/


        Debug.log("L: Success, Maximum Filename/Tagname Size [" + 
                   std::to_string(max_string_size) + "]");
        continue;
      }
      else
      {

        /* All other commands don't have any special considerations */
        Debug.log("L: Parsing Command");
        char temp[MaxBufferSize];
        memset(temp,'\0',MaxBufferSize);
        memcpy(temp,(msg + sizeof(int)), MaxBufferSize - sizeof(int));
        Parser->reset(temp,cwd);
        std::vector<std::string> vec;
        /*****************************************************************************************/


        /* Attempt Parsing */
        try
        {
          vec = Parser->parse(command_id);
          vec.push_back("$");
        }
        catch(ParseError& e)
        {
          std::cerr << "\n\n" << e.where() << e.what() << std::endl << std::endl;
          std::string s = "One Of Your Tag/Filenames Is Too Long; Command Ignored\n";
          s += "Maximum Tag/Filename Size For This Partition (" + partition + ") Is: ";
          s += std::to_string(max_string_size);
          s += "\n";
          s = pad_string(s, MaxBufferSize - s.length(), '\0');
          send_response(client_sock,s.c_str(),MaxBufferSize,
                        Flag,liaison_sock,liaison_sockpath,client_sockpath);
          continue;
        }
        /*****************************************************************************************/

        //print_vector(vec);
        Debug.log("Parsed Data Follows: ");
        for(unsigned int i = 0; i < vec.size(); i++){Debug.log((vec[i] + " "));}
        /*****************************************************************************************/


        
        Debug.log("L: Sending Parsed Data To File System...");
        /***************************************************************************************/

        int rval = 0;
        for(unsigned int i = 0; i < vec.size(); i++)
        {
          vec[i] = pad_string(vec[i], MaxBufferSize - vec[i].length(), '\0');
          Debug.log("L: Sending [" + vec[i] + "]");
          rval = send(liaison_fid,vec[i].c_str(),MaxBufferSize,Flag);
        }
        /***************************************************************************************/


        Debug.log("L: Waiting For Read Size...");
        char read_size_buf[MaxBufferSize];
        memset(read_size_buf,'\0',MaxBufferSize);
        rval = recv(liaison_fid,read_size_buf,MaxBufferSize,Flag);

        int read_size = get_cmnd_id(read_size_buf);
        Debug.log("L: Read Size Received [" + std::to_string(read_size) + "]");
        /***************************************************************************************/


        Debug.log("L: Receiving Data...");
        char data[read_size];
        memset(data,'\0',read_size);
        rval = recv(liaison_fid,data,read_size,Flag);

        std::string debug_print;

        for(unsigned int i = 0; i < read_size - 1; i++){debug_print += data[i];}
        Debug.log("L: Received :\n" + debug_print);

        std::cout << std::endl;
        std::cout << debug_print << std::endl;
        /***************************************************************************************/


        Debug.log("L: Sending Confirmation To Command Line");
        std::string success = "Success";
        send_response(client_sock,success.c_str(),MaxBufferSize,Flag,
                      liaison_sock,liaison_sockpath,client_sockpath);
        Debug.log("L: Return Value [" + std::to_string(rval) + "]");

      }
    }while(true);

    /* Quit Liaison Process (This is good quit) */
    shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);

  } /* End try{} */
  catch(arboreal_liaison_error e)
  {
    /* Quit Liaison Process (This is bad quit) */
    std::cerr << e.where() << e.what() << std::endl;
    shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);
  }
  return 0;
}
