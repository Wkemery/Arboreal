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
#include <iostream>                         /* Input & Output */
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
#include <algorithm>

#include "Backend/FileSystem.h"
#include "types.h"
#include "../CLI/Parser.h"
#include "File.h"

#define BACKLOG 10                  /* Number of Connection Requests that the Server Can Queue */
#define FLAG 0                      /* Flag for recv() */
#define TIMEOUT 10
#define TRUE 1
#define FALSE 0
#define PORT 70777
#define MAX_COMMAND_SIZE 4096

bool DEBUG = false;


/* Declarations */
int create_sock(int timeout);
void set_socket_opt(int daemon_sock, int sock_opt, int timeout);
void bind_socket(int daemon_sock, struct sockaddr_in daemon_sockaddr, int timeout);
void listen_on_socket(int daemon_sock,int backlog,int timeout);
void set_nonblocking(int daemon_sock, int is_on);
void quit_fs(void);
void sig_caught(int sig);
int get_cmnd_id(char* cmnd);
std::string get_partition(char* cmnd);
bool is_number(const char* str);
std::vector<std::string> execute(int id, char* command, int i);
std::string pad_string(std::string string, int size, char value);
std::unordered_set<std::string> get_set(char* command, char delim);
std::vector<std::string> serialize_fileinfo(std::vector<FileInfo*>* fileinfo);
std::string get_file_info(File* file);




/***************** BEGIN MAIN *******************/

fd_set master_set;
int my_fid = 999;
int max_fid = 0;
int current_command_id = 0;

std::map<int, FileSystem*> fd_fs_map;
std::map<std::string,FileSystem*> part_fs_map;
std::map<std::string, unsigned int> path_filedesc_map;

Disk* d = 0;
DiskManager* dm = 0;

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

              if(get_cmnd_id(buffer) == 999)
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
                  int failure = 9999;
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
                  printf("Data Size: %lu\n",data.size());
                  for(unsigned int j = 0; j < data.size(); j++)
                  {
                    std::string temp = pad_string(data[j],(MAX_COMMAND_SIZE - data[j].length()), '\0');
                    rval = send(i, temp.c_str(), MAX_COMMAND_SIZE, FLAG);
                  }
                  if(current_command_id == 5 || current_command_id == 4)
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













