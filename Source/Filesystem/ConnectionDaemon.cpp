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

#include <string>                           /* Strings */
#include <iostream>                         /* cout */
#include <vector>                           /* Vectors */
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

#include "Backend/FileSystem.h"
#include "types.h"

#define BACKLOG 10                  /* Number of Connection Requests that the Server Can Queue */
#define FLAG 0                      /* Flag for recv() */
#define TIMEOUT 10
#define TRUE 1
#define FALSE 0
#define PORT 70777

bool DEBUG = false;


/* Declarations */
int create_sock(int timeout);
void set_socket_opt(int daemon_sock, int sock_opt, int timeout);
void bind_socket(int daemon_sock, struct sockaddr_in daemon_sockaddr, int timeout);
void listen_on_socket(int daemon_sock,int backlog,int timeout);
void set_nonblocking(int daemon_sock, int is_on);
void quit_fs(void);
void sig_caught(int sig);




/***************** BEGIN MAIN *******************/

fd_set master_set;
int my_fid = 999;
int max_fid = 0;

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
  if(dbug) printf("D: Getting Max String Size...\n");
  unsigned int max_string_size = 64; // Will be set by a call to FS
  if(dbug) printf("D: Max String Size: %d\n", max_string_size);
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
    unsigned int max_command_size = (sizeof(int) + max_string_size * 2 + 1);
    if(dbug) printf("D: Max Command Size: %d\n",max_command_size);
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
    char buffer[max_command_size];

    if(dbug) printf("D: Zeroing Socket Address and Buffer\n");
    memset(buffer,'\0',max_command_size);
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
            printf("D: Client Descriptor - %d - Is Readable\n",i);
            if(dbug) printf("D: Beginning Receive Loop...\n");
            close_conn = FALSE;

            do
            {
              rval = recv(i, buffer, max_command_size, FLAG);
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
                printf("D: Client Closed Connection; Breaking From Receive Loop\n");
                close_conn = TRUE;
                break;
              }

              int bytes_received = rval;
              if(dbug) printf("\nBytes Received: %d\n",bytes_received);
              if(dbug) printf("Command Received: %s\n\n",buffer);

              // call arboreal functions and return data

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
    std::cerr << e.where() << std::endl << e.what() << std::endl;
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
  printf("D: Goodbye\n");
  return 0;
}













//////////////////////////////////////////////////////////////////////////////////////////////
//---------------- BEGIN FUNCTION DEFINITIONS ------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
///
///
void sig_caught(int sig)
{
  printf("\nD: Received - %s\n", strsignal(sig));
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  exit(-1);
}
//--------------------------------------------------------------------------------------------



void quit_fs(void)
{
  std::string input;
  while(true)
  {
    std::cin >> input;
    if(input == "Q" || input == "q" || input == "quit") break;
  }
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  return;
}
//--------------------------------------------------------------------------------------------



int create_sock(int timeout)
{
  int daemon_sock = 0;
  int timer = 0;
  int printer = 0;
  while(((daemon_sock = socket(AF_INET,SOCK_STREAM,0)) <= 0) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    std::string where = "[ConnectionDaemon.cpp::create_sock()]: ";
    std::string what = "Socket Creation Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(what,where);
  }
  return daemon_sock;
}
//--------------------------------------------------------------------------------------------



void set_socket_opt(int daemon_sock, int sock_opt, int timeout)
{
  int timer = 0;
  int printer = 0;
  while((setsockopt(daemon_sock,SOL_SOCKET,SO_REUSEADDR,&sock_opt,sizeof(sock_opt))) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::set_socket_opt()]: ";
    std::string what = "Set Socket Options Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//--------------------------------------------------------------------------------------------



void set_nonblocking(int daemon_sock, int is_on)
{
  int rval = ioctl(daemon_sock,FIONBIO,(char*)&is_on);
  if(rval < 0)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::set_nonblocking()]: ";
    std::string what = "Set Non-Blocking Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
}
//--------------------------------------------------------------------------------------------



void bind_socket(int daemon_sock, struct sockaddr_in daemon_sockaddr, int timeout)
{
  int timer = 0;
  int printer = 0;
  int rval = 0;
  while((rval = bind(daemon_sock, (struct sockaddr*)&daemon_sockaddr,sizeof(daemon_sockaddr))) < 0 && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying...", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying...", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::bind_socket()]: ";
    std::string what = "Bind Socket Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//--------------------------------------------------------------------------------------------



void listen_on_socket(int daemon_sock,int backlog,int timeout)
{
  int timer = 0;
  int printer = 0;
  while((listen(daemon_sock,backlog) < 0) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer > 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::listen_on_socket()]: ";
    std::string what = "Listen On Socket Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//--------------------------------------------------------------------------------------------
