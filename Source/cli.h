////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli.h
//  Comand Line Interface Header File
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Sun. | Jan. 28th | 2018 | 8:30 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CLI_H
#define CLI_H


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define MAX_HISTORY_SIZE 10
#define MAX_COMAND_SIZE 2048
#define SHMSZ 1


struct CLI_EX
{
    std::string where;
    std::string what;
    std::string why;
};


class CLI
{
public:
    CLI(char** partition);
    CLI(char** partition, char* isScript);
    CLI(char** partition, bool debug);
    CLI(char** partition, char* isScript, bool debug);
    ~CLI();
    void start();
    void run(std::string input);
    void run();
    char* build(int id, std::string input);
    void send_cmnd(char* command);
    int receive();
private:
    std::string is_script;
    std::string my_pid;
    std::string client_socket_path;
    std::string server_socket_path;
    int max_string_size;
    int shm_id;
    char* my_partition;
    char* shared_mem;

    int client_sock, rc, len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    bool dbug = false;
};

#endif




