////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.h
//  Liason headerfile
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Sun. | Jan. 28th | 2018 | 8:30 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
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
char* get_shm_seg(key_t key, int& id)
{
    int shm_id;
    char*  shm;

    /* Create Shared Memory Segment for Blocking */
    if ((shm_id = shmget(key, SHMSZ, PERMISSIONS)) < 0) 
    {
        throw ERR(2,SHM_GET_ERR,47);
    }

    /* Attach the shared memory to this process so the CLI can access it */
    if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1) 
    {
        throw ERR(2,SHM_ATT_ERR,53);  
    }

    id = shm_id;

    return shm;
}
//[================================================================================================]
//[================================================================================================]
void unat_shm(int shm_id, char* shm)
{
    if(shmdt(shm) == -1)
    {
        throw ERR(2,SHM_DET_ERR,66);
    }

}
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
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,90);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,91);
        throw ERR(2,SOK_BND_ERR,92);
    }

    return server_sock;
}

void listen_for_client(int server_sock, std::string server_sockpath)
{
    if(listen(server_sock, BACKLOG) < 0)
    { 
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,102);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,103);
        throw ERR(2,SOK_LSTN_ERR,104);
    }

    return;
}

int accept_client(int server_sock, struct sockaddr_un& client_sockaddr, socklen_t length, std::string server_sockpath)
{
    int client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &length);
    if(client_sock < 0)
    {
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,111);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,112);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,113);
        throw ERR(2,SOK_ACPT_ERR,116);
    }

    return client_sock;
}

void get_peername(int client_sock, struct sockaddr_un& client_sockaddr, int server_sock, std::string server_sockpath)
{
    socklen_t length = sizeof(client_sockaddr);
    
    if(getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &length) < 0)
    {
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,128);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,129);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,130);
        throw ERR(2,SOK_GTPR_ERR,131);
    }

    return;
}

char* recv_msg(int client_sock, int size, int flag, 
    int server_sock, std::string server_sockpath, std::string client_sockpath)
{
    char* msg = new char[size];
    memset(msg,'\0',size);

    int bytes_rec = recv(client_sock, msg, size, flag);
    if(bytes_rec < 0)
    {
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,145);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,146);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,147);
        if(unlink(client_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,148);
        throw ERR(2,SOK_RECV_ERR,149);
    }

    return msg;
}


void send_response(int client_sock, char* data, int size, int flag, 
    int server_sock, std::string server_sockpath, std::string client_sockpath)
{
    if(send(client_sock, data, size, flag) < 0) 
    {
        if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,164);
        if(close(client_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,165);
        if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,166);
        if(unlink(client_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,167);
        throw ERR(2,SOK_SEND_ERR,168);
    }   
}




