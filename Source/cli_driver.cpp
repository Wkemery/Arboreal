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
    if(argc < 2 || argc >= 5)
    {
        std::cerr << "Ivalid Argument Count\n";
        return 0;
    }
    if(argc == 4)
    {
        try
        {
            CLI cli(argv,argv[2],true);
        }
        catch(CLI_EX err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            exit(1);
        }
    }
    else if(argc == 3 && !strncmp(argv[2],"-d",sizeof("-d")))
    {
        try
        {
            CLI cli(argv,argv[2]);
        }
        catch(CLI_EX err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            exit(1);
        }
    }
    else if(argc == 3 && strncmp(argv[2],"-d",sizeof("-d")))
    {
        try
        {
            CLI cli(argv,true);
        }
        catch(CLI_EX err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            exit(1);
        }
    }
    else
    {
        try
        {
            CLI cli(argv);
        }
        catch(CLI_EX err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            exit(1);
        }
    }
    return 0;
}