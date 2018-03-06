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
    std::cout << "|  5)  { 'find' '-t' '{tagName,...}' }--> Find file with all tags                   |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  6)  { 'find' '-f' '[filename(.ext),...]' }--> Find files by name                 |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  7)  { 'new' '-t' '[tagName,...]' }--> Create tags                                |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  8) { 'new' '-f' '[filename.ext,...]' }--> Create files                           |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  9) { 'new' '-f' '[filename.ext,...]' '-t' '[tagName,...] }--> Create&Tag files   |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  10) { 'new' '-f' '[filename.ext,...]' '-t' '{tagName,...}' }--> Create&Tag files |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  11) { 'delete' '-t' '[tagName,...]' }--> Delete tags                             |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  12) { 'delete' '-f' '[filename.ext,...]' }--> Delete files                       |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  13) { 'fdelete' '-t' '[tagName,...]' }--> Force delete tags                      |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  14) { 'open' '[filename.ext,...]' }--> Open files  PATH                          |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  15) { 'close' '[filename.ext,...]' }--> Close files                              |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  16) { 'rename' '-t' '[tagName,...]' '-n' '[newName,...]' }--> Rename Tags        |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  17) { 'rename' '-f' '[filename.ext,...]' '-n' '[newName,...]' }--> Rename files  |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  18) { 'get' '[filename.ext,...]' }--> Get attributes of files                    |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  19) { 'merge' 'tagName1' '->' 'tagName2' }--> Merge two existing tags            |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  20) { 'merge' '[tagName,...]' '->' 'tagName' }--> Merge multiple tags into one   |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  21) { 'tag' 'filename.ext' '->' 'tagName' }--> Tag a file with a tag             |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  22) { 'tag' 'filename.ext' '->' '[tagName,...]' }--> Add tags to file            |\n";
    std::cout << "|                                                                                   |\n";
    std::cout << "|  23) { 'tag' '[filename.ext,...]' '->' 'tagName' }--> Tag multiple files          |\n";
    std::cout << "|                                                                                   |\n";
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
// Prints the contents of a buffer (Made to order for 'Command Buffers' but can be edited if need be)
// NOTE: The first four characters (or rather whatever happens to be the size of an integer)
//       are skipped since they need to be read all together to mean anything (avoids garbage output)
//
// @ buff: The buffer to be printed
// @ size: The size of the buffer to be printed
//[================================================================================================]
void print_buffer(char* buff, int size)
{
    std::cout << "Buffer Contents: ";
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        std::cout << buff[i];
    }
    std::cout << std::endl;
    return;
}
//[================================================================================================]
/*
 * Checks the data within a buffer (Made to order for 'Command Buffers' but can be edited if need be)
 * This is a security feature (hopefully)
 * The only data allowed in a command buffer is aplhanumeric including "_" and the '\0' character
 * All other characters are considered errors and will trigger a "fix_buffer" call
 * This prevents garbage in the 'Command Buffers' and hopefully limits side channel attacks
 *
 * NOTE: As in the case of print_buffer() the first X characters are skipped
 *       Where X is the size of an integer.  This is especially important here
 *       otherwise the 'Command Id' data will be flagged as bad and will be overwritten
 *       with null characters ('\0')
 *
 * @ buff: The buffer to be printed
 * @ size: The size of the buffer to be printed
 *
 */
//[================================================================================================]
bool check_buffer(char* buff, int size)
{
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        std::string temp;
        temp += buff[i];
        if(!std::regex_match(temp,good_buffer) && buff[i] != '\0')
        {
            std::cout << "\n\nBad Buffer!\n";
            return false;
        }
    }
    return true;
}
//[================================================================================================]
/*
 * This is more of a debug function.
 * It can be used to test the "write_to_cmnd()" operations in order to make sure that they
 * are writing data in chunks of size = to "max_string_size" (even for data that is only 2 char long)
 *
 * NOTE: As in the case of print_buffer() the first X characters are skipped
 *       Where X is the size of an integer
 *
 * @ buff: The buffer to be printed
 * @ size: The size of the buffer to be printed
 *
 */
