////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.cpp
//  Liason process for communication between GUI,CLI,Filesystem
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Fri. | Jan. 26th | 2018 | 11:27 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define MAX_COMMAND_SIZE 2048


using namespace::std;

int main(int argc, char** argv)
{
    // Get the CLI pipe that this Liason is tied to
    string pipe_name;
    pipe_name = *argv;
    char command[MAX_COMMAND_SIZE];

    if(pipe_name != "")
    {
        string line;
        ifstream pipe_in;
        pipe_in.open(pipe_name);

        if(pipe_in.is_open())
        {
            
            pipe_in.read(command,MAX_COMMAND_SIZE); 
            pipe_in.close();      
        }
        else
        {
            pipe_in.close();
            std::cerr << "Liason: Pipe Open [" << pipe_name << "] (read) FAIL\n";
        }

//                                      Read From Pipe        
//-------------------------------------------------------------------------------------------------
//                                      Write To Pipe
        ofstream pipe_out;
        pipe_out.open(pipe_name);
        if(pipe_out.is_open())
        {
            pipe_out.write("Liason: Command Received\n",sizeof("Liason: Command Received\n"));
            pipe_out.close();
        }
        else
        {
            pipe_out.close();
            std::cerr << "Liason: Pipe Open [" << pipe_name << "] (write) FAIL\n";
        }

    }
    return 0;
}


vector<char*> generate_system_calls(char* command)
{
    vector<char*> command_list;
    char command_id[4];
    for(unsigned int i = 0; i < 4; i++)
    {
        command_id[i] = command[i];
    }
    int c_id = atoi(command_id);
    cout << "C-ID: " << c_id << endl;

    return command_list;
}
