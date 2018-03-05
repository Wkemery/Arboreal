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
//[================================================================================================]
//[================================================================================================]
// Print a command buffer
// 
// @ cmnd: The command buffer
// @ size: The size of the command buffer
//[================================================================================================]
void print_cmnd(char* cmnd, int size)
{
    std::cout << get_cmnd_id(cmnd) << " ";
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        std::cout << cmnd[i];
    }
    std::cout << "\n";
}
//[================================================================================================]
//[================================================================================================]
template <typename T> 
void print_vector(std::vector<T> vec)
{
    std::cout << "Vector: " << std::endl;
    for(unsigned int i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << std::endl;
    }
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
char* get_shm_seg(key_t key, int& id)
{
    int shm_id;
    char*  shm;

    /* Create Shared Memory Segment for Process Synchronization */
    if ((shm_id = shmget(key, SHMSZ, PERMISSIONS)) < 0) 
    {
        throw ERR(2,SHM_GET_ERR,47);
    }

    /* Attach the shared memory to this process so the Liaison can access it */
    if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1) 
    {
        throw ERR(2,SHM_ATT_ERR,53);  
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
void unat_shm(int shm_id, char* shm)
{
    if(shmdt(shm) == -1)
    {
        throw ERR(2,SHM_DET_ERR,66);
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
        throw ERR(2,SOK_CRT_ERR,78);
    }

    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, server_sockpath.c_str()); 
    socklen_t length = sizeof(server_sockaddr);
    
    unlink(server_sockpath.c_str());

    
    if(bind(server_sock, (struct sockaddr *) &server_sockaddr, length) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,90);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,91);
        throw ERR(2,SOK_BND_ERR,92);
    }

    return server_sock;
}
//[================================================================================================]
//[================================================================================================]
// Mark the server socket as open for buisness (i.e. capable of accepting connections)
// The Server can queue up X number of connection requests were X = BACKLOG
// 
// @ server_sock: This server socket's identifier
// @ server_sockpath: This server socket's pathname
//[================================================================================================]
void listen_for_client(int server_sock, std::string server_sockpath)
{
    if(listen(server_sock, BACKLOG) < 0)
    { 
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,102);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,103);
        throw ERR(2,SOK_LSTN_ERR,104);
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
int accept_client(int server_sock, struct sockaddr_un& client_sockaddr, socklen_t length, std::string server_sockpath)
{
    int client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &length);
    if(client_sock < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,111);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,112);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,113);
        throw ERR(2,SOK_ACPT_ERR,116);
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
void get_peername(int client_sock, struct sockaddr_un& client_sockaddr, int server_sock, std::string server_sockpath)
{
    socklen_t length = sizeof(client_sockaddr);
    
    if(getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &length) < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,128);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,129);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,130);
        throw ERR(2,SOK_GTPR_ERR,131);
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
char* recv_msg(int client_sock, int size, int flag, 
    int server_sock, std::string server_sockpath, std::string client_sockpath)
{
    char* msg = new char[size];
    memset(msg,'\0',size);

    int bytes_rec = recv(client_sock, msg, size, flag);
    if(bytes_rec < 0)
    {
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,145);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,146);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,147);
        if(unlink(client_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,148);
        throw ERR(2,SOK_RECV_ERR,149);
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
void send_response(int client_sock, char* data, int size, int flag, 
    int server_sock, std::string server_sockpath, std::string client_sockpath)
{
    if(send(client_sock, data, size, flag) < 0) 
    {
        /* Close Connection */
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,164);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,165);

        /* Delete socket */
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,166);
        if(unlink(client_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,167);
        throw ERR(2,SOK_SEND_ERR,168);
    }   
}
//[================================================================================================]
std::vector<std::string> build_vector(char* cmnd, int id, int size)
{
    std::vector<std::string> commands;
    for(unsigned int i = sizeof(int); i < MAX_COMMAND_SIZE; i+=size)
    {
        if(cmnd[i] == '\0') break;
        std::string command(cmnd + i, size);
        command.insert(0,(std::to_string(id) + "-"));
        int offset = (sizeof(int) + size * 2 + 1) - command.length();
        command.insert(end(command),offset,'\0');
        commands.push_back(command);
        // std::cout << "Command: " << command << std::endl;
        // std::cout << "Command Size: " << command.length() << std::endl;
    }
    // std::cout << "Commands: " << std::endl;
    // for(unsigned int i = 0; i < commands.size(); i++)
    // {
    //     std::cout << commands[i] << std::endl;
    // }
    return commands;
}
//[================================================================================================]
std::vector<std::string> decompose(char* cmnd, int id, int size)
{   

    switch(id)
    {
        case(4): return build_vector(cmnd,id,size);
        case(5): return build_vector(cmnd,id,size);
        case(6): return build_vector(cmnd,id,size);
        case(7): return build_vector(cmnd,id,size);
        case(8): return build_vector(cmnd,id,size);
        //case(9)
        //case(10)
        case(11): return build_vector(cmnd,id,size);
        case(12): return build_vector(cmnd,id,size);
        case(13): return build_vector(cmnd,id,size);
        //case(14)
        case(15): return build_vector(cmnd,id,size);
        //case(16)
        //case(17)
        case(18): return build_vector(cmnd,id,size);
        //case(19)
        //case(20)
        //case(21)
        //case(22)
        //case(23)
        default:
        {
            std::cerr << "Invalid Command ID" << std::endl;
            std::vector<std::string> empty;
            return empty;
        }

    }
}
//[================================================================================================]


#endif
