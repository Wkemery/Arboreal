///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Print.h
// Functions that print to stdout
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
//
//  Tue. | Mar. 20th | 2018 | 10:50 PM | Stable | Documented 
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "CommandValidation.h"
#ifndef PRINT
#define PRINT

//[================================================================================================]
/*!
 * Print a table of all of the available command archetypes
 */
//[================================================================================================]
void print_cmnd_lst()
{
  std::cout << "[==========================================================]" << std::endl;
  std::cout << "[                List Of Available Commands                ]" << std::endl;
  std::cout << "[==========================================================]" << std::endl;
  std::cout << "[ help                       ]" << "[ attr                       ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ quit                       ]" << "[ merge                      ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ find                       ]" << "[ tag                        ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ new                        ]" << "[ untag                      ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ delete                     ]" << "[ cd                         ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ open                       ]" << "[ read                       ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ close                      ]" << "[ write                      ]" << std::endl;
  std::cout << "[----------------------------]" << "[----------------------------]" << std::endl;
  std::cout << "[ rename                     ]" << "[ copy                       ]" << std::endl;
  std::cout << "[=============================" << "=============================]" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'help' command
 */
//[================================================================================================]
void print_help()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ help | h ]: (Runs The Arboreal Helper)" << std::endl;
  std::cout << ">>X   [ -h --command name ]: (Prints Usage For A Single Command)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]





//[================================================================================================]
/*!
 * Print usage for 'quit' command
 */
//[================================================================================================]
void print_quit()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ quit | q | Q ]: (Quits The Command Line Interface)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]





//[================================================================================================]
/*!
 * Print usage for 'find' command
 */
//[================================================================================================]
void print_find()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ find -t [tagname,...] ]: (Finds Files By Tag)" << std::endl;
  std::cout << ">>    [ find -t [filename(.ext),...] ]: (Finds File By Name)" << std::endl;
  std::cout << std::endl;
  return; 
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'new' command
 */
//[================================================================================================]
void print_new()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ new -t [tagname,...] ]: (Creates a New Tag(s))" << std::endl;
  std::cout << ">>    [ new -f [filename(.ext),...] ]: (Creates A New File(s) Within The Current Directory)" << std::endl;
  std::cout << ">>    [ new ../tagname/filename(.ext) ]: (Creates A New File With The Chosen Path)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'delete' command
 */
//[================================================================================================]
void print_del()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ delete -t [tagname,...] ]: (Deletes An Empty Tag(s))" << std::endl;
  std::cout << ">>    [ delete -f [filename(.ext)...] ]: (Deletes A File(s) From The Current Directory)" << std::endl;
  std::cout << ">>X   [ delete ../tagname/filename(.ext) ]: (Deletes A File With The Given Path)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'open' command
 */
//[================================================================================================]
void print_open()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ open ../tagname/filename(.ext) ]: (Opens A File With The Specified Path)" << std::endl;
  std::cout << ">>    [ open filename(.ext) ]: (Opens A File Within The Current Directory With The Specified Name)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'close' command
 */
//[================================================================================================]
void print_close()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ close ../tagname/filename(.ext) ]: (Closes A File With The Specified Path)" << std::endl;
  std::cout << ">>    [ close filename(.ext) ]: (Closes A File Within The Current Directory With The Specified Name)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'rename' command
 */
//[================================================================================================]
void print_rname()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ rename -t [tagname,...] => [newname,...] ]: (Renames A Tag(s))" << std::endl;
  std::cout << ">>X   [ rename -f [filename(.ext),...] => [newname(.ext),...] ]: (Renames A File Within The Current Directory)" << std::endl;
  std::cout << ">>    [ rename ../tagname/filename(.ext) => newname(.ext) ]: (Renames A File With The Given Path)" << std::endl;
  std::cout << std::endl;
  return; 
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'attr' command
 */
//[================================================================================================]
void print_attr()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ attr ../tagname/filename(.ext) ]: (Gets The Attributes For The File With The Given Path)" << std::endl;
  std::cout << ">>X   [ attr [filename(.ext),...] ]: (Gets The Attributes For The File(s), Within The Current Directory, With The Given Name(s))" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'merge' command
 */
//[================================================================================================]
void print_merge()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>X   [ merge tagname1 => tagname2 ]: (Merges Tag1 Into Tag2)" << std::endl;
  std::cout << ">>X   [ merge [tagname,...] => tagname ]: (Merges Many Tags Into A Single Tag)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'tag' command
 */
//[================================================================================================]
void print_tag()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ tag ../tagname/filename(.ext) +> [tagname,...] ]: (Tags The File With The Given Path, With ALL The Given Tags)" << std::endl;
  std::cout << ">>    [ tag [filename(.ext),...] +> [tagname,...] ]: (Tags ALL The File(s), Within The Current Directory And With The Given Name(s), With ALL The Given Tag(s))" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'untag' command
 */
//[================================================================================================]
void print_utag()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ untag ../tagname/filename(.ext) -> [tagname,...] ]: (Untags ALL The Given Tags From The File With The Given Path)" << std::endl;
  std::cout << ">>    [ untag [filename(.ext),...] -> [tagname,...] ]: (Untags ALL The Given Tag(s) From The File(s) Within The Current Directory And With The Given Name(s))" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'cd' command
 */
//[================================================================================================]
void print_cd()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>    [ cd ../tagname/tagname ]: (Changes The Current Directory To The Specified Directory)" << std::endl;
  std::cout << ">>    [ cd ./tagname/tagname ]: (Prepends The Current Directory And Then Changes The Current Directory To The Specified Directory (The '.' Is Required))" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'read' command
 */