//[================================================================================================]
void check_buffer_partitioning(char* buffer,int size)
{
    bool is_null = false;
    int alpha_count = 0;
    int null_count = 0;
    int p_count = 1;
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        if(buffer[i] == '\0' && !is_null){is_null = true;}
        else if(!is_null){alpha_count += 1;}
        else if(is_null)
        {
            null_count += 1;
            if(null_count > 64)
            {
                std::cout << "Partition " << p_count << " Consists Of:\n";
                std::cout << "   " << alpha_count << " non-null characters\n";
                std::cout << "Remaining Space is Free\n\n";
                return;
            }
            if(buffer[i] != '\0')
            {
                std::cout << "Partition " << p_count << " Consists Of:\n";
                std::cout << "   " << alpha_count << " non-null characters\n";
                std::cout << "   " << null_count << " null characters\n";
                std::cout << "   With a total character count of: " << alpha_count + null_count << std::endl;
                std::cout << std::endl;
                is_null = false;
                alpha_count = 1;
                null_count = 0;
                p_count += 1;
            }
        }
    }
    return;
}
//[================================================================================================]
// Fixes a buffer flagged as bad by check_buffer()
// It does this by overwriting all offending data with the '\0' character
//
// NOTE: As in the case of print_buffer() the first X characters are ignored
//       Where X is the size of an integer
//
// @ buff: The buffer to be printed
// @ size: The size of the buffer to be printed
//
//[================================================================================================]
void fix_buffer(char* buff,int size)
{
    int  non_null = 0;
    int fixed_count = 0;
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        std::string temp;
        temp += buff[i];
        if(buff[i] != '\0')
        {
            non_null += 1;
        }
        if(!std::regex_match(temp,good_buffer) && buff[i] != '\0')
        {
            buff[i] = '\0';
            fixed_count += 1;
        }
    }
    std::cout << "\n\nBuffer Fixed!\n";
    std::cout << "+-------------------------------------------+\n";
    std::cout << "Non-Null Char Count: " << non_null << std::endl;
    std::cout << "Fixed " << fixed_count << " Character(s) In Buffer\n";
    std::cout << ".............................................\n";

    return;
}
//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer and return it
//
// @ cmnd: A Command Buffer (Buffer with the first n slots representing an integer)
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
    char* data = new char[MAX_COMMAND_SIZE];
    memset(data, '\0', MAX_COMMAND_SIZE);

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
//[================================================================================================]
// Used by CLI::build()
//
// Main logic used to write Liason redable commands to the 'Command Buffer'
// A lot of the commands are built with similar syntax (this is on purpose)
// Thus it made sense to avoid duplication and use a helper function
//
//[================================================================================================]
void write_to_cmnd(char* cmnd, std::string input, int offset, int version, int max_string_size)
{
    bool ignore = true;
    int temp_index = 0;
    char temp[max_string_size];

    // Zero out the temp buffer
    memset(temp,'\0',max_string_size);

    if(version == OPEN)
    {
        // needs testing and comments
        for(unsigned int i = 5; i < input.length(); i++)
        {
            if(input[i] == ' ') continue;
            else if(input[i] != '/')
            {
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
        temp_index = 0;
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        offset += max_string_size;
    }
    else if(version == INCLUSIVE) // Command uses the '[]' rather than '{}'
    {
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '[')
                {
                    // begin list (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == ']')
            {
                // Write last element in list to cmnd buffer
                // write to cmnd buffer and reset temp buffer, increase offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // write item to temp pipe until you hit a comma
                // (which signals that you are moving on to another item)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma, write the temp buffer to the cmnd buffer
                // reset temp buffer, update offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
        std::cout << "Command: ";
        print_buffer(cmnd, MAX_COMMAND_SIZE);
    }
//[================================================================================================]

    else if(version == EXCLUSIVE) // Uses '{}' rather than '[]'
    {
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '{')
                {
                    // begin set (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
               else{continue;}
            }
            else if(input[i] == '}')
            {
                // Write last element in set to cmnd buffer
                // write to cmnd buffer and reset temp buffer, increase offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // write item to temp pipe until you hit a comma
                // (which signals that you are moving on to another item)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma, write the temp buffer to the cmnd buffer
                // reset temp buffer, update offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == NEW_AND_TAG)
    {
        // For the 'new' commands that ALSO tag we must deal with two seperate lists/sets
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '[')
                {
                    // begin lists (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == '-')
            {
                // Write the flag to the buffer to be used as a delimiter later on
                // then reset temp and update offset
                ignore = true; // In order to ignore spaces after flag
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1];
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;

                // increment the index by one (so we don't read in the 't' of the flag)
                i += 1;
            }
            else if(input[i] == ']')
            {
                // Write out last element of list
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // Write list element to temp (to later write out to cmnd buffer)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma and need to write the element
                // you just read into temp buffer to the cmnd buffer
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == NEW_AND_TAG_EXC)
    {
        // Same as the above except the multiple tags listed make part of a set
        // rather than a list ('{}' rather than '[]')
        //

        // Boolean variables to control when to execute which particular logic
        bool bracket = true;
        bool brace = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(bracket && input[i] == '[')
                {
                    // begin list name (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else if(brace && input[i] == '{')
                {
                    // begin set (you don't want to ignore any of the data coming next)
                    ignore  = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == '-')
            {
                // Save flag (to cmnd buffer) for later use as delimiter
                //
                ignore = true; // ignore spaces after flag
                bracket = false; // done with brackets
                brace = true; // need to do braces next
                              //
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1];
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;

                // increment the index by one (so we don't read in the 't' of the flag)
                i += 1;
            }
            else if(bracket && input[i] == ']')
            {
                // Write final element in list
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(brace && input[i] == '}')
            {
                // Write final element in set
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // save current element info
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // Write current element info to cmnd buffer
                // before moving on to next element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == MERGE_1)
    {
        // for single tag into single tag:  merge(tag1,tag2)
        for(unsigned int i = 0; i < input.length(); i++)
        {
            // ignore the begin part i.e. the "merge" std::string of ther command
            if(i < 6){continue;}
            else
            {
                if(input[i] == '-')
                {
                    // Save previous entry (before the flag was hit)
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // Write flag
                    temp[temp_index] = input[i];
                    temp[temp_index+1] = input[i+1];
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // increment the index by one (so we don't read in the '>' of the flag)
                    i += 1;
                    continue;
                }
                else if(input[i] != ' ')
                {
                    // Save current entry (and later write it to cmnd buffer)
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
            }
        }

        // Write last entry to buffer
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
    }
//[================================================================================================]

    else if(version == MERGE_2)
    {
        // for merging many tags into a single tag: merge([tag1,tag2,tag3],tag4)
        bool ignore = true;
        bool single = false; // tag to merge into is not a list need to handle differently

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the comand name and first flag
            {
                if(input[i] == '[')
                {
                    // begin list (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == ',')
            {
                // Write out current list element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] == ']')
            {
                // Write out last list element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] == '-')
            {
                // Write flag for use as delimiter later
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1]; // write char after '-'
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                temp_index = 0;
                offset += max_string_size;

                // increment the index by one (so we don't read in the '>' of the flag)
                i += 1;
                single  = true;
                continue;
            }
            else if(input[i] != ',' && input[i] != ' ')
            {
                // Save current element
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else if(single)
            {
                if(input[i] == ' '){continue;} // skip space preceding the single tag
                else
                {
                    // Save the single tag
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
            }
        }

        // Write out the single tag
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
    }
//[================================================================================================]

    else if(version == TAG_1)
    {
        // tagging a single file with a single tag
        bool ignore = true;
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the comand name and first flag
            {
                if(input[i] == ' ')
                {
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else
            {
                if(input[i] != ' ')
                {
                    // Save current element
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
                else
                {
                    // Write out first element (filename)
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
            }
        }

        // Write out last element (tag)
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
        offset += max_string_size;
    }
//[================================================================================================]

    else if(version == TAG_2)
    {
        // Tag a file with multiple tags
        bool ignore = true;
        bool list = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the command name and first flag
            {
                if(input[i] == ' ')
                {
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else
            {
                if(input[i] == ' ')
                {
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
                else
                {
                    if(input[i] == '-')
                    {
                        // Save flag for use as delimiter
                        temp[temp_index] = input[i];
                        temp[temp_index+1] = input[i+1];
                        memcpy(cmnd + offset,temp,max_string_size);
                        memset(temp,'\0',max_string_size);
                        temp_index = 0;
                        offset += max_string_size;

                        // skip the rest of the flag and the space after it
                        i += 2;
                    }
                    else
                    {
                        if(input[i] == '[') // need to work with a list now
                        {
                            list = true;
                            continue;
                        }
                        else if(list)
                        {
                            if(input[i] == ',')
                            {
                                // Write out current element
                                memcpy(cmnd + offset,temp,max_string_size);
                                memset(temp,'\0',max_string_size);
                                temp_index = 0;
                                offset += max_string_size;
                            }
                            else if(input[i] == ']')
                            {
                                // Write out last element
                                memcpy(cmnd + offset,temp,max_string_size);
                                memset(temp,'\0',max_string_size);
                                temp_index = 0;
                                offset += max_string_size;
                            }
                            else
                            {
                                // Save current element
                                temp[temp_index] = input[i];
                                temp_index += 1;
                            }
                        }
                        else
                        {
                            // Save first entry (filename)
                            temp[temp_index] = input[i];
                            temp_index += 1;
                        }
                    }
                }
            }
        }
    }
//[================================================================================================]

    else if(version == TAG_3)
    {
        // tag multiple files with a single tag

        bool ignore = true;
        bool list = false;
        bool arrow = false;
        bool single = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // Ignore command name and first flag/space/etc
            {
                if(input[i] == '[')
                {
                    ignore = false;
                    list = true;
                    continue;
                }
                else{continue;}
            }
            else if(list) // Write out the list elements
            {
                if(input[i] != ',' && input[i] != ']')
                {
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
                else if(input[i] == ']')
                {
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                    list = false; // done with list
                    arrow = true; // start writing flag
                }
                else
                {
                    // Write out list element
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
            }
            else if(arrow)
            {
                if(input[i] == ' ') // skip space before flag
                {
                    continue;
                }
                else
                {
                    // Write out flag
                    temp[temp_index] = input[i];
                    temp[temp_index+1] = input[i+1];
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // Skip rest of flag + space after flag
                    i += 2;

                    arrow = false; // done with flag
                    single = true; // Write single entry (tagname)
                }
            }
            else if(single)
            {
                // Save tagname
                temp[temp_index] = input[i];
                temp_index += 1;
            }
        }

        // Write tag name
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
        offset += max_string_size;
    }
}
//[================================================================================================]

bool check_name_size(std::string input, int cmnd_id, int max_string_size)
{
    // Make sure file or tagname size does not exceed maximum
    switch(cmnd_id)
    {
        case(7): // new tag(s)
        {
            int count = 0;
            // start process right after first '[' of list
            for(unsigned int i = 9; i < input.length(); i++)
            {
                // Not a comma means were still on the same name
                if(input[i] != ',')
                {
                    count += 1;
                    if(count > max_string_size) return false;
                }
                // Comma means we finished a name and need to reset the count
                if(input[i] == ',') count = 0;
                // Last bracket means we checked the whol list and everything is ok
                if(input[i] == ']') return true;
            }
        }
        case(8): // new file(s)
        {
            int count = 0;
            // start process right after first '[' of list
            for(unsigned int i = 9; i < input.length(); i++)
            {
                // Not a comma means were still on the same name
                if(input[i] != ',')
                {
                    count += 1;
                    if(count > max_string_size) return false;
                }
                if(input[i] == ',') count = 0;
                if(input[i] == ']') return true;
            }
        }
        case(9): // new file(s) with tag(s)
        {
            int count = 0;
            bool tags = false;
            for(unsigned int i = 9; i < input.length(); i++)
            {
                // Not a comma means were still on the same name
                if(input[i] != ',')
                {
                    count += 1;
                    if(count > max_string_size) return false;
                }

                // Reset the count if finished with a filename or finished with the whole list
                if(input[i] == ',' || (input[i] == ']' && !tags))
                {
                    count = 0;
                    continue;
                }

                // About to start tag list (reset counter)
                if(input[i] == '[')
                {
                    count = 0;
                    tags = true;
                    continue;
                }

                // Finished tag list everything checks out
                if(input[i] == ']' && tags)
                {
                    return true;
                }
            }
        }
        case(10): // new file(s) with exclusive tag(s)
        {
            // Same stuff going on as case 7/8/9
            // Don't nee boolean variable here because the tags are surrounded by '{}'
            // (In case 9 they were surrounded by '[]' like the filenames
            //  this made differentiation difficult without a boolean)
            int count = 0;
            for(unsigned int i = 9; i < input.length(); i++)
            {
                if(input[i] != ',')
                {
                    count += 1;
                    if(count > max_string_size) return false;
                }
                if(input[i] == ',' || input[i] == ']') count = 0;
                if(input[i] == '{') count = 0;
                if(input[i] == '}') return true;
            }
        }
        case(16): // rename a tag(s)
        {
            int count = 0;
            bool ready = false;

            // Only need to check new tagnames (last list)
            // Since we skip the first list's '[' (we start at i = 12)
            //  we can use the last list's '[' as our delimiter to know when we start counting
            for(unsigned int i = 12; i < input.length(); i++)
            {
                if(input[i] == '[')
                {
                    ready = true;
                    continue;
                }
                if(ready)
                {
                    if(input[i] != ',')
                    {
                        count += 1;
                        if(count > max_string_size) return false;
                    }
                    if(input[i] == ',') count = 0;
                    if(input[i] == ']') return true;
                }
            }
        }
        case(17): // rename a file(s)
        {
            // Same as case 16
            int count = 0;
            bool ready = false;
            for(unsigned int i = 12; i < input.length(); i++)
            {
                if(input[i] == '[')
                {
                    ready = true;
                    continue;
                }
                if(ready)
                {
                    if(input[i] != ',')
                    {
                        count += 1;
                        if(count > max_string_size) return false;
                    }
                    if(input[i] == ',') count = 0;
                    if(input[i] == ']') return true;
                }
            }
        }
        case(20): // merge tag(s) into 1 (maybe new) tag
        {
            // These are nice cause we can just skip to the arrow ('->')
            //  and subtract remaining input length from our current index
            for(unsigned int i = 0; i < input.length(); i++)
            {
                if(input[i] == '>')
                {
                    // subtract 1 in order to skip space right after arrow: i.e. "...-> newtagname"
                    if(((input.length() - i)-1) > max_string_size) return false;
                    else return true;
                }
            }
        }
        case(21): // tag (maybe new) a file
        {
            // Same as 20
            for(unsigned int i = 0; i < input.length(); i++)
            {
                if(input[i] == '>')
                {
                    // subtract 1 in order to skip space right after arrow: i.e. "...-> newtagname"
                    if(((input.length() - i)-1) > max_string_size) return false;
                    else return true;
                }
            }
        }
        case(22): // add (maybe new) tag(s) to a file
        {
            int count = 0;
            bool ready = false;
            for(unsigned int i = 0; i < input.length(); i++)
            {
                if(input[i] == '[')
                {
                    ready = true;
                    continue;
                }
                if(ready)
                {
                    if(input[i] != ',')
                    {
                        count += 1;
                        if(count > max_string_size) return false;
                    }
                    if(input[i] == ',') count = 0;
                    // End of list all is well
                    if(input[i] == ']') return true;
                }
            }
        }
        case(23): // tag file(s) all with the same (maybe new) tag
        {
            // same as 20
            for(unsigned int i = 0; i < input.length(); i++)
            {
                if(input[i] == '>')
                {
                    // subtract 1 in order to skip space right after arrow: i.e. "...-> newtagname"
                    if(((input.length() - i)-1) > max_string_size) return false;
                    else return true;
                }
            }
        }
        default:
        {
            // If there is no worry about potential new things
            // then we just return true, after all if the tag name is too long for
            // a command like 'find' the command will simply return not found!
            // Since all tags/files already in system will have name length <= max
            return true;
        }
    }
}






#endif
