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
#include <signal.h>
#include "types.h"
void clean(int signal)
{
    std::cout << "SIGNAL: " << signal << std::endl;
    system("rm *socket");
    exit(0);
}

void bad_clean(int signal)
{
    system("rm *socket");
    exit(1);
}

int main(int argc, char** argv)
{
    signal(SIGABRT,bad_clean);
    signal(SIGTERM,clean);
    signal(SIGINT,clean);
    signal(SIGQUIT,clean);

    std::string flag;
    if(argc == 3) flag = argv[2];
    if(argc == 2)
    {
        try
        {
            CLI cli(argv);
        }
        catch(arboreal_cli_error err)
        {
            std::cerr << err.where() <<std::endl;
            std::cerr << err.what() << std::endl;;
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
        catch(arboreal_cli_error err)
        {
            std::cerr << err.where() <<std::endl;
            std::cerr << err.what() << std::endl;;
            exit(1);
        }
    }
    else if(argc == 3 && flag == "-d")
    {
        std::cout << "Debugging Has Been Turned ON\n\n";
        try
        {
            CLI cli(argv,true);
        }
        catch(arboreal_cli_error err)
        {
            std::cerr << err.where() <<std::endl;
            std::cerr << err.what() << std::endl;;
            exit(1);
        }
    }
    else if(argc == 4)
    {
        std::cout << "Reading From Text File....\n";
        std::cout << "Debugging Has Been Turned ON\n\n";
        try
        {
            CLI cli(argv,argv[2],true);
        }
        catch(arboreal_cli_error err)
        {
            std::cerr << err.where() <<std::endl;
            std::cerr << err.what() << std::endl;;
            exit(1);
        }
    }
    else
    {
        std::cerr << "Invalid Arg Count - Must Define A Partition On Which To Initialize The Command Line Interface\n";
    }
    std::cout << "Command Line Interface Quit Successfully; Goodbye" << std::endl << std::endl;

    return 0;
}
