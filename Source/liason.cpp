////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.cpp
//  Liason process for communication between GUI,CLI,Filesystem
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Sun. | Jan. 28th | 2018 | 8:30 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>
#include <ctime>
#include "ErrorClass.h"



#define PERMISSIONS 0666
#define MAX_COMMAND_SIZE 2048
#define SHMSZ 1
#define BACKLOG 10
#define FLAG 0
#include "liason_helper.hpp"

int main(int argc, char** argv)
{
    //int max_string_size = 64;
    
    bool dbug = true;

    if(dbug) std::cout << "L: Beginning Liaison Process..." << std::endl;
    std::string client_sockpath = argv[0];
    std::string server_sockpath = argv[1];
    if(dbug) std::cout << "L: Client Socket Path: " << client_sockpath << std::endl;
    if(dbug) std::cout << "L: Server Socket Path: " << server_sockpath << std::endl;

    key_t shm_key = atoi(argv[2]);
    int shm_id;
    if(dbug) std::cout << "L: Shared Memory Key: " << shm_key << std::endl;
    if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl;

    if(dbug) std::cout << "L: Accessing Shared Memory For Interprocess Synchronization..." << std::endl;
    char* shm = get_shm_seg(shm_key,shm_id);
    if(dbug) std::cout << "L: Shared Memory Found; Attachment Successfull" << std::endl;

    if(dbug) std::cout << "L: Initializing Server and Client Socket Addresses..." << std::endl;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;  

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    if(dbug) std::cout << "L: Server and Client Socket Address Initilization Successfull" << std::endl;
    if(dbug) std::cout << "L: Setting Up Server Socket..." << std::endl;
    int server_sock = set_up_socket(server_sockpath,server_sockaddr);
    if(dbug) std::cout << "L: Server Socket Set Up Successfull" << std::endl;
    if(dbug) std::cout << "L: Signaling Client" << std::endl;

    shm[0] = 1;

    if(dbug) std::cout << "L: Listening On Server Socket..." << std::endl;
    listen_for_client(server_sock,server_sockpath);
    if(dbug) std::cout << "L: Accepting Client Connections..." << std::endl;
    if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl << std::endl;

    socklen_t length = sizeof(server_sockaddr);
    int client_sock;
    while(shm[0] == 1)
    {
        client_sock = accept_client(server_sock,client_sockaddr,length,server_sockpath);
    }
    if(dbug) std::cout << "L: Client Connection Accepted" << std::endl;


    if(dbug) std::cout << "L: Unattatching Shared Memory Segment..." << std::endl;
    unat_shm(shm_id,shm);
    if(dbug) std::cout << "L: Shared Memory Succesfully Unattatched" << std::endl;

    if(dbug) std::cout << "L: Retrieving Client Peername..." << std::endl;
    get_peername(client_sock,client_sockaddr,server_sock,server_sockpath);
    if(dbug) std::cout << "L: Client Peername Retrieved Successfully: " << client_sockaddr.sun_path << std::endl;


    do
    {
    
        if(dbug) std::cout << "L: Awaiting Command From Client..." << std::endl;
        char* msg = recv_msg(client_sock,MAX_COMMAND_SIZE,FLAG,server_sock,server_sockpath,client_sockpath);
        if(dbug) std::cout << "L: Client Command Received Successfully" << std::endl;
        if(dbug) std::cout << "L: Command Received From: " << client_sock << " @ " << client_sockpath << std::endl;
        if(dbug) std::cout << "L: Command: ";
        if(dbug) print_cmnd(msg,MAX_COMMAND_SIZE);
        if(get_cmnd_id(msg) == 999) break;
    
    
        //if(dbug) std::cout << "L: Sending Command to File System..." << std::endl;
        //
        //  TO DO:
        //          Rebuild Commands For FS
        //          Send To Daemon
        //          Await Response
        //          Send Response To CLI
        
    
        if(dbug) std::cout << "L: Building Response..." << std::endl;
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        std::string data = "Command Received @ ";
        data += std::ctime(&end_time);
        memset(msg,'\0', MAX_COMMAND_SIZE);
        memcpy(msg,data.c_str(),data.length()); 
        if(dbug) std::cout << "L: Response Built Successfully" << std::endl;

        if(dbug) std::cout << "L: Sending Response to Client..." << std::endl;
        send_response(client_sock,msg,MAX_COMMAND_SIZE,FLAG,server_sock,server_sockpath,client_sockpath);
        if(dbug) std::cout << "L: Response Successfully Sent" << std::endl;
        if(dbug) std::cout << "L: Response Sent To: " << client_sock << " @ " << client_sockpath << std::endl;

    }while(true);




    if(dbug) std::cout << "L: Closing Connections..." << std::endl;
    if(close(server_sock) < 0) throw ERR(2,SOK_CLOSE_ERR,112);
    if(dbug) std::cout << "L: Server Socket Successfully Closed" << std::endl;
    if(unlink(server_sockpath.c_str()) < 0) throw ERR(2,SOK_UNLNK_ERR,114);
    if(dbug) std::cout << "L: Server Socket Successfully Removed" << std::endl;
    if(dbug) std::cout << "L: Liaison Process Closing; Goodbye" << std::endl;
    return 0;
}















































