///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Cli.h
//  Comand Line Interface Class Header File
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Mar. 19th | 2018 | 1:38 PM
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CLI_H
#define CLI_H

#include <string>                                         /* Strings */
#include <iostream>                                       /* cout */
#include <vector>                                         /* Vectors */
#include <errno.h>                                        /* errno Definitions */
#include <unistd.h>                                       /* Unix Std. Stuff */
#include <sys/socket.h>                                   /* Socket Handling */
#include <sys/un.h>                                       /* Unix Domain Socket Stuff */
#include <sys/ipc.h>                                      /* Inter Process Communication Stds. */
#include <sys/shm.h>                                      /* Shared Memory Handling */
#include <signal.h>                                       /* Signal Handling */
#include <sys/types.h>                                    /* System Types Definitions*/
#include <sys/wait.h>                                     /* Wait Calls */

#include "../../SharedHeaders/Arboreal_Exceptions.h"      /* Custom Exception Handling */
#include "../../SharedHeaders/Print.h"                    /* A Bunch of Print Functions */
#include "../../SharedHeaders/DebugMessages.hpp"

static const int MaxBufferSize = 4096;                    /* Maximum size a command can be */
static const int SharedMemorySize = 1;                    /* Size of Shared Memory Segment */
static const int Permissions = 0666;                      /* Socket Permissions */
static const int Flag = 0;                                /* Socket Send/Recv Flag */
DebugMessages Debug;


class CLI
{
public:

    /*!
     * @param partition A pointer to a charachter array containing the partition name
     * that this particular command line interface will operate in
     */
    CLI(char** partition);

    /*!
     * @param partition A pointer to a charachter array containing the partition name
     * that this particular command line interface will operate in
     * 
     * @param debug Wether or not debug messages should be turned on for this interface
     */
    CLI(char** partition, bool debug);

    /*!
     * @param partition A pointer to a charachter array containing the partition name
     * that this particular command line interface will operate in
     * 
     * @param isScript Flag telling whether or not the input for this interface will be coming from a file
     * (The flag value is '-s')
     */
    CLI(char** partition, char* isScript);

    /*!
     * @param partition A pointer to a charachter array containing the partition name
     * that this particular command line interface will operate in
     * 
     * @param debug Wether or not debug messages should be turned on for this interface
     * 
     * @param isScript Flag telling whether or not the input for this interface will be coming from a file
     * (The flag value is '-s')
     */
    CLI(char** partition, char* isScript, bool debug);

    /*!
     * Default Destructor
     */
    ~CLI();

    /*!
     * Performs initial set-up activities such as initiating connections and 
     * sending handshakes.  Upon the completion of a successful handshake, run()
     * is called and the interface is ready to use.  If the handshake was not successful,
     * the interface notifies the user and quits.
     */
    void start();

    /*!
     * This function operates the same as run() but takes its input from a filestream rather than a user.
     * Reads in the input data (A File System Command) and sends it down to the file system.  
     * Some commands that do not need to interact with the File System code are handled in this function.
     * For example, displaying the 'help' messages is executed from this function since the File System
     * does not have or need and 'help' command.  This function will block until it receives a response
     * from the File System (provided that the command inputted is intended to go to the File System)
     * this function will continue reading from the input file until an error occurs or 'end' is read in.
     * 
     * @param input A std::string value representing a File System command.  This value is generally handed
     * to the function by reading an input file.  But may also be sent to it from another process such as a UI
     */
    void run(std::string input);

    /*!
     * Reads in the input data (A File System Command) and sends it down to the file system.  
     * Some commands that do not need to interact with the File System code are handled in this function.
     * For example, displaying the 'help' messages is executed from this function since the File System
     * does not have or need and 'help' command.  This function will block until it receives a response
     * from the File System (provided that the command inputted is intended to go to the File System)
     * this function will continue reading from user input until an error occurs or the user quits the application.
     */
    void run();

    /*!
     * Converts a std::string to a C-Style String,
     * embeds the command id into the C-String,
     * and pads it to length = MaxBufferSize
     * 
     * @param  id    File System Command ID
     * 
     * @param  input File System Command
     * 
     * @return       A C-Style String of length = MaxBufferSize containing the
     * command ID in the first X Bytes where X is the size of an integer type
     * followed by the command itself followed by as many nullbytes as nescesarry in order
     * to have a length = MaxBufferSize
     */
    char* build(const int id, const std::string input);

    /*!
     * Sends a command converted to a C-Style String to the Liaison Process
     * for parsing and execution.
     * 
     * @param command A C-Style String of length = MaxBufferSize containing the
     * command ID in the first X Bytes where X is the size of an integer type
     * followed by the command itself followed by as many nullbytes as nescesarry in order
     * to have a length = MaxBufferSize
     */
    void send_cmnd(const char* command);

    /*!
     * Receive data from the liaison process
     * The data is X number of charachters
     * The data can be anything from a list of files returned by the 'find' operation
     * To an error message.  This function blocks until it receives data.
     */
    void await_response();

private:

    /* Wether or not the input is coming from a file */
    std::string _is_script;

    std::string _my_pid;
    std::string _client_sockpath;
    std::string _server_sockpath;

    /* Maximum file/tagname length allowed */
    int _max_string_size;
    std::string _my_partition;
    std::string _cwd = "/"; //current working directory

    int _client_sock;
    struct sockaddr_un _server_sockaddr;
    struct sockaddr_un _client_sockaddr;
};

#endif
