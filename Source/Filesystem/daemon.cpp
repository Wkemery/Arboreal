//////////////////////////////////////////////////////////
//
// ConnectionDaemon.cpp
//
// Handles Command Requests From CLI And Executes The Appropriate File System Functions
// Then Returns The Requested Data Back To The CLI
//
// Author:    Adrian Barberis
// For:       Arboreal Project
// Dated:     March | 3rd | 2018
//////////////////////////////////////////////////////////


#include <thread>                           /* Threading */
#include <errno.h>                          /* errno Definitions */
#include <unistd.h>                         /* Unix Std. Stuff */
#include <sys/socket.h>                     /* Socket Handling */
#include <sys/un.h>                         /* Unix Domain Socket Stuff */
#include <sys/ipc.h>                        /* Inter Process Communication Stds. */
#include <sys/shm.h>                        /* Shared Memory Handling */
#include <netinet/in.h>                     /* Internet TCP Socket Stuff */
#include <netdb.h>                          /* More Internet Socket Stuff */
#include <sys/ioctl.h>                      /* Set Sockets To Non-Blocking */
#include <signal.h>

#include "DaemonDependancies/FileSystem/FileSystem.h"
#include "DaemonDependancies/File/File.h"
#include "DaemonHeaders/daemon.h"



/***************** BEGIN MAIN *******************/

