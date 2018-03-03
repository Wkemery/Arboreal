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

#include <stdlib.h>                 /* System Stuff */
#include <string>                   /* Strings */
#include <iostream>                 /* cout */
#include <vector>                   /* Vectors */
#include <errno.h>                  /* errno Definitions */
#include <stdio.h>                  /* C-Std Input/Output */
#include <unistd.h>                 /* Unix Std. Stuff */
#include <sys/socket.h>             /* Socket Handling */
#include <sys/un.h>                 /* Unix Domain Socket Stuff */
#include <sys/ipc.h>                /* Inter Process Communication Stds. */
#include <sys/shm.h>                /* Shared Memory Handling */
#include <chrono>                   /* System Time */
#include <ctime>                    /* Time Utilities */
#include <netinet/in.h>
#include <netdb.h> 
#include "ErrorClass.h"             /* Exception Handling */



#define PERMISSIONS 0666                 /* Socket Permissions */
#define MAX_COMMAND_SIZE 2048            /* Maximum Size a FS Command Buffer Can Be */
#define SHMSZ 1                          /* The Size of a Shared Memory Segment */
#define BACKLOG 10                       /* Number of Connection Requests that the Server Can Queue */
#define FLAG 0                           /* Flag for Send/Recv. Operations */
#define DMON_SOCK_PATH "fs_daemon.sock"  /* Daemon Process Socket Path */
#define DMON_PORT 70777
#define TIMEOUT 10
#include "liason_helper.hpp"             /* Helper Functions */

void clean(int signal)
{
    std::cout << "SIGNAL: " << signal << std::endl;
    system("rm *socket");
    exit(0);
}

void bad_clean(int signal)
{
    system("rm *socket");
    exit(1);
}

void seg_fault(int signal)
{
    std::cout << "SEGFAULT: " << signal << std::endl;
    system("rm *socket");
    exit(1);
}

