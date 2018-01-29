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
#include "liason_helper.hpp"


#define PERMISSIONS 0666
#define MAX_COMMAND_SIZE 2048
#define SHMSZ 1

struct LSON_EX
{
    std::string what;
    std::string why;
    std::string where;
};


int main(int argc, char** argv)
{
    int max_string_size = 64;
    std::string server_path = argv[0];
    std::string client_path = argv[1];
    std::string shared_mem_id = argv[2];

    std::cout << "LSON: " << server_path << std::endl;
    std::cout << "LSON: " << client_path << std::endl;
    std::cout << "LSON: " << shared_mem_id << std::endl;


/* Set up Shared Mem for flags */
    int shm_id;
    key_t shm_key = atoi(shared_mem_id.c_str());
    char *shm, *s;

    if((shm_id = shmget(shm_key, SHMSZ, IPC_CREAT | PERMISSIONS)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    s = shm;
    s[0] = 0;


    /* Set up socket */

    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;     
    char buf[MAX_COMMAND_SIZE];
    int backlog = 10;
    memset(&server_sockaddr, '\0', sizeof(struct sockaddr_un));
    memset(&client_sockaddr, '\0', sizeof(struct sockaddr_un));
    memset(buf, '\0', MAX_COMMAND_SIZE);  

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1){
        printf("LIASON: SOCKET ERROR: %d\n", errno);
        exit(1);
    }

    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, server_path.c_str()); 
    len = sizeof(server_sockaddr);
    
    unlink(server_path.c_str());

    rc = bind(server_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1){
        printf("LIASON: BIND ERROR: %d\n", errno);
        close(server_sock);
        exit(1);
    }

    s[0] = 1;

    while(true)
    {
        std::cout << "Shared Mem[2]: " << (int)s[2] << std::endl;
        rc = listen(server_sock, backlog);
        if (rc == -1){ 
            printf("LIASON: LISTEN ERROR: %d\n", errno);
            close(server_sock);
            exit(1);
        }
        printf("Liason: socket listening...\n\n\n");
    
        client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, (socklen_t*)&len);
        if (client_sock == -1){
            printf("LIASON: ACCEPT ERROR: %d\n", errno);
            close(server_sock);
            close(client_sock);
            exit(1);
        }
    
        len = sizeof(client_sockaddr);
        rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, (socklen_t*)&len);
        if (rc == -1){
            printf("LIASON: GETPEERNAME ERROR: %d\n", errno);
            close(server_sock);
            close(client_sock);
            exit(1);
        }
        else {
            printf("Liason: Client socket filepath: %s\n", client_sockaddr.sun_path);
        }
    
        printf("liason: waiting to read...\n");
        bytes_rec = recv(client_sock, buf, sizeof(buf), 0);
        if (bytes_rec == -1){
            printf("LIASON: RECV ERROR: %d\n", errno);
            close(server_sock);
            close(client_sock);
            exit(1);
        }
        else
        {
            std::cout << "Liason: Data Received:\n";

            int cmnd_val = get_cmnd_id(buf);
            std::cout << "Command ID: " << cmnd_val << std::endl;
            std::cout << "Command Data: ";
            char temp[max_string_size];
            int temp_index = 0;
            for(unsigned int i = sizeof(int); i < MAX_COMMAND_SIZE; i++)
            {
                if(i%64 != 0)
                {
                    temp[temp_index] = buf[i];
                    temp_index += 1;
                }
                else
                {
                    std::string data = temp;
                    if(data == "quit")
                    {
                        std::cout << "QUIT\n";
                        std::cout << "Liason: Closing Connections\n";
                        close(server_sock);
                        close(client_sock);
                        unlink(client_path.c_str());
                        unlink(server_path.c_str());
                        close(shm_id);
                        clean_segments();
                        return 0;
                    }
                    std::cout << data << std::endl;
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                }
            }
            std::cout << "\n\n\n";

        }
    
        memset(buf, '\0', MAX_COMMAND_SIZE);
        strcpy(buf, "Command Received\n");      
        printf("Liason: Sending data...\n");
        rc = send(client_sock, buf, strlen(buf), 0);
        if (rc == -1) {
            printf("LIASON: SEND ERROR: %d", errno);
            close(server_sock);
            close(client_sock);
            exit(1);
        }   
        else {
            printf("Liason: Data sent!\n");
        }

    }

    std::cout << "Liason: Closing Connections\n";
    close(server_sock);
    close(client_sock);
    unlink(client_path.c_str());
    unlink(server_path.c_str());

    return 0;
}















































