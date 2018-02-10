////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli_driver.cpp
//  CLI Test Driver
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Mon. | Feb. 5th | 2018 | 8:30 AM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cli.h"
std::string pipe_name = "";

int main(int argc, char** argv)
{
    std::cout << argc << std::endl;
    std::string flag;
    if(argc == 3) flag = argv[2];
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
    else if(argc == 3 && flag == "-s")
    {
        std::cout << "Reading From Text File....\n\n";
        try
        {
            CLI cli(argv,argv[2]);
        }
        catch(ERR err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            std::cerr << err.where;
            exit(1);
        }
    }
    else if(argc == 3 && flag == "-d")
    {
        try
        {
            CLI cli(argv,true);
        }
        catch(ERR err)
        {
            std::cerr << err.what;
            std::cerr << err.why;
            std::cerr << err.where;
            exit(1);
        }
    }
    else if(argc == 4)
    {
        std::cout << "Reading From Text File....\n\n";
        try
        {
            CLI cli(argv,argv[2],true);
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
    std::cout << "Command Line Interface Quit Successfully; Goodbye" << std::endl << std::endl;
    
    return 0;
}