int main(int argc, char** argv)
{
    signal(SIGABRT,bad_clean);
    signal(SIGTERM,clean);
    signal(SIGINT,clean);
    signal(SIGQUIT,clean);
    signal(SIGSEGV,seg_fault);

    int max_string_size = 64; // Will be set via handshake
    
    /* Turn on debug printing */
    bool dbug = false;
    if(argc == 5) dbug = true;

    if(dbug) std::cout << "L: Beginning Liaison Process..." << std::endl;
    std::string client_sockpath = argv[0];
    std::string liaison_sockpath = argv[1];
    std::string hostname = argv[3];
    if(dbug) std::cout << "L: Filesystem Hostname: " << hostname << std::endl;
    if(dbug) std::cout << "L: Client Socket Path: " << client_sockpath << std::endl;
    if(dbug) std::cout << "L: Liaison Socket Path: " << liaison_sockpath << std::endl;
    if(dbug) std::cout << "L: Daemon Socket Path: " << DMON_SOCK_PATH << std::endl;
    if(dbug) std::cout << "L: Daemon Port#: " << DMON_PORT << std::endl;


    // Connect To Daemon
    int liaison_fid, valread, timer, printer;
    struct sockaddr_in liaison_addr;
    struct sockaddr_in daemon_addr;

    timer = 0;
    printer = 0;
    while((liaison_fid = socket(AF_INET,SOCK_STREAM,0)) < 0 && timer < TIMEOUT)
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
    if(timer >= TIMEOUT)
    {
        printf("\nL: Liaison Socket Could Not Be Created; Exiting\n");
        exit(1);
    }
    else{ timer = 0; }

    memset(&daemon_addr, '\0', sizeof(daemon_addr));
    daemon_addr.sin_family = AF_INET;
    daemon_addr.sin_port = htons(DMON_PORT);

    while((connect(liaison_fid,(struct sockaddr*)&daemon_addr,sizeof(daemon_addr))) < 0)
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
    if(timer >= TIMEOUT)
    {
        printf("\nL: Connection To File System Daemon Could Not Be Established; Exiting\n");
        close(liaison_fid);
        exit(1);
    }
  

    key_t shm_key = atoi(argv[2]);
    int shm_id;
    if(dbug) std::cout << "L: Shared Memory Key: " << shm_key << std::endl;
    if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl;

    if(dbug) std::cout << "L: Accessing Shared Memory For Interprocess Synchronization..." << std::endl;
    char* shm = get_shm_seg(shm_key,shm_id);
    if(dbug) std::cout << "L: Shared Memory Found; Attachment Successfull" << std::endl;

    if(dbug) std::cout << "L: Initializing Server and Client Socket Addresses..." << std::endl;
    struct sockaddr_un liaison_sockaddr;
    struct sockaddr_un client_sockaddr;

    /* Zero the structure buffers */
    memset(&liaison_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    if(dbug) std::cout << "L: Server and Client Socket Address Initilization Successfull" << std::endl;
    if(dbug) std::cout << "L: Setting Up Server Socket..." << std::endl;
    int liaison_sock = set_up_socket(liaison_sockpath,liaison_sockaddr);
    if(dbug) std::cout << "L: Server Socket Set Up Successfull" << std::endl;
    if(dbug) std::cout << "L: Signaling Client" << std::endl;

    /* Signal CLI that it is ok to continue */
    shm[0] = 1;

    if(dbug) std::cout << "L: Listening On Server Socket..." << std::endl;
    listen_for_client(liaison_sock,liaison_sockpath);
    if(dbug) std::cout << "L: Accepting Client Connections..." << std::endl;
    if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl << std::endl;

    socklen_t length = sizeof(liaison_sockaddr);
    int client_sock;

    /* Wait until CLI is ready, if this is not done you will get a
     * "Connection Refused Error" every once in a while */
    while(shm[0] == 1)
    {
        client_sock = accept_client(liaison_sock,client_sockaddr,length,liaison_sockpath);
    }
    if(dbug) std::cout << "L: Client Connection Accepted" << std::endl;


    if(dbug) std::cout << "L: Unattatching Shared Memory Segment..." << std::endl;
    unat_shm(shm_id,shm);
    if(dbug) std::cout << "L: Shared Memory Succesfully Unattatched" << std::endl;

    if(dbug) std::cout << "L: Retrieving Client Peername..." << std::endl;
    get_peername(client_sock,client_sockaddr,liaison_sock,liaison_sockpath);
    if(dbug) std::cout << "L: Client Peername Retrieved Successfully: " << client_sockaddr.sun_path << std::endl;


    /* Begin communication */
    do
    {
    
        if(dbug) std::cout << "L: Awaiting Command From Client..." << std::endl;
        char* msg = recv_msg(client_sock,MAX_COMMAND_SIZE,FLAG,liaison_sock,liaison_sockpath,client_sockpath);
        if(dbug) std::cout << "L: Client Command Received Successfully" << std::endl;
        if(dbug) std::cout << "L: Command Received From: " << client_sock << " @ " << client_sockpath << std::endl;
        if(dbug) std::cout << "L: Command: ";
        if(dbug) print_cmnd(msg,MAX_COMMAND_SIZE);
        int command_id = get_cmnd_id(msg);
        if(command_id == 999) break;
    
    
        //if(dbug) std::cout << "L: Sending Command to File System..." << std::endl;
        //
        //  TO DO:
        //          Rebuild Commands For FS
        //          Send To Daemon
        //          Await Response
        //          Send Response To CLI
        std::vector<std::string> cmnds;
        if(command_id != 0) cmnds = decompose(msg,command_id,max_string_size);
        for(unsigned int i = 0; i < cmnds.size(); i++)
        {
            std::cout << cmnds[i].c_str() << std::endl;
            send(liaison_fid,cmnds[i].c_str(),cmnds[i].length(),0);
        }
        
        if(dbug) std::cout << "L: Building Response..." << std::endl;
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        std::string data = "Command Received @ ";
        data += std::ctime(&end_time);
        memset(msg,'\0', MAX_COMMAND_SIZE);
        memcpy(msg,data.c_str(),data.length()); 
        if(dbug) std::cout << "L: Response Built Successfully" << std::endl;

        if(dbug) std::cout << "L: Sending Response to Client..." << std::endl;
        send_response(client_sock,msg,MAX_COMMAND_SIZE,FLAG,liaison_sock,liaison_sockpath,client_sockpath);
        if(dbug) std::cout << "L: Response Successfully Sent" << std::endl;
        if(dbug) std::cout << "L: Response Sent To: " << client_sock << " @ " << client_sockpath << std::endl;

    }while(true);

    std::string close_connection = "close";
    int offset = (sizeof(int) + max_string_size * 2 + 1) - close_connection.length();
    close_connection.insert(end(close_connection),offset,'\0');
    send(liaison_fid,close_connection.c_str(),close_connection.length(),0);


    if(dbug) std::cout << "L: Closing Connections..." << std::endl;
    if(close(liaison_fid) < 0) throw ERR(2,SOK_CLOSE_ERR);
    if(close(liaison_sock) < 0) throw ERR(2,SOK_CLOSE_ERR);
    if(dbug) std::cout << "L: Server Socket Successfully Closed" << std::endl;
    if(unlink(liaison_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR);
    if(dbug) std::cout << "L: Server Socket Successfully Removed" << std::endl;
    if(dbug) std::cout << "L: Liaison Process Closing; Goodbye" << std::endl;
    return 0;
}




