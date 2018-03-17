////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  helper_functions.h
//  Helper Functions for CLI
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Filesystem/types.h"

#ifndef HELPER_FUNC
#define HELPER_FUNC


#define INCLUSIVE 0
#define EXCLUSIVE 1
#define NEW_AND_TAG 2
#define NEW_AND_TAG_EXC 3
#define MERGE_1 4
#define MERGE_2 5
#define TAG_1 6
#define TAG_2 7
#define TAG_3 8
#define OPEN 9




//[================================================================================================]
/*
 *  Print a list of legal commands
 *
 *  Eventually I would like to change this to a more 'man pages' style output
 *  And make the 'help' command display only "Commonly Used" commands
 *  A lot of these commands have some quirks which are important to let the user know about
 *  But printing them all in a single print is not a very good idea (ergo the man pages)
 *
 */
//[================================================================================================]
void print_help()
{
    std::cout << "+-----------------------------------------------------------------------------------+\n";
    std::cout << "|                                    Available Commands                             |\n";
    std::cout << "+-----------------------------------------------------------------------------------+\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  1)  { 'help' | 'h' }--> Display this printout                                    |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  2)  { 'quit' | 'q' }--> Quit this interface                                      |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  3)  { 'history' }--> Display the last 10 commands inputted                       |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  4)  { 'find' '-t' '[tagName,...]' }--> Find file(s) by tag(s)                    |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  5)  { 'find' '-f' '[filename(.ext),...]' }--> Find files by name                 |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  6)  { 'new' '-t' '[tagName,...]' }--> Create tags                                |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  7) { 'new' '-f' '[filename.ext...]' }--> Create file                             |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  8) { 'new' '-f' 'filename.ext' '-t' '[tagName,...] }--> Create&Tag file          |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  9) { 'delete' '-t' '[tagName,...]' }--> Delete tags                              |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  10) { 'delete' '-f' '[filename.ext,...]' }--> Delete files                       |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  11) { 'delete' '../tag/filename(.ext)'                                           |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  12) { 'open' (-r|-w|-x) '../tag/filename(.ext)' }--> Open files  PATH            |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  13) { 'close' '../tag/filename(.ext)' }--> Close files                           |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  14) { 'rename' '-t' '[tagName,...]' '=>' '[newName,...]' }--> Rename Tags        |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  15) { 'rename' '../tag/filename(.ext)' '=>' 'newName(.ext)' }--> Rename files    |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  16) { 'attr' '../tag/filename(.ext)' }--> Get attributes of files                |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  17) { 'merge' 'tagName1' '=>' 'tagName2' }--> Merge two existing tags            |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  18) { 'merge' '[tagName,...]' '=>' 'tagName' }--> Merge multiple tags into one   |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  19) { 'tag' '../tag/filename(.ext)' '+>' '[tagName,...]' }--> Add tags to file   |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  20) { 'tag' '[filename(.ext),...]' +> [tagName,...]' }--> Tag multiple files     |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  21) { 'untag' '../tag/filename(.ext) -> [tagName,...]'                           |\n";
    std::cout << "|                                                                                   |\n"; 
    std::cout << "|  22) { 'untag' '[filename(.ext) -> [tagName,...]'                                 |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  23) { 'cd' '../tag1/tag2'                                                        |\n";
    std::cout << "+-----------------------------------------------------------------------------------+\n";
    std::cout << "Arboreal >> ";
    return;
}
//[================================================================================================]
// Print a Welcome Header
//
// You may ask,  "Is this actually necessarry?"
// I respond, "Yes, Yes it is."
//[================================================================================================]
void print_header()
{
    std::cout << "\n[]==============================================================================[]\n";
    std::cout << "||                              Welcome To Arboreal                             ||\n";
    std::cout << "||------------------------------------------------------------------------------||\n";
    std::cout << "||              Enter 'help' or 'h' to see a list of available commands         ||\n";
    std::cout << "[]==============================================================================[]\n";
    std::cout << "\n\n";
    std::cout << "Arboreal >> ";
    return;
}

