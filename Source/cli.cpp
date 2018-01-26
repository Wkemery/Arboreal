//////////////////////////////////////////////////////////////////////////////////////////////////// 
//
// cli.cpp
// Comand Line Interface Class Definitions
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
// 
// Sun. | Jan. 7th | 2018 | 8:11 PM
// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "cli.h"
#include "cmnd_validation.h"
#include "helper_functions.hpp"



// Constructor 1 (Basic Command Line Interaction)
//[================================================================================================]
CLI::CLI(char** partition)
{
   my_partition = partition[1];
   start();
}
//[================================================================================================]







// Constructor 2 (Used For Reading From A Text File)
//[================================================================================================]
CLI::CLI(char** partition, char* isScript)
{
   my_partition = partition[1];
   is_script = isScript;
   start();
}
//[================================================================================================]







// Default Destructor
//[================================================================================================]
CLI::~CLI(){}
//[================================================================================================]







// Run Initial Setup (Store Info, Perform Handshakes, etc.)
//[================================================================================================]
void CLI::start()
{

   // Build pipe name for this CLI 
   string pipe_name = ("pipe-"+std::to_string(getpid())+".pipe");
   my_pipe_name = pipe_name;

   // Convert pipe_name to a format which we can send
   // to the Liason Process as an argv for Liason's main() process
   // Type will be char* const*
   char* temp_name = const_cast<char*>(my_pipe_name.c_str());
   vector<char*> arguments;
   arguments.push_back(temp_name);
   arguments.push_back(NULL);


   // Initiate Handshake
   //[---------------------------------------------------------------------------------------------]
   
   // Open pipe to write out
   ofstream my_pipe_out;
   my_pipe_out.open(my_pipe_name);

   if(!my_pipe_out.is_open())
   {
      my_pipe_out.close();
      cerr << "CLI: Pipe Open (write) FAIL\n";
      exit(1);
   }


   // Convert the handshake command to a char array of size = MAX_COMAND_SIZE
   // In order to emulate the format the build() returns
   char command[MAX_COMAND_SIZE];
   string temp = "handshake\n";

   // Zero out buffer
   for(unsigned int i = 0; i < MAX_COMAND_SIZE; i++)
   {
      command[i] = '\0';
   }

   // Write "handshake" to command buffer
   for(unsigned int i = 0; i < temp.length(); i++)
   {
      command[i] = temp[i];
   }

   // Write out command buffer
   my_pipe_out.write(command,MAX_COMAND_SIZE);
   my_pipe_out.close();
   //[---------------------------------------------------------------------------------------------]



   // Initiate a Liason Process
   pid_t pID = vfork();
   if(pID == 0)
   {
      // Replace child with Liason process
      execv("liason",arguments.data());
   }
   else if(pID < 0)
   {
      cerr << "Fork Failed!\n";
      exit(0);
   }
   else
   {
      // Wait for child to finish
      while(wait(&pID) > 0){}
      cout << "Returned to Parent\n";

      ifstream my_pipe_in;
      my_pipe_in.open(my_pipe_name);

      string line;

      if(!my_pipe_in.is_open())
      {
         my_pipe_in.close();
         cerr << "CLI: Pipe Open (read) FAIL\n";
         exit(1);
      }

      getline(my_pipe_in,line);
      cout << "CLI: Received --> \"" << line << "\"\n";

      my_pipe_in.close();
   
      cout << "\n\nInitiating Command Line Interface Using Partition: " << my_partition << endl;
      cout << ".................................................................................\n\n";
   
      max_string_size = 64;

      run();

      return;
   }

}
//[================================================================================================]