int main(int argc, char** argv)
{
  bool dbug = false;
  signal(SIGABRT,sig_caught);
  signal(SIGTERM,sig_caught);
  signal(SIGINT,sig_caught);
  signal(SIGQUIT,sig_caught);
  signal(SIGSEGV,sig_caught);

  std::string arg;
  if(argc == 2) arg = argv[1];
  if(arg == "-d") dbug = true;

//---------------- INITIALIZATIONS ---------------------------------------------------------
//
//
//
  if(dbug) printf("D: Beginning File System...\n");

  try
  {
    d = new Disk(500, 512, const_cast<char *>("DISK1"));
    dm = new DiskManager(d);
  }
  catch(arboreal_exception& e)
  {
    std::cerr << "[Error]: " << e.what() << " in " << e.where() << std::endl;
    exit(1);
  }
//
//
//
//--------------------------------------------------------------------------------------------




//---------------- BEGIN "MAIN" PROGRAM ------------------------------------------------------
//
//
//
//
  try
  {
    if(dbug) printf("D: Max Command Size: %d\n",MAX_COMMAND_SIZE);
    std::vector<int> active_connections;



  //---------------- BEGIN SOCKET SETUP ------------------------------------------------------
  //
  //
    fd_set working_set;
    struct sockaddr_in daemon_sockaddr;
    int daemon_sock, close_conn = 0;
    int sock_opt = 1;
    int on = 1;
    int rval = 0;
    int desc_ready, client_sock = 0;
    int END_SERVER = FALSE;
    int daemon_addrlen = daemon_addrlen = sizeof(daemon_sockaddr);
    char buffer[MAX_COMMAND_SIZE];

    if(dbug) printf("D: Zeroing Socket Address and Buffer\n");
    memset(buffer,'\0',MAX_COMMAND_SIZE);
    memset(&daemon_sockaddr, 0, sizeof(daemon_sockaddr));

    if(dbug) printf("D: Creating Daemon Socket...\n");
    daemon_sock = create_sock(TIMEOUT);
    my_fid = daemon_sock;
    if(dbug) printf("D: Daemon Socket ID: %d\n",daemon_sock);
    if(dbug) printf("D: Setting Socket Options In Order To Reuse Socket Later...\n");
    set_socket_opt(daemon_sock,sock_opt,TIMEOUT);

    daemon_sockaddr.sin_family = AF_INET;
    daemon_sockaddr.sin_addr.s_addr = INADDR_ANY;
    daemon_sockaddr.sin_port = htons(PORT);

    if(dbug) printf("D: Setting Socket To Non-Blocking Mode...\n");
    set_nonblocking(daemon_sock,on);

    if(dbug) printf("D: Binding Socket %d To Port %d\n",daemon_sock,PORT);
    bind_socket(daemon_sock,daemon_sockaddr,TIMEOUT);

    if(dbug) printf("D: Listening On Socket %d With a Backlog of %d\n",daemon_sock,BACKLOG);
    listen_on_socket(daemon_sock,BACKLOG,TIMEOUT);

    if(dbug) printf("D: Initializing Master File Descriptor Array...\n");
    FD_ZERO(&master_set);
    max_fid = daemon_sock;
    FD_SET(daemon_sock,&master_set);
    if(dbug) printf("D: File Descriptor Set Initialized\n");
  //
  //
  //---------------- END SOCKET SET UP -------------------------------------------------------



  //---------------- START "QUIT" THREAD -----------------------------------------------------
  //
  //
    if(dbug) printf("D: Beginning Quit Thread...\n");
    // Listens for "Q"/"q"/"quit" and quits daemon (for testing only)
    std::thread quit(quit_fs);
    quit.detach();
    if(dbug) printf("D: Quit Thread Started and Detatched From Main Process\n");
  //
  //
  //------------------------------------------------------------------------------------------




  //---------------- BEGIN COMMUNICATIONS ----------------------------------------------------
  //
  //
  //
  //
    do
    {
      /* Copy master file descriptor set to working set */
      if(dbug) printf("D: Copying Master File Descriptor Set To Working Set...\n");
      memcpy(&working_set, &master_set, sizeof(master_set));


    //---------------- ATTEMPT SELECT() ------------------------------------------------------
    //
    //
      if(dbug) printf("D: Attempting Call To select()...\n");
      rval = select(max_fid + 1, &working_set, NULL, NULL, NULL);
      if(rval < 0)
      {
        std::string where = "[ConnectionDaemon.cpp::main()]: ";
        std::string what = "select() Failed -- ";
        what += strerror(errno);
        throw arboreal_daemon_error(where,what);
        break;
      }
      else if(rval == 0)
      {
        std::string where = "[ConnectionDaemon.cpp::main()]: ";
        std::string what = "select() Timed Out -- ";
        what += strerror(errno);
        throw arboreal_daemon_error(what,where);
        break;
      }
      if(dbug) printf("D: Call To select() Successful\n");
    //
    //
    //----------------------------------------------------------------------------------------



    //---------------- BEGIN ACCEPTING AND RECEIVING LOOP ------------------------------------
    //
    //
      if(dbug) printf("D: Beginning Accept Loop...\n");
      desc_ready = rval;
      for(int i = 0; i <= max_fid && desc_ready > 0; i++)
      {
        if(FD_ISSET(i,&working_set))
        {
          desc_ready -= 1;
          if(i == daemon_sock)
          {
            if(dbug) printf("D: Listening Socket (Server) Is Readable; Now Accepting Connections...\n");
            do
            {
              client_sock = accept(daemon_sock,NULL,NULL);
              if(client_sock < 0)
              {
                if(errno != EWOULDBLOCK)
                {
                  END_SERVER = TRUE;
                  std::string where = "[ConnectionDaemon.cpp::main()]: ";
                  std::string what = "accept() Failed -- ";
                  what += strerror(errno);
                  throw arboreal_daemon_error(what,where);
                }
                break;
              }
              if(dbug) printf("D: New Incoming Connection - %d - Detected, Adding To Master Set\n",client_sock);
              FD_SET(client_sock,&master_set);
              if(client_sock > max_fid) max_fid = client_sock;

            }while(client_sock != -1);
          }
          else
          {
            if(dbug) printf("D: Client Descriptor - %d - Is Readable\n",i);
            if(dbug) printf("D: Beginning Receive Loop...\n");
            close_conn = FALSE;

            do
            {
              memset(buffer,'\0',MAX_COMMAND_SIZE);
              rval = recv(i, buffer, MAX_COMMAND_SIZE, FLAG);
              if(rval < 0)
              {
                if(errno != EWOULDBLOCK)
                {
                  close_conn = TRUE;
                  std::string where = "[ConnectionDaemon.cpp::main()]: ";
                  std::string what = "recv() Failed -- ";
                  what += strerror(errno);
                  throw arboreal_daemon_error(what,where);
                }
                break;
              }

              if(rval == 0)
              {
                if(dbug) printf("D: Client Closed Connection; Breaking From Receive Loop\n");
                close_conn = TRUE;
                break;
              }

              int bytes_received = rval;
              if(dbug) printf("\nBytes Received: %d\n",bytes_received);
              if(dbug) printf("Command Received: %s\n\n",buffer);

              if(get_cmnd_id(buffer) == QUIT)
              {
                if(dbug) printf("D: Client Closed Connection; Breaking From Receive Loop\n");
                close_conn = TRUE;
                break;
              }
              else if(get_cmnd_id(buffer) == 0)
              {
                std::string part = get_partition(buffer);
                printf("D: Requested Partition: %s\n",part.c_str());
                try
                {
                  auto it = part_fs_map.find(part);
                  if(it == end(part_fs_map))
                  {
                    fd_fs_map.insert(std::pair<int,FileSystem*>(i,new FileSystem(dm,part)));
                    part_fs_map.insert(std::pair<std::string,FileSystem*>(part,fd_fs_map[i]));
                  }
                  else
                  {
                    fd_fs_map.insert(std::pair<int,FileSystem*>(i,part_fs_map[part]));
                  }
                }
                catch(arboreal_exception& e)
                {
                  std::cerr << "D: [Error]: " << e.where() << "--" << e.what() << std::endl;
                  std::cerr << "D: Closing File Descriptor [" << i << "]" << std::endl;
                  char failed[MAX_COMMAND_SIZE];
                  int failure = FTL_ERR;
                  memset(failed,0,MAX_COMMAND_SIZE);
                  memcpy(failed,&failure,sizeof(int));
                  rval = send(i,failed,MAX_COMMAND_SIZE,FLAG);
                  close_conn = TRUE;
                  break;
                }

                if(dbug) printf("D: Handshake Accepted; Sending Maximum String Size\n");
                char str_size[MAX_COMMAND_SIZE];
                memset(str_size,0,MAX_COMMAND_SIZE);
                int temp = fd_fs_map[i]->get_file_name_size();
                memcpy(str_size,&temp,sizeof(int));

                rval = send(i, str_size, sizeof(int), FLAG);
                if(rval < 0)
                {
                  close_conn = TRUE;
                  std::cerr << "D: [ConnectionDaemon::main()] - Send To Client Failed" << std::endl;
                  std::cerr << "D: Closing Client Connection" << std::endl;
                  break;
                }


              }
              else
              {
                if(is_number(buffer))
                {
                  char* end;
                  int recv_command = (int)strtol(buffer, &end, 10);
                  current_command_id = recv_command;
                  if(dbug) printf("D: Current Command ID Set To: %d\n", current_command_id);
                  std::string response = "Command Accepted";
                  response = pad_string(response,MAX_COMMAND_SIZE - response.length(), '\0');
                  rval = send(i,response.c_str(),MAX_COMMAND_SIZE,FLAG);
                }
                else
                {
                  if(current_command_id == 0)
                  {
                    std::string failure = "failure -- Invalid Command ID";
                    failure = pad_string(failure, MAX_COMMAND_SIZE - failure.length(), '\0');
                    rval = send(i, failure.c_str(), MAX_COMMAND_SIZE, FLAG);
                  }
                  std::vector<std::string> data = execute(current_command_id, buffer,i);
                  //printf("Data Size: %lu\n",data.size());
                  for(unsigned int j = 0; j < data.size(); j++)
                  {
                    std::string temp = pad_string(data[j],(MAX_COMMAND_SIZE - data[j].length()), '\0');
                    rval = send(i, temp.c_str(), MAX_COMMAND_SIZE, FLAG);
                  }
                  if(current_command_id == FIND_TS || current_command_id == FIND_FS)
                  {
                    std::string done = "DONE";
                    done = pad_string(done, MAX_COMMAND_SIZE - done.length(), '\0');
                    rval = send(i, done.c_str(), MAX_COMMAND_SIZE, FLAG);
                  }
                }
              }

            }while(TRUE);

            if(close_conn)
            {
              if(dbug) printf("D: Removing Closed Connection - %d - From Master File Descriptor Set\n",i);
              close(i);
              FD_CLR(i, &master_set);
              if(i == max_fid)
              {
                while (FD_ISSET(max_fid, &master_set) == FALSE) max_fid -= 1;
              }
            }
          }
        }
      }
    //
    //
    //---------------- END ACCEPT/RECEIVE LOOP -----------------------------------------------




    }while(END_SERVER == FALSE);
  //
  //
  //
  //
  //---------------- END COMMUNICATIONS ------------------------------------------------------


  } // END try{}
  catch(arboreal_daemon_error e)
  {
    std::cerr << e.where() << e.what() << std::endl;
    printf("D: Closing All Open Connections and Exiting...\n");
    for (int i=0; i <= max_fid; ++i)
    {
      if (FD_ISSET(i, &master_set)) close(i);
    }
    printf("D: Goodbye\n");
    return 1;
  }
//
//
//
//
//---------------- END "MAIN" PROGRAM --------------------------------------------------------


  printf("D: Closing All Open Connections and Exiting...\n");
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
  }
  printf("D: Goodbye\n");
  return 0;
}




