void print_command(char* cmnd, int size)
{
    int index = sizeof(int);
    while(index < size)
    {
        std::cout << cmnd[index];
        index += 1;
    }
    std::cout << std::endl;
    return;
}
//[================================================================================================]
// Delete a Shared Memory Fragment
//
// @ shm_id: The Shared Memory Fragment's identifier
// @ shm:    The pointer to the Shared Memory
//[================================================================================================]
void delete_shm(int shm_id, char* shm)
{
    if(shmdt(shm) == -1)
    {
        std::string where = "[cli_helper.hpp::delete_shm()]: ";
        std::string what = "Shared Memory Detach Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    if(shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        std::string where = "[cli_helper.hpp::delete_shm()]: ";
        std::string what = "Shared Memory Delete Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }
    return;
}
//[================================================================================================]
// Create and attach a Shared Memory Segment
//
// @ key: The Key required to access the Shared Memory Segment
// @ id:  Address of an integer variable that will store the created segments identification number
//[================================================================================================]
char* create_shm_seg(key_t key, int& id)
{
    int shm_id;
    char*  shm;

    /* Create Shared Memory Segment for Blocking */
    if ((shm_id = shmget(key, SHMSZ, IPC_CREAT | PERMISSIONS)) < 0)
    {
        std::string where = "[cli_helper.hpp::create_shm_seg()]: ";
        std::string what = "Shared Memory Get Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    /* Attach the shared memory to this process so the CLI can access it */
    if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1)
    {
       std::string where = "[cli_helper.hpp::create_shm_seg()]: ";
        std::string what = "Shared Memory Attach Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    /* Save the Shared Memory Segment Id */
    id = shm_id;

    /* Return a pointer to the shared memory segment */
    return shm;
}
//[================================================================================================]
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
// Create and set-up a socket used for communication with Liaison process
// Returns the client socket's identification number
//
// @ client_sockpath: Client Socket's pathname
// @ client_sockaddr: A reference to a standard structure whose components I will not describe here
//                    and can be viewed in a Unix manual.  Suffice it to say, it stores the socket
//                    type and the socket path.  (Note that the "type" of the struct is sockaddr_un
//                    signifing that this is a unix domain socket)
//[================================================================================================]
int set_up_socket(std::string client_sockpath, struct sockaddr_un& client_sockaddr)
{
    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(client_sock == -1)
    {
        std::string where = "[cli_helper.hpp::set_up_socket()]: ";
        std::string what = "Client Socket Create Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, client_sockpath.c_str());
    socklen_t len = sizeof(client_sockaddr);

    unlink(client_sockpath.c_str());

    if(bind(client_sock, (struct sockaddr *) &client_sockaddr, len) < 0)
    {
        if(close(client_sock) < 0)
        {
            std::string where = "[cli_helper.hpp::set_up_socket()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[cli_helper.hpp::set_up_socket()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        std::string where = "[cli_helper.hpp::set_up_socket()]: ";
        std::string what = "Client Socket Bind Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    return client_sock;
}
//[================================================================================================]
// Attempt a connection to the Liaison process
//
// @ client_sock: Client socket identifiaction number
// @ client_sockpath: Client socket pathname
// @ server_sockpath: Server socket pathname
// @ server_sockaddr: A reference to a standard structure whose components I will not describe here
//                    and can be viewed in a Unix manual.  Suffice it to say, it stores the socket
//                    type and the socket path.  (Note that the "type" of the struct is sockaddr_un
//                    signifing that this is a unix domain socket)
// @ len:             Size of server_sockaddr in bytes (from sizeof() )
//[================================================================================================]
void connect_to_server(int client_sock, std::string client_sockpath,
    std::string server_sockpath, struct sockaddr_un& server_sockaddr, socklen_t len)
{
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, server_sockpath.c_str());

    if(connect(client_sock, (struct sockaddr *) &server_sockaddr, len) < 0)
    {
        if(close(client_sock) < 0)
        {
            std::string where = "[cli_helper.hpp::connect_to_server()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[cli_helper.hpp::connect_to_server()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        std::string where = "[cli_helper.hpp::connect_to_server()]: ";
        std::string what = "Connect To Server Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }
    return;
}
//[================================================================================================]
// Send a command to the Liaison process
//
// @ client_sock:     Client socket identification number
// @ client_sockpath: Client socket pathname
// @ cmnd:            Command to be sent
// @ size:            Size of 'cmnd'
// @ flag:            Flag for 'send()' call (see 'man send')
//[================================================================================================]
void send_to_server(int client_sock, std::string client_sockpath, char* cmnd, int size, int flag)
{
    if(send(client_sock, cmnd, size, flag) < 0)
    {
        if(close(client_sock) < 0)
        {
            std::string where = "[cli_helper.hpp::send_to_server()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[cli_helper.hpp::send_to_server()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        std::string where = "[cli_helper.hpp::send_to_server()]: ";
        std::string what = "Send To Server Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }

    return;
}
//[================================================================================================]
// Receive data from server, returns a pointer to the data
//
// @ client_sock:     Client socket identification number
// @ client_sockpath: Client socket pathname
// @ size:            Size of command to be recieved
// @ flag:            Flag for 'recv()' call (see 'man recv')
//[================================================================================================]
char* receive_from_server(int client_sock, std::string client_sockpath, int size, int flag)
{
    char* data = new char[size];
    memset(data, '\0', size);

    if(recv(client_sock, data, size, flag) < 0)
    {
        if(close(client_sock) < 0)
        {
            std::string where = "[cli_helper.hpp::receive_from_server()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[cli_helper.hpp::receive_from_server()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        std::string where = "[cli_helper.hpp::receive_from_server()]: ";
        std::string what = "Receive From Server Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }
    return data;
}

std::string receive_from_server(int client_sock, std::string client_sockpath)
{
    char data[MAX_COMMAND_SIZE];
    memset(data, '\0', MAX_COMMAND_SIZE);

    if(recv(client_sock, data, MAX_COMMAND_SIZE, 0) < 0)
    {
        if(close(client_sock) < 0)
        {
            std::string where = "[cli_helper.hpp::receive_from_server()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        if(unlink(client_sockpath.c_str()) < 0)
        {
            std::string where = "[cli_helper.hpp::receive_from_server()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
        }
        std::string where = "[cli_helper.hpp::receive_from_server()]: ";
        std::string what = "Receive From Server Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
    }
    std::string rval = data;
    return rval;
}
//[================================================================================================]
#endif
