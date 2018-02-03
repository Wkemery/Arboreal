////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli_driver.cpp
//  CLI Test Driver
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Sun. | Jan. 28th | 2018 | 8:30 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cli.h"
std::string pipe_name = "";

bool server_up = false;

int main(int argc, char** argv)
{
    if(argc == 2)
    {
        try
        {
            CLI cli(argv);
        }
        catch(ERR err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            std::cerr << err.where;
            exit(1);
        }
    }
    else
    {
        std::cerr << "Invalid Arg Count - Must define a partition on which to initialize the command line interface\n";
    }
    std::cout << "C: Command Line Interface Quit Successfully; Goodbye" << std::endl << std::endl;
    return 0;
}