// Begin The Command Reading Loop
//[================================================================================================]
void CLI::run()
{

   /*
    * 1. Get input   (DONE)
    * 2. Check if input is correct (DONE)
    * 3. Convert user legible command to Liason legible command (DONE)
    * 3. Send Command to Liason (TO DO)
    */

   // user input
   string input;

   // last 10 inputs
   vector<string> history;
                           
   bool from_history = false; // unimportant at the moment
   
   // Print the welcome header
   print_header();


   // Operations will differ slightly if reading commands from a text file
   if(is_script == "-s")
   {
      cout << "Reading from input file...\n";
      cout << "+---------------------------------------\n\n\n";
   }


   // Begin read loop
   while(true)
   {
      // Check for ENTER pressed (prints out a new line with 'Arboreal >> ')
      char c = cin.get();
      if(c == '\n')
      {
         if(is_script != "-s"){cout << "Arboreal >> \n";}
         continue;
      }
      else
      {
         /* 
          * Put back the character used to check for ENTER pressed
          * If this is not done will result in the first character 
          * of the input being thrown away
          *
          * The conditional below is currently meaningless
          * The idea was to provide a history that one could scroll through
          * similar to how the regular command line operates
          * however this is NOT implemented yet 
          * Note that the code IN the conditional IS IMPORTANT
          * (I've just set things up for later edits)
          */
         if(!from_history) 
         {
            cin.putback(c);

            // Get the command
            getline(cin,input);
   
            // Add command to history
            history.push_back(input);
            if(is_script == "-s"){cout << "Arboreal >> " << input << endl;}
         }
         from_history = false;

         //----------------------------------------------------------------------------------------+


         if(input == "quit" || input == "q")
         {
            // May need to do more than return in order to make
            // sure we dont corrupt data
            cout << "Are you sure you would like to quit? (Y/N)\n";
            cin >> input;
            if(input == "Y" || input == "y")
            {
               // delete pipe file
               if(remove(my_pipe_name.c_str()) != 0)
               {
                  cerr << "CLI: Error Removing " << my_pipe_name << endl;
               }
               return;
            }
            else{continue;}
         }


         //----------------------------------------------------------------------------------------+
         

         else if(input == "h" || input == "help")
         {
            print_help();
         }


         //----------------------------------------------------------------------------------------+
         

         else if(input == "history")
         {
            for(unsigned int i = 0; i < history.size(); i++)
            {
               cout << i << ": " << history[i] << "\n";
            }
            if(history.size() >= MAX_HISTORY_SIZE)
            {
               history.clear();
            }
            if(is_script == "-s"){cout << "\n";}
            if(is_script != "-s"){cout << "Arboreal >> ";}
         }

         // If using a text file to read in commands
         if(input == "end" && is_script == "-s")
         {
            cout << "\n\n";
            return;
         }


         //----------------------------------------------------------------------------------------+
         

         else
         {
            // Test if the command is valid
            int rtrn = check_command(input);
            if(rtrn != 0)
            {
               // Debug
               cout << "Return: " << rtrn << endl;
               cout << "Buffer: \n";
               send(build(rtrn,input));
               // If pipe empty continue else output
               if(is_script != "-s"){cout << "Arboreal >> ";}
            }
            else
            {
               cerr << "Comand Not Valid\n";
               if(is_script != "-s"){cout << "Arboreal >> ";}
            }

         }
         //----------------------------------------------------------------------------------------+
      }
   }
   return;
}
//[================================================================================================]








// Send the newly built command to the Liason process
//[================================================================================================]
int CLI::send(char* command)
{

   // FIX NULL CMND
   // 
   ofstream my_pipe_out;
   char* temp_name = const_cast<char*>(my_pipe_name.c_str());
   vector<char*> arguments;
   arguments.push_back(temp_name);
   arguments.push_back(NULL);

   cout << "YES: " << command << endl;
   my_pipe_out.open(my_pipe_name);
   if(my_pipe_out.is_open())
   {
      my_pipe_out.write(command,MAX_COMAND_SIZE);
      my_pipe_out.close();
   }
   else
   {
      cerr << "CLI: Pipe Open (write) FAIL\n";
      return -1;
   }


   pid_t pID = vfork();

   if(pID == 0)
   {
      execv("liason",arguments.data());
   }
   else if(pID < 0)
   {
      cerr << "CLID: Fork Creation Failed\n";
      return -1;
   }
   else
   {
      while(wait(&pID) > 0);

      ifstream my_pipe_in;
      my_pipe_in.open(my_pipe_name);

      if(my_pipe_in.is_open())
      {
         string line;
         while(getline(my_pipe_in,line))
         {
            cout << "CLI: Received --> " << "\"" << line << "\"\n"; 
         }
      }
      my_pipe_in.close();
   }
   return 0;
}
//[================================================================================================]










