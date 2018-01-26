////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  cli_driver.cpp
//  CLI Test Driver
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//
//  Fri. | Jan. 26th | 2018 | 11:27 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cli.h"
string pipe_name = "";

int main(int argc, char** argv)
{
    if(argc < 2 || argc >= 4)
    {
        cerr << "Ivalid Argument Count\n";
        return 0;
    }
    if(argc == 3)
    {
        CLI cli(argv,argv[2]);
        //cli.run();
    }
    else
    {
        CLI cli(argv);
        //cli.run();
    }
    return 0;
}