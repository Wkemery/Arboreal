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



using namespace::std;

int main(int argc, char** argv)
{
    // Get the CLI pipe that this Liason is tied to
    string pipe_name;
    pipe_name = *argv;

    if(pipe_name != "")
    {
        string line;
        ifstream pipe_in;
        pipe_in.open(pipe_name);

        if(pipe_in.is_open())
        {
            getline(pipe_in,line);
            std::cout << "Liason: Received --> \"" << line << "\"\n";
            if(line == "handshake")
            {
                // query max string length
            } 
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
            pipe_out << "Message " << "\"" << line << "\"" << " Recevied by Liason " << getpid() << endl;
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