// Build the Liason Readable Command
/* The majority of the logic here is offloaded to a helper function in 'helper_functions.hpp'
 * This was done in order to avoid code duplication
 * There is still some code duplication in that each section starts the same and ends the same
 * However some commands require different build versions so this duplication is 
 * somewhat unavoidable.  Perhaps later on it may be worth looking into reducing it further
 * However I think that all of this code is very readable and pretty simple so it's not pressing
 */
//[================================================================================================]
char* CLI::build(int id,string input)
{
   char* cmnd = new char[MAX_COMAND_SIZE];
   int offset = 0;

   // Zero out the command buffer
   memset(cmnd,'\0',MAX_COMAND_SIZE);


   switch(id)
   {
      case(4): // find -t [tagName,...] }--> Find file(s) by tag(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);

         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;

      }
//[================================================================================================]
      
      case(5): // find -t {tagName,...} }--> Find file with all tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,EXCLUSIVE,max_string_size);

         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]
      
      case(6): // find -f [filename.ext,...] }--> Find files by name
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]
      
      case(7): // new -t [tagName,...] }--> Create tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);

         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]
      
      case(8): // new -f [filename.ext,...] }--> Create files
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);

         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(9): // new -f [filename.ext,...] -t [tagName,...] }--> Create & Tag Files 1:1
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,NEW_AND_TAG,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(10): // new -f [filename.ext,...] -t {tagName,...} }--> Create & Tag Files w/ ALL tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,NEW_AND_TAG_EXC,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(11): // delete -t [tagName,...] }--> Delete tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(12): // delete -f [filename.ext,...] }--> Delete files  
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(13): // fdelete -t [tagName,...] }--> Force delete tags 
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case (14): // open [filename.ext,...] }--> Open file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(15): // close [filename.ext,...] }--> Close file(s)  
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(16): // rename -t [tagName,...] -n [newName,...] }--> Rename Tag(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,NEW_AND_TAG,max_string_size);

         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(17): // rename -f [filename.ext,...] -n [newName.ext,...] }--> Rename file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,NEW_AND_TAG,max_string_size);

         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(18): // get [filename.ext,...] }--> Get attributes of file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(19): // merge tagName1 -> tagName2 }--> Merge two existing tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,MERGE_1,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(20): // merge [tagName,...] -> tagName }--> Merge multiple tags into one
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,MERGE_2,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(21): // tag filename.ext -> tagName }--> Tag a file with a tag
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,TAG_1,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(22): // tag filename.ext -> [tagName,...] }--> Add tags to file
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,TAG_2,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
//[================================================================================================]

      case(23): // tag [filename.ext,...] -> tagName }--> Tag multiple files
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(cmnd,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(cmnd,input,offset,TAG_3,max_string_size);
         
         // Some Debug Printing
         cout << "Value: " << get_cmnd_id(cmnd) << endl;
         print_buffer(cmnd,MAX_COMAND_SIZE);
         check_buffer_partitioning(cmnd,MAX_COMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(cmnd,MAX_COMAND_SIZE))
         {
            fix_buffer(cmnd,MAX_COMAND_SIZE);

            // Debug Print
            print_buffer(cmnd,MAX_COMAND_SIZE);
         }
         return cmnd;
      }
   }

   // For completeness' sake
   return cmnd;
}
//[================================================================================================]


