//[================================================================================================]
void print_read()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>X   [ read ../tagname/filename(.ext) -b X ]: (Reads X Bytes From The File With The Specified Path)" << std::endl;
  std::cout << ">>X   [ read filename(.ext) -b X ]: (Reads X Bytes From The File Within The Current Directory With The Given Name)" << std::endl;
  std::cout << ">>X   [ read ../tagname/filename(.ext) ]: (Reads ALL Of The Contents Of The File With The Specified Path)" << std::endl;
  std::cout << ">>X   [ read filename(.ext) ]: (Reads ALL Of The Contents Of The File Within The Current Directory With The Given Name)" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'write' command
 */
//[================================================================================================]
void print_write()
{
  std::cout << "[Usage]" << std::endl;
  std::cout << ">>X   [ write ../tagname/filename(.ext) ]: (Writes Data To The File With The Given Path)" << std::endl;
  std::cout << ">>X   [ write -a ../tagname/filename(.ext) ]: (Appends Data To The File With The Given Path)" << std::endl;
  std::cout << ">>X   [ write filename(.ext) ]: (Writes Data To The File Within The Current Directory With The Given Name)" << std::endl;
  std::cout << ">>X   [ write -a filename(.ext) ]: (Appends Data To The File Within The Current Directory With The Given Name)" << std::endl;
  std::cout << ">>X   [ write -b X ../tagname/filename1(.ext) -> ../tagname/filename2(.ext) ]: (Overwrites File Two With The First X Bytes Of File One)" << std::endl;
  std::cout << ">>X   [ write -a -b X ../tagname/filename1(.ext) -> ../tagname/filename2(.ext) ]: (Appends File Two With The First X Bytes Of File One)" << std::endl;
  std::cout << ">>X   [ write -b X filename(.ext) -> filename2(.ext) ]: (Overwrites File Two With The First X Bytes Of File One (Both Files Must Be In The Current Directory))" << std::endl;
  std::cout << ">>X   [ write -a -b X filename(.ext) -> filename2(.ext) ]: (Appends File Two With The First X Bytes Of File One (Both Files Must Be In The Current Directory))" << std::endl;
  std::cout << std::endl;
  return; 
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print usage for 'copy' command
 */
//[================================================================================================]
void print_copy()
{
  std::cout << "[Usage]" <<std::endl;
  std::cout << ">>X   [ copy ../tagname/filename1(.ext) -> ../tagname/filename2(.ext) ]: (Overwrites The Contents Of File Two with Those Of File One)" << std::endl;
  std::cout << ">>X   [ copy filname1(.ext) -> filename2(.ext) ]: (Overwrites The Contents Of File Two with Those Of File One (Both Files Must Be In The Currrent Directory))" << std::endl;
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Run helper applet
 */
//[================================================================================================]
void help()
{
  print_cmnd_lst();
  std::cout << "Please Enter '--' Followed By A Command Name To See Usage (Q/q to Quit): ";
  std::string input;
  std::cin >> input;
  while(input != "Q" && input != "q")
  {
    if(input == "Q" || input == "q") break;
    else
    {
      switch(check_usage(input))
      {
        case(UHELP):{print_help();break;}
        case(UQUIT):{print_quit();break;}
        case(UFIND):{print_find();break;}
        case(UNEW):{print_new();break;}
        case(UDEL):{print_del();break;}
        case(UOPEN):{print_open();break;}
        case(UCLOSE):{print_close();break;}
        case(URNAME):{print_rname();break;}
        case(UATTR):{print_attr();break;}
        case(UMERG):{print_merge();break;}
        case(UTAG):{print_tag();break;}
        case(UUTAG):{print_utag();break;}
        case(UCD):{print_cd();break;}
        case(UREAD):{print_read();break;}
        case(UWRITE):{print_write();break;}
        case(UCOPY):{print_copy();break;}
        default:
        {
          std::cout << "Please Enter '--' Followed By A Command Name (e.g. --help) To See Usage (Q/q to Quit): ";
        }
      }
    }
    input = "";
    std::cout << "Please Enter '--' Followed By A Command Name To See Usage (Q/q to Quit): ";
    std::cin >> input;
  }
  std::cout << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]






//[================================================================================================]
/*!
 * Print a welcome header
 */
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
//[================================================================================================]






//[================================================================================================]
/*! \brief Print a command buffer
 *
 * Because the command ID is saved as literral bytes (as opposed to the string representation)
 * and std::cout does not print those well, the first X bytes of the C-String are skipped
 * where X is the size of an integer
 *
 * @param cmnd: The command buffer as a C-String
 * @param size: The size of the command buffer (should always be whatever MaxBufferSize is
 *              although this is not strictly speaking mandatory)
 */
//[================================================================================================]
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
//[================================================================================================]






//[================================================================================================]
/*!
 * Prints the contents of any vector as long as the contents of the vector can be piped to stdout
 *
 * @param vec: A referance to the vector to be printed
 */
//[================================================================================================]
template <typename T>
void print_vector(const std::vector<T>& vec)
{
  std::cout << "Vector: " << std::endl;
  for(unsigned int i = 0; i < vec.size(); i++)
  {
    std::cout << vec[i] << std::endl;
  }
}
//[================================================================================================]
//                                          END Print.h
//[================================================================================================]

#endif