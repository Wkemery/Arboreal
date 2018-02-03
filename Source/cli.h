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
#include "ErrorClass.h"


#define MAX_HISTORY_SIZE 10
#define MAX_COMAND_SIZE 2048
#define SHMSZ 1
#define PERMISSIONS 0666
#define FLAG 0
#define DEBUG false


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
    void await_response();
private:
    std::string is_script;
    std::string my_pid;
    std::string client_sockpath;
    std::string server_sockpath;
    int max_string_size;
    char* my_partition;

    int client_sock;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    bool dbug = true;
};

#endif




