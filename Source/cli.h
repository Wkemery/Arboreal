////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli.h
//  Comand Line Interface Header File
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Fri. | Jan. 26th | 2018 | 11:27 PM
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


#define MAX_HISTORY_SIZE 10
#define MAX_COMAND_SIZE 2048
using namespace::std;



class CLI
{
public:
    CLI(char** partition);
    CLI(char** partition, char* isScript);
    ~CLI();
    void start();
    void run(string input);
    void run();
    char* build(int id, string input);
    int send(char* command);
    int receive();
private:
    string is_script;
    int max_string_size;
    string my_partition;
    string my_pipe_name;
    char* latest_command;
};

#endif




