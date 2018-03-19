////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.h
//  Liason headerfile
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef LIAISON_H
#define LIAISON_H

#define NEW_PLUS "n+"



void clean(int signal)
{
  std::cout << "SIGNAL: " << signal << std::endl;
  system("rm *socket");
  exit(0);
}
//[================================================================================================]
//[================================================================================================]
void bad_clean(int signal)
{
  system("rm *socket");
  exit(1);
}
//[================================================================================================]
//[================================================================================================]
void seg_fault(int signal)
{
  std::cout << "SEGFAULT: " << signal << std::endl;
  system("rm *socket");
  exit(1);
}
//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
//
// @ cmnd : The command buffer
//[================================================================================================]
int get_cmnd_id(const char* cmnd)
{
    char temp[sizeof(int)];
    for(unsigned int i = 0; i < sizeof(int); i++)
    {
        temp[i] = cmnd[i];
    }

    int* id = (int*)temp;
    return *id;
}
//[================================================================================================]
//[================================================================================================]
std::string get_command_string(const char* cmnd, const int size)
{
    std::string command;
    int index = sizeof(int);
    while(index < size)
    {
        command += cmnd[index];
        index += 1;
    }

    return command;
}
//[================================================================================================]
//[================================================================================================]
std::string pad_string(const std::string string, const int size, const char value)
{
    std::string padded = string;
    for(unsigned int i = 0; i < size; i++)
    {
        padded += value;
    }
    return padded;
}
//[================================================================================================]
//[================================================================================================]
// Request and attach to, a shared memory segment with a specific key.
// The shared memory segment will be used to synchronize the command line interface
// and this liason process.  (Note that the only difference between the shmget() of
// the Command Line Process and the Liaison Process is the lack of IPC_CREAT as one
// of the flags passed.  IPC_CREAT will create a new fragment leaving it off only)
//
// @ key: The unique key required to access the specific shared memory segment
//        This is passed as a parameter from the CLI to the Liason process via main() arguments
// @ id:  A reference to an integer in which to store the shared memory id that shmget() returns
//[================================================================================================]
char* get_shm_seg(const key_t key, int& id)
{
    int shm_id;
    char*  shm;

    /* Create Shared Memory Segment for Process Synchronization */
    if ((shm_id = shmget(key, SharedMemorySize, Permissions)) < 0)
    {
        std::string where = "[liason_helper.hpp::get_shm_seg()]: ";
        std::string what = "Shared Memory Get Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    /* Attach the shared memory to this process so the Liaison can access it */
    if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1)
    {
        std::string where = "[liason_helper.hpp::get_shm_seg()]: ";
        std::string what = "Shared Memory Attach Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    id = shm_id;

    /* Return the shared memory */
    return shm;
}
//[================================================================================================]
//[================================================================================================]
// Un-attach the shared memory segment from this process.
// (Process will not be able to access the shared memory segment until it is reattached)
// (Additionally a shared memory segment can only be removed once nothing is attached to it)
//
// @ shm_id: The id of the shared memory segement that will be detatched
// @ shm:    The actual pointer to the shared memory segment
//[================================================================================================]
void unat_shm(const int shm_id, const char* shm)
{
    if(shmdt(shm) == -1)
    {
        std::string where = "[liason_helper.hpp::unat_shm()]: ";
        std::string what = "Shared Memory Unattach Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

}
//[================================================================================================]
//[================================================================================================]
// Set up a server socket to receive incoming connections
//
// @ server_sockpath: The pathname fo the server's socket
//                    (In this case the pathame will not be static, as each CLI process will fork
//                     its own Liaison process, therefore the server pathname is passed as an
//                     argument to the Liaison process' main() function)
// @ server_sockaddr: A reference to a standard structure whose components I will not describe here
//                    and can be viewed in a Unix manual.  Suffice it to say, it stores the socket
//                    type and the socket path.  (Note that the "type" of the struct is sockaddr_un
//                    signifing that this is a unix domain socket)
//[================================================================================================]
int set_up_socket(std::string server_sockpath, struct sockaddr_un& server_sockaddr)
{
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        std::string where = "[liason_helper.hpp::set_up_socket()]: ";
        std::string what = "Server Socket Create Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, server_sockpath.c_str());
    socklen_t length = sizeof(server_sockaddr);

    /* This should fail and so does NOT throw an exception */
    /* If it does not fail that's fine too */
    unlink(server_sockpath.c_str());


    if(bind(server_sock, (struct sockaddr *) &server_sockaddr, length) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::set_up_socket()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::set_up_socket()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::set_up_socket()]: ";
        std::string what = "Server Socket Bind Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    return server_sock;
}
//[================================================================================================]
//[================================================================================================]
// Mark the server socket as open for buisness (i.e. capable of accepting connections)
// The Server can queue up X number of connection requests were X = Backlog
//
// @ server_sock: This server socket's identifier
// @ server_sockpath: This server socket's pathname
//[================================================================================================]
void listen_for_client(const int server_sock, const std::string server_sockpath)
{
    if(listen(server_sock, Backlog) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::listen_for_client()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::listen_for_client()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::listen_for_client()]: ";
        std::string what = "Server Socket Listen Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    return;
}
//[================================================================================================]
//[================================================================================================]
// Accept a connection request, returns the client socket's identifier
//
// @ server_sock: This server socket's identifier
// @ client_sockaddr: A reference to a standard structure whose components I will not describe here
//                    and can be viewed in a Unix manual.  Suffice it to say, it stores the socket
//                    type and the socket path.  (Note that the "type" of the struct is sockaddr_un
//                    signifing that this is a unix domain socket).  This will store the connecting
//                    client's information
// @ length: The size of the server_sockaddr (This must be the size of the whole structure not
//           just a single part and is most easily retrieved via a call to sizeof() )
// @ server_sockpath: This server socket's pathname
//[================================================================================================]
int accept_client(int server_sock, struct sockaddr_un& client_sockaddr, 
                  socklen_t length, std::string server_sockpath)
{
    int client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &length);
    if(client_sock < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::accept_client()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(close(client_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::accept_client()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::accept_client()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::accept_client()]: ";
        std::string what = "Accept Client Connection Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    return client_sock;
}
//[================================================================================================]
//[================================================================================================]
// Retrieve a accepted client's information for use in send/receive functionality
//
// @ client_sock: The client socket's identifier
// @ client_sockaddr: A reference to a standard structure whose components I will not describe here
//                    and can be viewed in a Unix manual.  Suffice it to say, it stores the socket
//                    type and the socket path.  (Note that the "type" of the struct is sockaddr_un
//                    signifing that this is a unix domain socket).  This will store the connecting
//                    client's information
// @ server_sock: This server socket's identifier
// @ server_sockpath: This server socket's pathname
//[================================================================================================]
void get_peername(const int client_sock, const struct sockaddr_un& client_sockaddr, 
                  const int server_sock, const std::string server_sockpath)
{
    socklen_t length = sizeof(client_sockaddr);

    if(getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &length) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::get_peername()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(close(client_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::get_peername()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::get_peername()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::get_peername()]: ";
        std::string what = "Get Peername Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(where,what);
    }

    return;
}
//[================================================================================================]
//[================================================================================================]
// Receive a message from an accepted socket. (Note that the client/server pathnames and the server
// socket id are only used when an exception is thrown in order to correctly close the socket)
//
// @ client_sock: The client socket's identifier
// @ size:        The size of the message to be received
// @ flag:        Any flags for the recv() function (see 'man recv')
// @ server_sock: This server socket's identifier
// @ server_sockpath: This server socket's pathname
// @ client_sockpath: The client socket's pathname
//[================================================================================================]
char* recv_msg(const int client_sock, const int size, const int flag, const int server_sock, 
               const std::string server_sockpath, const std::string client_sockpath)
{
    char* msg = new char[size];
    memset(msg,'\0',size);

    int bytes_rec = recv(client_sock, msg, size, flag);
    if(bytes_rec < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::recv_msg()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(close(client_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::recv_msg()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::recv_msg()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::recv_msg()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::recv_msg()]: ";
        std::string what = "Receive Message From Client Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }

    return msg;
}
//[================================================================================================]
//[================================================================================================]
// Send a response to an accepted socket (Note that the client/server pathnames and the server
// socket id are only used when an exception is thrown in order to correctly close the socket)
//
// @ client_sock: The client socket's identifier
// @ size:        The size of the message to be received
// @ flag:        Any flags for the recv() function (see 'man recv')
// @ server_sock: This server socket's identifier
// @ server_sockpath: This server socket's pathname
// @ client_sockpath: The client socket's pathname
//[================================================================================================]
void send_response(const int client_sock, const char* data, const int size, const int flag,
                   const int server_sock, const std::string server_sockpath, const std::string client_sockpath)
{
    if(send(client_sock, data, size, flag) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::send_response()]: ";
            std::string what = "Server Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(close(client_sock) < 0)
        {
            std::string where = "[liason_helper.hpp::send_response()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::send_response()]: ";
            std::string what = "Server Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[liason_helper.hpp::send_response()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_liaison_error(what,where);
        }
        std::string where = "[liason_helper.hpp::send_response()]: ";
        std::string what = "Send Response To Client Failed -- ";
        what += strerror(errno);
        throw arboreal_liaison_error(what,where);
    }
}
//[================================================================================================]
//[================================================================================================]
void shutdown(const int liaison_fid,
              const int client_sock,
              const std::string client_sockpath,
              const int liaison_sock,
              const std::string liaison_sockpath)
{

    Debug.log(liaison_close);
    if(close(liaison_fid) < 0)
    {
      std::string where = "[liason.cpp::main()]: ";
      std::string what = "Liaison Client Socket Close Failed -- ";
      what += strerror(errno);
      throw arboreal_liaison_error(what,where);
    }
    if(close(liaison_sock) < 0)
    {
      std::string where = "[liason.cpp::main()]: ";
      std::string what = "Liaison Server Socket Close Failed -- ";
      what += strerror(errno);
      throw arboreal_liaison_error(what,where);
    }
    if(unlink(liaison_sockpath.c_str()) < 0)
    {
      std::string where = "[liason.cpp::main()]: ";
      std::string what = "Liaison Server Socket Unlink Failed -- ";
      what += strerror(errno);
      throw arboreal_liaison_error(what,where);
    }

    if(Parser != 0){delete Parser;}
    char* quit = new char[MaxBufferSize];
    memset(quit,'\0',MaxBufferSize);
    int val = 999;
    memcpy(quit,&val,sizeof(int));
    memcpy(quit + sizeof(int), "QUIT", sizeof("QUIT"));
    Debug.log(liaison_quit);
    send_response(client_sock,quit,MaxBufferSize,Flag,liaison_sock,liaison_sockpath,client_sockpath);
    exit(1);

}
//[================================================================================================]
//[================================================================================================]
int create_daemon_sock( const int client_sock,
                        const std::string client_sockpath,
                        const int liaison_sock, 
                        const std::string liaison_sockpath)
{
    int printer = 0;
    int timer = 0;
    int liaison_fid;
    while((liaison_fid = socket(AF_INET,SOCK_STREAM,0)) < 0 && timer < Timeout)
    {
      sleep(1);
      if(printer % 3 == 0)
      {
        printf("\33[2K\r");
        printf("\rL: Liaison Client Socket Creation Failed - %s - Retrying.", strerror(errno));
        fflush(stdout);
      }
      else if(printer % 3 == 1)
      {
        printf("\33[2K\r");
        printf("\rL: Liaison Client Socket Creation Failed - %s - Retrying..", strerror(errno));
        fflush(stdout);
      }
      else if(printer % 3 == 2)
      {
        printf("\33[2K\r");
        printf("\rL: Liaison Client Socket Creation Failed - %s - Retrying...", strerror(errno));
        fflush(stdout);
      }
      timer += 1;
      printer += 1;
    }
    if(timer >= Timeout)
    {
        printf("\nL: Connection To File System Daemon Could Not Be Established; Exiting\n");
        shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);
    }
    return liaison_fid;
}
//[================================================================================================]
//[================================================================================================]
void connect_to_daemon(int liaison_fid, struct sockaddr_in daemon_addr, 
                        const int client_sock,
                        const std::string client_sockpath,
                        const int liaison_sock, 
                        const std::string liaison_sockpath)
{
    int timer = 0;
    int printer = 0;
    while((connect(liaison_fid,(struct sockaddr*)&daemon_addr,sizeof(daemon_addr))) < 0 && timer < Timeout)
    {
      sleep(1);
      if(printer % 3 == 0)
      {
        printf("\33[2K\r");
        printf("\rL: Connection To File System Daemon Failed - %s - Retrying.",strerror(errno));
        fflush(stdout);
      }
      else if(printer % 3 == 1)
      {
        printf("\33[2K\r");
        printf("\rL: Connection To File System Daemon Failed - %s - Retrying..",strerror(errno));
        fflush(stdout);
      }
      else if(printer % 3 == 2)
      {
        printf("\33[2K\r");
        printf("\rL: Connection To File System Daemon Failed - %s - Retrying...",strerror(errno));
        fflush(stdout);
      }
      printer += 1;
      timer += 1;
    }
    if(timer >= Timeout)
    {
        printf("\nL: Connection To File System Daemon Could Not Be Established; Exiting\n");
        shutdown(liaison_fid, client_sock, client_sockpath, liaison_sock, liaison_sockpath);
    }
}
//[================================================================================================]
//[================================================================================================]
#endif
