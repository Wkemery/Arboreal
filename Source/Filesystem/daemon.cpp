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
#include <chrono>
#include <ctime>

#include "DaemonDependancies/FileSystem/FileSystem.h"
#include "DaemonDependancies/File/File.h"
#include "DaemonHeaders/daemon.h"
#include "../SharedHeaders/Print.h"

/*************************************************************************************************/
#define STARTTUPDATA "Data/startup_time.txt"

/* MAIN */

int main(int argc, char** argv)
{

  signal(SIGABRT,sig_caught);
  signal(SIGTERM,sig_caught);
  signal(SIGINT,sig_caught);
  signal(SIGQUIT,sig_caught);
  signal(SIGSEGV,sig_caught);

  std::string arg;
  if(argc == 2)
  {
    arg = argv[1];
    if(arg == "-d") Debug.ON();
    else if(arg == "-v") verbose = true;
  }
  else if(argc == 3)
  {
    Debug.ON();
    verbose = true;
  }


//---------------- INITIALIZATIONS ----------------------------------------------------------------
//
//
//

  Debug.log("D: Launching File System...");
  try
  {
    d = new Disk(300000, 4096, const_cast<char *>("DISK1"));
    dm = new DiskManager(d);
  }
  catch(arboreal_exception& e)
  {
    std::cerr << "[Error]: " << e.what() << " in " << e.where() << std::endl;
    exit(1);
  }
  /***********************************************************************************************/


  fd_set working_set;
  struct sockaddr_in daemon_sockaddr;
  int daemon_sock, close_conn = 0;
  int sock_opt = 1;
  int on = 1;
  int rval = 0;
  int desc_ready, client_sock = 0;
  int END_SERVER = FALSE;
  int daemon_addrlen = sizeof(daemon_sockaddr);
  char buffer[MAX_COMMAND_SIZE];
  std::vector<int> active_connections;

//
//
//
//-------------------------------------------------------------------------------------------------




//---------------- BEGIN "MAIN" PROGRAM -----------------------------------------------------------
//
//
//
//
  try
  {
  //---------------- BEGIN SOCKET SETUP -----------------------------------------------------------
  //
  //

    memset(buffer,'\0',MAX_COMMAND_SIZE);
    memset(&daemon_sockaddr, 0, sizeof(daemon_sockaddr));

    Debug.log("D: Creating Daemon Socket...");
    daemon_sock = create_sock(TIMEOUT);
    my_fid = daemon_sock;
    Debug.log("D: Success");
    /*********************************************************************************************/


    Debug.log(("D: Daemon Socket ID: " + std::to_string(daemon_sock)));
    Debug.log("D: Setting Socket Options To Allow Port Reuse");
    set_socket_opt(daemon_sock,sock_opt,TIMEOUT);

    daemon_sockaddr.sin_family = AF_INET;
    daemon_sockaddr.sin_addr.s_addr = INADDR_ANY;
    daemon_sockaddr.sin_port = htons(PORT);

    Debug.log("D: Setting Socket To Non-Blocking Mode");
    set_nonblocking(daemon_sock,on);
    /*********************************************************************************************/


    Debug.log("D: Binding Socket To [" + std::to_string(daemon_sock) +
              "] To Port [" + std::to_string(PORT) + "]");

    bind_socket(daemon_sock,daemon_sockaddr,TIMEOUT);
    /*********************************************************************************************/


    Debug.log("D: Listening on Socket [" + std::to_string(daemon_sock) +"]" +
              "With a Backlog of [" + std::to_string(BACKLOG) + "]");

    listen_on_socket(daemon_sock,BACKLOG,TIMEOUT);
    /*********************************************************************************************/


    Debug.log("D: Initializing Master File Descriptor (Socket ID) Array");
    FD_ZERO(&master_set);
    max_fid = daemon_sock;
    FD_SET(daemon_sock,&master_set);

  //
  //
  //---------------- END SOCKET SET UP ------------------------------------------------------------



  //---------------- START THREADS ----------------------------------------------------------------
  //
  //

    /* Unsafe */
    // Debug.log("D: Launching [Quit] Thread...");
    // // Listens for "Q"/"q"/"quit" and quits daemon (for testing only)
    // std::thread quit(quit_fs);
    // quit.detach();

    // Debug.log("D: [Quit] Thread Started and Detatched From Main Process");
    /*********************************************************************************************/

    /* Unsafe */
    // Debug.log("D: Launching [Write Changes] Thread...");
    // std::thread write(save_to_disk);
    // write.detach();

    // Debug.log("D: [Write Changes] Thread Started and Detatched From Main Process");
  //
  //
  //-----------------------------------------------------------------------------------------------




  //---------------- BEGIN COMMUNICATIONS ---------------------------------------------------------
  //
  //
  //
  //
    do
    {
      Debug.log("D: Copying Master File Descriptor Set Into Working Set");
      memcpy(&working_set, &master_set, sizeof(master_set));

    //---------------- ATTEMPT SELECT() -----------------------------------------------------------
    //
    //

      Debug.log("D: Attempting Call To select()...");
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
      Debug.log("D: Success");
    //
    //
    //---------------------------------------------------------------------------------------------



    //---------------- BEGIN ACCEPTING AND RECEIVING LOOP -----------------------------------------
    //
    //
      Debug.log("D: Beginning Accept Loop");
      desc_ready = rval;
      for(int i = 0; i <= max_fid && desc_ready > 0; i++)
      {
        if(FD_ISSET(i,&working_set))
        {

          desc_ready -= 1;
          if(i == daemon_sock)
          {

            Debug.log("D: Listening Socket (Server) Is Readable; Now Accepting Connections");
            /*************************************************************************************/
            do
            {
              client_sock = accept(daemon_sock,NULL,NULL);
              /***********************************************************************************/


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
              /***********************************************************************************/


              Debug.log("D: New Incoming Connection -" +
                        std::to_string(client_sock) + " - Detected, Adding To Master Set");

              FD_SET(client_sock,&master_set);
              if(client_sock > max_fid) max_fid = client_sock;

            }while(client_sock != -1);

            /* END DO WHILE */
            /*************************************************************************************/

          }
          else
          {
            Debug.log("D: Client Descriptor - " + std::to_string(i) + " - Is Readable");
            Debug.log("D: Beginning Receive Loop");
            close_conn = FALSE;
            /*************************************************************************************/

            do
            {
              memset(buffer,'\0',MAX_COMMAND_SIZE);
              rval = recv(i, buffer, MAX_COMMAND_SIZE, FLAG);
              /***********************************************************************************/


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
              /***********************************************************************************/


              if(rval == 0)
              {
                Debug.log("D: Client Closed Connection; Breaking From Receive Loop");
                close_conn = TRUE;
                break;
              }
              /***********************************************************************************/


              int bytes_received = rval;

              Debug.log("D: Bytes Received [" + std::to_string(bytes_received) + "]");
              /***********************************************************************************/


              if(get_cmnd_id(buffer) == QUIT)
              {
                Debug.log("D: Client Closed Connection; Breaking From Receive Loop");
                close_conn = TRUE;
                break;
              }
              else if(get_cmnd_id(buffer) == 0)
              {
                std::string part = get_partition(buffer);
                Debug.log("D: Requested Partition [" + part + "]");
                /***********************************************************************************/

                try
                {
                  /* Attempt to create new FS object using the requested partition */
                  /*
                   * If an object with that partition exists already simply add a new
                   * entry to the map using a differet file descriptor but the SAME EXACT
                   * FS object that is already in the map
                   */

                  auto it = part_fs_map.find(part);
                  if(it == end(part_fs_map))
                  {
                    /*Timing Code*/
                    std::ofstream outfile;
                    outfile.open(STARTTUPDATA, std::ofstream::out | std::ofstream::app);
                    auto t_start = std::chrono::high_resolution_clock::now();
                    FileSystem* fs = new FileSystem(dm, part);
                    auto t_end = std::chrono::high_resolution_clock::now();
                    outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                            << fs->num_of_files() << " " << " " << fs->num_of_tags() <<endl;
                    /*Timing Code*/
                    fd_fs_map.insert(std::pair<int,FileSystem*>(i,fs));
                    part_fs_map.insert(std::pair<std::string,FileSystem*>(part,fd_fs_map[i]));
                  }
                  else
                  {
                    fd_fs_map.insert(std::pair<int,FileSystem*>(i,part_fs_map[part]));
                  }
                }
                catch(arboreal_exception& e)
                {
                  /* Problems Creating the FS Object */
                  std::cerr << "D: [Error]: " << e.where() << "--" << e.what() << std::endl;
                  std::cerr << "D: Closing File Descriptor [" << i << "]" << std::endl;

                  /* Avoid const char* errors */
                  std::string err = "D: [Error]: ";
                  err += e.where();
                  err += "--";
                  err += e.what();

                  Debug.log(err);
                  Debug.log("D: Closing File Descriptor [" + std::to_string(i) + "]");

                  char failed[MAX_COMMAND_SIZE];
                  int failure = FTL_ERR;
                  memset(failed,0,MAX_COMMAND_SIZE);
                  memcpy(failed,&failure,sizeof(int));
                  /*******************************************************************************/

                  rval = send(i,failed,MAX_COMMAND_SIZE,FLAG);
                  close_conn = TRUE;
                  break;
                }

                /* END TRY/CATCH */
                /*********************************************************************************/


                Debug.log("D: Handshake Accepted; Sending Maximum String Size");
                char str_size[MAX_COMMAND_SIZE];
                memset(str_size,0,MAX_COMMAND_SIZE);
                int temp = fd_fs_map[i]->get_file_name_size();
                memcpy(str_size,&temp,sizeof(int));
                /*********************************************************************************/


                rval = send(i, str_size, sizeof(int), FLAG);
                if(rval < 0)
                {
                  close_conn = TRUE;
                  std::cerr << "D: [ConnectionDaemon::main()] - Send To Client Failed" << std::endl;
                  std::cerr << "D: Closing Client Connection" << std::endl;

                  Debug.log("D: [ConnectionDaemon::main()] - Send To Client Failed");
                  Debug.log("D: Closing Client Connection");

                  break;
                }
                /*********************************************************************************/

              }
              else
              {
                std::string end_check = buffer;
                Debug.log("D: Command Received: " + end_check);

                if(is_number(buffer))
                {
                  /* Send command acceptance conformation and set current command ID */
                  /* The ID will not change until a different command is issued */
                  /*
                   * This is nescesarry because some commands come bundled as lists
                   * and it is important to continue using the same excecution steps for as long
                   * as the data is coming (provided that the incoming data is not a new command).
                   * this section checks the buffer to make sure it is not a number
                   * (i.e. a new command)
                   */
                  char* end;
                  int recv_command = (int)strtol(buffer, &end, 10);
                  current_command_id = recv_command;

                  Debug.log("D: Current Command ID Set To [" +
                             std::to_string(current_command_id) + "]");
                }
                else if(end_check == "$")
                {
                  Debug.log("D: Preparing Data For Sending...");
                  for(unsigned int i = 0; i < data.size(); i++)
                  {
                    data[i] += "\n";
                    data[i] = pad_string(data[i], MAX_COMMAND_SIZE - data[i].length(), '\0');
                  }
                  /*******************************************************************************/


                  int send_size = (data.size() * MAX_COMMAND_SIZE) + 1;
                  char send_size_buf[MAX_COMMAND_SIZE];
                  memset(send_size_buf,'\0',MAX_COMMAND_SIZE);
                  memcpy(send_size_buf,&send_size,sizeof(int));

                  Debug.log("D: Sending Read Amount [" + std::to_string(send_size) + "]");

                  rval = send(i, send_size_buf, MAX_COMMAND_SIZE, FLAG);
                  /*******************************************************************************/


                  char data_buf[send_size];
                  memset(data_buf,'\0',send_size);
                  int index = 0;
                  for(unsigned int i = 0; i < data.size(); i++)
                  {
                    for(unsigned int j = 0; j < data[i].size(); j++)
                    {
                      data_buf[index] = data[i][j];
                      index += 1;
                    }
                  }
                  /*******************************************************************************/


                  std::string temp_string;
                  for(unsigned int i = 0; i < send_size; i++){temp_string += data_buf[i];}
                  Debug.log("D: Sending Data: \n" + temp_string);

                  rval = send(i, data_buf, send_size, FLAG);

                  data.erase(begin(data),end(data));
                  /*******************************************************************************/

                }
                else
                {
                  if(current_command_id == 0)
                  {
                    std::string failure = "Failure -- Invalid Command ID";
                    failure = pad_string(failure, MAX_COMMAND_SIZE - failure.length(), '\0');
                    rval = send(i, failure.c_str(), MAX_COMMAND_SIZE, FLAG);
                  }
                  /*******************************************************************************/


                  /* Execute the command */
                  execute(current_command_id, buffer, i, data);
                  /*******************************************************************************/
                }
              }

            }while(TRUE);

            /* END DO/WHILE */
            /*************************************************************************************/


            /* Connection ended remove from active set */
            if(close_conn)
            {
              Debug.log("D: Removing Closed Connection - " + std::to_string(i) +
                "- From Master File Descriptor Set");

              close(i);
              FD_CLR(i, &master_set);
              if(i == max_fid)
              {
                while (FD_ISSET(max_fid, &master_set) == FALSE) max_fid -= 1;
              }
            }
            /**************************************************************************************/

          }
        }
      }
    //
    //
    //---------------- END ACCEPT/RECEIVE LOOP ----------------------------------------------------




  }while(END_SERVER == FALSE);
  //
  //
  //
  //
  //---------------- END COMMUNICATIONS -----------------------------------------------------------


  } // END try{}
  catch(arboreal_daemon_error e)
  {
    std::cerr << e.where() << e.what() << std::endl;

    std::string err = "D: [Error]: ";
    err += e.where();
    err += "--";
    err += e.what();

    Debug.log(err);
    Debug.log("D: Closing All Open Connections and Exiting");
    printf("D: Closing All Open Connections and Exiting...\n");

    for(auto it = begin(fd_fs_map); it != end(fd_fs_map); ++it)
    {
      it->second->write_changes();
    }

    quit_writing = true;

    /*
     * Prevent Abort Trabs and Segfaults:
     * The Quit thread does not quit fast enough to avoid an
     * exception being thrown due to a bad call to select thus
     * The quit thread deletes everything, then the exception handling
     * tries to delete everything and it throws all kinds of segfaults
     * in order to stop this from happening it is nescesarry to
     * either reimagine how the quit thread works or (more easily)
     * guard the deletion of items in the exception handling; using a boolean
     * flag which is set by the quit_thread so that if the quit thread intercepts
     * a quit signal it tells the exception handler to NOT delete any of the data
     * (since it will be doing the deleteing) however in circumstances were
     * quit was not called but an exception still occurs it is nescesarry to properly
     * delete all of the pointers and the like.
     */
    if(!quit_signaled)
    {
      for (int i=0; i <= max_fid; ++i)
      {
        if (FD_ISSET(i, &master_set)) close(i);
      }
      for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
      {
        delete it->second;
      }
    }
    printf("D: Goodbye\n");
    return 1;
  }
//
//
//
//
//---------------- END "MAIN" PROGRAM -------------------------------------------------------------


  Debug.log("D: Closing All Open Connections and Exiting");
  printf("D: Closing All Open Connections and Exiting...\n");
  /************************************************************************************************/

  for(auto it = begin(fd_fs_map); it != end(fd_fs_map); ++it)
  {
    it->second->write_changes();
  }

  quit_writing = true;
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
  }
  /************************************************************************************************/

  printf("D: Goodbye\n");
  return 0;
}
