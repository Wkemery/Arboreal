///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli.h
//  Comand Line Interface Header File
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Filesystem/types.h"


#ifndef CLI_H
#define CLI_H


#include <string>                           /* Strings */
#include <iostream>                         /* cout */
#include <vector>                           /* Vectors */
#include <errno.h>                          /* errno Definitions */
#include <unistd.h>                         /* Unix Std. Stuff */
#include <sys/socket.h>                     /* Socket Handling */
#include <sys/un.h>                         /* Unix Domain Socket Stuff */
#include <sys/ipc.h>                        /* Inter Process Communication Stds. */
#include <sys/shm.h>                        /* Shared Memory Handling */
#include "../Filesystem/Arboreal_Exceptions.h"            /* Exception Handling */


#define MAX_HISTORY_SIZE 10                 /* Maximum number of previously entered commands recorded */
#define MAX_COMMAND_SIZE 2048               /* Maximum size a command can be */
#define SHMSZ 1                             /* Size of Shared Memory Segment */
#define PERMISSIONS 0666                    /* Socket Permissions */
#define FLAG 0                              /* Socket Send/Recv Flag */


class CLI
{
public:
    CLI(char** partition);
    CLI(char** partition, bool debug);
    CLI(char** partition, char* isScript);
    CLI(char** partition, char* isScript, bool debug);
    ~CLI();

    /* Perform Initial Set-Up */
    void start();

    /* Run the CLI using a txt file as input */
    void run(std::string input);

    /* Run the CLI using raw user input */
    void run();

    /* Build Liaison readable commands from user readable commands */
    char* build(int id, std::string input);

    /* Send Command to Liaison */
    void send_cmnd(char* command);

    /* Receive data from liaison */
    void await_response();

private:

    /* This is a string because it is an argument passed to main ( -s ) */
    std::string is_script;
    std::string my_pid;
    std::string client_sockpath;
    std::string server_sockpath;

    int max_string_size;
    std::string my_partition;

    int client_sock;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    bool dbug = false;
};

#endif
