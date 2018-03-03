// ConnectionDaemon.cpp

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
#include <netdb.h> 
#include <sys/ioctl.h>
#include "filesystem.h"
                 
#define BACKLOG 10                  /* Number of Connection Requests that the Server Can Queue */
#define FLAG 0                      /* Flag for recv() */
#define TIMEOUT 10
#define TRUE 1                      
#define FALSE 0
#define PORT 70777

int create_sock(int timeout);
void set_socket_opt(int daemon_sock, int sock_opt, int timeout);
void bind_socket(int daemon_sock, struct sockaddr_in daemon_sockaddr, int timeout);
void listen_on_socket(int daemon_sock,int backlog,int timeout);
void set_nonblocking(int daemon_sock, int is_on);
void quit_fs(void);
void sig_caught(int sig);

struct error
{
    error(std::string where, std::string what)
    {
        __what = what;
        __where = where;
    }
    void print(void)
    {
        std::cerr << __where << __what << std::endl;
    }
    std::string __what;
    std::string __where;
};

fd_set master_set;
int my_fid = 999;
int max_fid = 0;

int main(int argc, char** argv)
{

    signal(SIGABRT,sig_caught);
    signal(SIGTERM,sig_caught);
    signal(SIGINT,sig_caught);
    signal(SIGQUIT,sig_caught);
    signal(SIGSEGV,sig_caught);

    int max_string_size = 64; // Set by a call to FS

    try
    {

        int max_command_size = (sizeof(int) + max_string_size * 2 + 1);
        std::vector<int> active_connections;
        
    
        //---------------- BEGIN SOCKET SETUP ---------------------------------------------------------
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
        memset(buffer,'\0',max_command_size);
        memset(&daemon_sockaddr, 0, sizeof(daemon_sockaddr));
    
        daemon_sock = create_sock(TIMEOUT);
        my_fid = daemon_sock;
    
        set_socket_opt(daemon_sock,sock_opt,TIMEOUT);
    
        daemon_sockaddr.sin_family = AF_INET;
        daemon_sockaddr.sin_addr.s_addr = INADDR_ANY;
        daemon_sockaddr.sin_port = htons(PORT);
    
        set_nonblocking(daemon_sock,on);
    
        bind_socket(daemon_sock,daemon_sockaddr,TIMEOUT);
        listen_on_socket(daemon_sock,BACKLOG,TIMEOUT);
    
        FD_ZERO(&master_set);
        max_fid = daemon_sock;
        FD_SET(daemon_sock,&master_set);
    
        // Listens for "Q"/"q"/"quit" and quits daemon (for testing only)
        std::thread quit(quit_fs);
        quit.detach();
    
        do
        {
            /* Copy master file descriptor set to working set */
            memcpy(&working_set, &master_set, sizeof(master_set));
    
            /* Attempt a select() call to get a file descriptor */
            rval = select(max_fid + 1, &working_set, NULL, NULL, NULL);
            if(rval < 0)
            {
                std::string where = "[ConnectionDaemon.cpp::main()]: ";
                std::string what = "select() Failed -- ";
                what += strerror(errno);
                throw error(where,what);
                break;
            }
            else if(rval == 0)
            {
                std::string where = "[ConnectionDaemon.cpp::main()]: ";
                std::string what = "select() Timed Out -- ";
                what += strerror(errno);
                throw error(where,what);
                break;
            }
    
            desc_ready = rval;
            for(int i = 0; i <= max_fid && desc_ready > 0; i++)
            {
                if(FD_ISSET(i,&working_set))
                {
                    desc_ready -= 1;
                    if(i == daemon_sock)
                    {
                        printf("D: Listening Socket Is Readable...\n");
                        do
                        {
                            client_sock = accept(daemon_sock,NULL,NULL);
                            if(client_sock < 0)
                            {
                                if(errno != EWOULDBLOCK)
                                {
                                    printf("D: accept() Failed\n");
                                    END_SERVER = TRUE;
                                    std::string where = "[ConnectionDaemon.cpp::main()]: ";
                                    std::string what = "accept() Failed -- ";
                                    what += strerror(errno);
                                    throw error(where,what);
                                }
                                break;
                            }
                            printf("D: New Incoming Connection - %d\n",client_sock);
                            FD_SET(client_sock,&master_set);
                            if(client_sock > max_fid) max_fid = client_sock;
    
                        }while(client_sock != -1);
                    }
                    else
                    {
                        printf("D: Descriptor %d is readable\n",i);
                        close_conn = FALSE;
    
                        do
                        {
                            rval = recv(i, buffer, max_command_size, FLAG);
                            if(rval < 0)
                            {
                                if(errno != EWOULDBLOCK)
                                {
                                    printf("D: recv() Failed; Closing Connection...\n");
                                    close_conn = TRUE;
                                    std::string where = "[ConnectionDaemon.cpp::main()]: ";
                                    std::string what = "recv() Failed -- ";
                                    what += strerror(errno);
                                    throw error(where,what);
                                }
                                break;
                            }
    
                            if(rval == 0)
                            {
                                printf("D: Client Closed Connection\n");
                                close_conn = TRUE;
                                break;
                            }
    
                            int bytes_received = rval;
                            printf("\nBytes Received: %d\n",bytes_received);
                            printf("Command Received: %s\n",buffer);
                            fflush(stdout); 
    
                        }while(TRUE);
    
                        if(close_conn)
                        {
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
    
        }while(END_SERVER == FALSE);


    } // END try{}
    catch(error& e)
    {
        e.print();
        printf("D: Closing All Open Connections and Exiting...\n");
        for (int i=0; i <= max_fid; ++i)
        {
           if (FD_ISSET(i, &master_set)) close(i);
        }
        printf("Goodbye\n");
        return 1;
    }



    printf("D: Closing All Open Connections and Exiting...\n");
    for (int i=0; i <= max_fid; ++i)
    {
       if (FD_ISSET(i, &master_set)) close(i);
    }
    printf("Goodbye\n");
    return 0;
}



void sig_caught(int sig)
{
    printf("\nD: Received - %s\n", strsignal(sig));
    for (int i=0; i <= max_fid; ++i)
    {
       if (FD_ISSET(i, &master_set)) close(i);
    }
    exit(-1);
}


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
        //throw arboreal_daemon_error(where,what);
        throw error(where,what);
    }
    return daemon_sock;
}


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
        //throw arboreal_daemon_error(where,what);
        throw error(where,what);
    }
    return;
}

void set_nonblocking(int daemon_sock, int is_on)
{
    int rval = ioctl(daemon_sock,FIONBIO,(char*)&is_on);
    if(rval < 0)
    {
        close(daemon_sock);
        std::string where = "[ConnectionDaemon.cpp::set_nonblocking()]: ";
        std::string what = "Set Non-Blocking Failed -- ";
        what += strerror(errno);
        // throw arboreal_daemon_error(where,what);
        throw error(where,what);
    }
}


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
        //throw arboreal_daemon_error(where,what);
        throw error(where,what);
    }
    return;
}

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
        //throw arboreal_daemon_error(where,what);
        throw error(where,what);
    }
    return;
}