//////////////////////////////////////////////////////////////////////////////////////////////
//---------------- BEGIN FUNCTION DEFINITIONS ------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
///
///
void sig_caught(int sig)
{
  printf("\nD: [Fatal Error] Daemon Received Signal - %s\n", strsignal(sig));
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
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
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
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

//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
//
// @ cmnd : The command buffer
//[================================================================================================]
int get_cmnd_id(char* cmnd)
{
    char temp[sizeof(int)];
    for(unsigned int i = 0; i < sizeof(int); i++)
    {
        temp[i] = cmnd[i];
    }

    int* id = (int*)temp;
    return *id;
}
//--------------------------------------------------------------------------------------------

std::string get_partition(char* cmnd)
{
  std::string temp = "";
  int index = (int)sizeof(int);
  while(cmnd[index] != '-'){index += 1;}
  index += 1;
  while(cmnd[index] != '\0')
  {
    temp += cmnd[index];
    index += 1;
  }
  return temp;
}

bool is_number(const char* str)
{
  char* ptr;
  strtol(str, &ptr, 10);
  return *ptr == '\0';
}

std::string pad_string(std::string string, int size, char value)
{
    std::string padded = string;
  for(unsigned int i = 0; i < size; i++)
  {
    padded += value;
  }
  return padded;
}





std::vector<std::string> execute(int id, char* command, int fd)
{
  std::vector<std::string> data;
  switch(id)
  {
    case(4): // find tag
    {
      std::unordered_set<std::string> tags = get_set(command,',');
      std::vector<FileInfo*>* rval;
      try
      {
        rval = fd_fs_map[fd]->tag_search(tags);
        data = serialize_fileinfo(rval);
        if(data.size() == 0)
        {
          data.push_back("Tag Exists But Has No Associated Files");
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }

      delete rval;
      return data;
    }
    case(5): //find file
    { 
      std::string file = command;
      std::vector<FileInfo*>* rval;
      try
      {
        rval = fd_fs_map[fd]->file_search(file);
        data = serialize_fileinfo(rval);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }

      delete rval;
      return data;
    }
    case(6): // Create Tag
    {
      std::string tag = command;
      try
      {
        fd_fs_map[fd]->create_tag(tag);
        std::string success = "New Tag [";
        success += (tag + "] Created");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(7): //create file in CWD
    {
      std::unordered_set<std::string> tags;
      std::string filename;
      char tag[MAX_COMMAND_SIZE];
      memset(tag,'\0',MAX_COMMAND_SIZE);
      int index = 0;
      while(command[index] != '-'){filename += command[index]; index += 1;}
      printf("Filename: %s\n",filename.c_str());
      memcpy(tag,(command + filename.length() + 1), MAX_COMMAND_SIZE - (filename.length() + 1));
      tags = get_set(tag,'-');

      try
      {
        FileInfo* finfo = fd_fs_map[fd]->create_file(filename,tags);
        if(finfo != 0)
        {
          File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(info));
        }
        else
        {
          std::string failure = "Creation of Requested File [";
          failure += (filename + "] Failed\n");
          failure += "Potential Causes Include: A Full Disk, One Or More Of The Specified Tags Do Not Exist, \
          File Already Exists, Internal Logic Error\n";

          data.push_back(failure);
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(8): // create file anywhere
    {
      std::unordered_set<std::string> tags;
      std::string filename;
      char tag[MAX_COMMAND_SIZE];
      memset(tag,'\0',MAX_COMMAND_SIZE);
      int index = 0;
      while(command[index] != '-'){filename += command[index]; index += 1;}
      printf("Filename: %s\n",filename.c_str());
      memcpy(tag,(command + filename.length() + 1), MAX_COMMAND_SIZE - (filename.length() + 1));
      tags = get_set(tag,'-');

      try
      {
        FileInfo* finfo = fd_fs_map[fd]->create_file(filename,tags);
        if(finfo != 0)
        {
          File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(info));
        }
        else
        {
          std::string failure = "Creation of Requested File [";
          failure += (filename + "] Failed\n");
          failure += "Potential Causes Include: A Full Disk, One Or More Of The Specified Tags Do Not Exist, \
          File Already Exists, Internal Logic Error\n";

          data.push_back(failure);
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(9): // delete tag(s)
    {
      std::string tagname = command;
  
      try
      {
        fd_fs_map[fd]->delete_tag(tagname);
        std::string success = ("Tag [" + tagname + "] Successfully Deleted; Current Working Directory Changed To [ / ]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(10): // delete files from CWD
    {
      std::string to_delete = command;
      std::vector<std::string> path = Parser::split_on_delim(to_delete,'-');
  
      try
      {
        fd_fs_map[fd]->delete_file(path);
        std::string success = ("File [" + path[path.size()-1] +"] Successfully Deleted");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(11): // delete file from anywhere
    {
      std::string filepath = command;
      std::vector<std::string> path = Parser::split_on_delim(filepath,'/');
      for(uint i = 0; i < path.size(); i++)
      {
        std::cout << path[i] << std::endl;
      }
      try
      {
        fd_fs_map[fd]->delete_file(path);
        std::string success = ("File [" + path[path.size()-1] + "] Successfully Deleted; Current Working Directory Changed To [ / ]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(12): // Open file
    {
      std::string temp = command;
      std::vector<std::string> info = Parser::split_on_delim(temp,'/');

      char mode = info[0].c_str()[0];
      printf("Mode: %c\n",mode);


      std::string path = temp.substr(2,temp.length());
      printf("Path: %s\n",path.c_str());

      info.erase(begin(info));
      printf("Info Size: %lu\n",info.size());
      for(unsigned int i = 0; i < info.size(); i++)
      {
        std::cout << info[i] << std::endl;
      }

      try
      {
        unsigned int filedesc = 0;
        filedesc = fd_fs_map[fd]->open_file(info,mode);
        auto it = path_filedesc_map.find(path);
        if(filedesc != 0 && it == end(path_filedesc_map))
        {
          path_filedesc_map.insert(std::pair<std::string, unsigned int>(path,filedesc));
        }
        std::string success = ("File [" + info[info.size()-1] + "] Opened Successfuly For ");
        if(mode == 'r'){success += "Read";}
        else if(mode == 'w'){success += "Write";}
        else if(mode == 'x'){success += "Read and Write";}

        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }

      return data;
    }
    case(13): // close file
    {
      std::string p = command;
      std::vector<std::string> path = Parser::split_on_delim(p,'/');

      try
      {
        fd_fs_map[fd]->close_file(path_filedesc_map[p]);
        std::string success = ("File [" + path[path.size()-1] + "] Closed Successfully");
        path_filedesc_map.erase(p);
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      return data;
    }
    case(14): // Rename tag
    {
      std::string rename = command;
      std::vector<std::string> names = Parser::split_on_delim(rename,'-');
      for(unsigned int i = 0; i < names.size(); i++)
      {
        std::cout << names[i] << std::endl;
      }
      try
      {
        fd_fs_map[fd]->rename_tag(names[0],names[1]);
        std::string success = ("Tag [" + names[0] + "] Successfully Renamed To [" + names[1] + "]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(15): // rename file
    {
      // NEED TO TEST ONCE RNAME FILE IS FIXED
      std::string to_split = command;
      std::vector<std::string> split = Parser::split_on_delim(to_split,'/');
      std::string new_name = split[split.size() - 1];
      std::cout << new_name << std::endl;
      std::cout << split.size() << std::endl;
      split.erase(end(split) - 1);
      std::cout << split.size() << std::endl;

      try
      {
        fd_fs_map[fd]->rename_file(split,new_name);
        std::string success = ("File [" + split[split.size() - 1] + "] Successfully Renamed To [" + new_name +"]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(16): //get file attr
    {
      std::string path = command;
      std::vector<std::string> vpath = Parser::split_on_delim(path,'/');

      try
      {
        Attributes* attr = fd_fs_map[fd]->get_attributes(vpath);
        FileAttributes fattr = attr->get_file_attributes();
        std::string t1 = ("[ Name -- " + vpath[vpath.size() - 1] + "]\n");

        std::tm * ptm = std::localtime(&fattr.creationTime);
        char buffer[32];
        memset(buffer,'\0',32);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        std::string buf = buffer;

        std::string t2 = ("[ Created -- " + buf + "]\n");

        memset(buffer, '\0', 32);
        ptm = std::localtime(&fattr.lastEdit);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        buf = buffer;

        std::string t3 = ("[ Last Edit -- " + buf + "]\n");

        memset(buffer, '\0', 32);
        ptm = std::localtime(&fattr.lastAccess);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        buf = buffer;
        std::string t4 = ("[ Last Accessed -- " + buf + "]\n");

        int size = (int)fattr.size;
        std::string t5 = ("[ Size -- " + std::to_string(size) + "]\n");

        char perm[12];
        for(unsigned int i = 0; i < 12; i++){perm[i] = fattr.permissions[i];}
        std::string t6 = ("[ Permissions -- [");
        for(unsigned int i = 0; i < 12; i++)
        {
          if(i + 1 == 12){t6 += perm[i];}
          else{t6 += perm[i]; t6 += ",";}
        }
        t6 += "]\n";

        std::string t7 = ("[ Owner -- " + std::to_string(fattr.owner) + "]\n");

        std::string success = t1 + t2 + t3 + t4 + t5 + t6 + t7;
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      return data;
    }
  }

}


std::unordered_set<std::string> get_set(char* command, char delim)
{
  std::string exec = command;
  std::vector<std::string> temp = Parser::split_on_delim(exec,delim);
  std::unordered_set<std::string> tags;
  
  for(unsigned int i = 0; i < temp.size(); i++)
  {
    printf("Split: [%d]: %s\n",i,temp[i].c_str());
    if(temp[i] != ""){tags.emplace(temp[i]);}
  }
  return tags;
}

std::vector<std::string> serialize_fileinfo(std::vector<FileInfo*>* fileinfo)
{
  std::vector<std::string> data;
  if(fileinfo != 0)
  {
    for(unsigned int i = 0; i < fileinfo->size(); i++)
    {
      if(fileinfo->at(i) != 0)
      {
        File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(fileinfo->at(i))->c_str()));
        data.push_back(get_file_info(info));
      }
      else{continue;}
    }
  }
  else
  {
    throw arboreal_daemon_error("Bad Vector Pointer","[ConnectionDaemon.cpp::execute()]");
    return data;
  }

  return data;
}


std::string get_file_info(File* file)
{
  std::string file_info = ("[ " + file->get_name() + " | ");
  std::vector<std::string> tags = file->get_tags();
  for(unsigned int i = 0; i < tags.size(); i++)
  {
    if(file_info.length() + 5 == MAX_COMMAND_SIZE)
    {
      file_info += " ...]";
      return file_info;
    }
    if(i + 1 != tags.size()){file_info += (tags[i] + ",");}
    else{file_info += tags[i];}
  }

  //file_info += " |...]";

  if(file_info.length() < MAX_COMMAND_SIZE)
  {
    file_info += " | ";

    FileAttributes attr = file->get_attributes();

    std::tm * ptm = std::localtime(&attr.creationTime);
    char buffer[32];
    memset(buffer,'\0',32);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm); 

    file_info += "Created @ ";
    file_info += buffer;
  
    file_info += " | ";

    memset(buffer,'\0',32);
    ptm = std::localtime(&attr.lastEdit);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm); 
    file_info += "Last Edit @ ";
    file_info += buffer;
  
    file_info += " ...]";
  }

  return file_info;
}














