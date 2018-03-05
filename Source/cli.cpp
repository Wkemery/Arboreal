//////////////////////////////////////////////////////////////////////////////////////////////////// 
//
// cli.cpp
// Comand Line Interface Class Definitions
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
// 
//  Mon. | Feb. 5th | 2018 | 8:30 AM
// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "cli.h"
#include "cmnd_validation.h"
#include "cli_helper.hpp"



//[================================================================================================]
// Constructor 1 (Basic Command Line Interaction)
//[================================================================================================]
CLI::CLI(char** partition)
{
   my_partition = partition[1];
   my_pid = std::to_string(getpid());
   client_sockpath = "cli-" + my_pid + "-socket";
   server_sockpath = "lson-" + my_pid + "-socket";
   start();
}
//[================================================================================================]
// Constructor 2 (Basic Command Line Interaction + Debugging Enabled)
//[================================================================================================]
CLI::CLI(char** partition, bool debug)
{
   my_partition = partition[1];
   my_pid = std::to_string(getpid());
   client_sockpath = "cli-" + my_pid + "-socket";
   server_sockpath = "lson-" + my_pid + "-socket";
   dbug = debug;
   start();
}
//[================================================================================================]
// Constructor 2 (Used For Reading From A Text File)
//[================================================================================================]
CLI::CLI(char** partition, char* isScript)
{
   my_partition = partition[1];
   is_script = isScript;
   my_pid = std::to_string(getpid());
   client_sockpath = "cli-" + my_pid + "-socket";
   server_sockpath = "lson-" + my_pid + "-socket";
   start();
}
//[================================================================================================]
// Constructor 3 (Used For Reading From A Text File + Debugging Enabled)
//[================================================================================================]
CLI::CLI(char** partition, char* isScript, bool debug)
{
   my_partition = partition[1];
   is_script = isScript;
   my_pid = std::to_string(getpid());
   client_sockpath = "cli-" + my_pid + "-socket";
   server_sockpath = "lson-" + my_pid + "-socket";
   dbug = debug;
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

   if(dbug) std::cout << std::endl << std::endl;
   if(dbug) std::cout << "C: Command Line Interface Process Id: " << my_pid << std::endl;
   if(dbug) std::cout << "C: Command Line Interface Client Socket Path: " << client_sockpath << std::endl;
   if(dbug) std::cout << "C: Server Socket Path: " << server_sockpath << std::endl;
   if(dbug) std::cout << "C: Initiating Handshake With Liaison Process..." << std::endl;
   if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl <<std::endl;

   /* Data for shared memory operations */
   key_t shm_key;
   int shm_id;

   if(dbug) std::cout << "C: Generating Shared Memory Key..." << std::endl;
   shm_key = atoi(my_pid.c_str());
   if(dbug) std::cout << "C: Shared Memory Key: " << shm_key << std::endl;


   if(dbug) std::cout << "C: Generating Argument Vector For Liason Process..." << std::endl;
   std::string hostname;
   char temp[MAX_COMMAND_SIZE];
   memset(temp,'\0',MAX_COMMAND_SIZE);
   hostname = gethostname(temp,MAX_COMMAND_SIZE);
   /* Create char** to send to Liason via main() argv param */
   std::vector<char*> argv;
   argv.push_back(const_cast<char*>(client_sockpath.c_str()));
   argv.push_back(const_cast<char*>(server_sockpath.c_str()));
   argv.push_back(const_cast<char*>(my_pid.c_str()));
   argv.push_back(const_cast<char*>(hostname.c_str()));
   if(dbug) argv.push_back(const_cast<char*>("-d"));
   argv.push_back(NULL);
   if(dbug) std::cout << "C: Argument Vector Generated Successfully" << std::endl;


   if(dbug) std::cout << "C: Creating a Shared Memory Segment For Inter-Process Synchronization" << std::endl;
   char* shm = create_shm_seg(shm_key,shm_id);
   if(dbug) std::cout << "C: Shared Memory Segment Creation Successfull" << std::endl;
   shm[0] = 0;

   if(dbug) std::cout << "C: Initiating Liason Process Fork..." << std::endl;
   pid_t pid = fork();
   if(pid == 0)
   {
      /* Child Process */
      execv("liaison",argv.data());
   }
   else if(pid < 0)
   {
      /* Fork Failed */
      delete_shm(shm_id,shm);
      std::string where = "[cli.cpp::start()]: ";
      std::string what = "Fork Failed -- ";
      what += strerror(errno);
      throw arboreal_cli_error(what,where);
   }
   else
   {
      /* Parent Process */
      if(dbug) std::cout << "C: Fork Successful; Returned to CLI Process" << std::endl;
      if(dbug) std::cout << "C: Waiting For Liaison Process to Initiate Server Socket..." << std::endl;
      if(dbug) std::cout << "-----------------------------------------------------------------" << std::endl <<std::endl;

      /* Wait for Server to be set up */
      while(shm[0] == 0);

      if(dbug) std::cout << "C: Server Found" << std::endl;
      if(dbug) std::cout << "C: Initializing Server and Client Address Structures..." << std::endl;
      memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
      memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
      if(dbug) std::cout << "C: Server and Client Address Structures Successfully Initialized" << std::endl;

      if(dbug) std::cout << "C: Creating Client Socket..." << std::endl;
      client_sock = set_up_socket(client_sockpath,client_sockaddr);
      if(dbug) std::cout << "C: Client Socket Creation Successfull" << std::endl;
      if(dbug) std::cout << "C: Signaling Server..." << std::endl;

      /* Signal that Client is done setting up */
      shm[0] = 2;

      if(dbug) std::cout << "C: Deleting Shared Memory Segment..." << std::endl;
      delete_shm(shm_id,shm);
      if(dbug) std::cout << "C: Shared Memory Segment Deletion Successfull" << std::endl;


      if(dbug) std::cout << "C: Attempting to Connect to Server..." << std::endl;
      socklen_t length = sizeof(client_sockaddr);
      connect_to_server(client_sock,client_sockpath,server_sockpath,server_sockaddr,length);
      if(dbug) std::cout << "C: Server Connection Successfull" << std::endl;

      if(dbug) std::cout << "C: Building Handshake Command..." << std::endl;
      char* cmnd = new char[MAX_COMMAND_SIZE];
      int cmnd_id = 0;
      memset(cmnd,'\0',MAX_COMMAND_SIZE);
      memcpy(cmnd,&cmnd_id,sizeof(int));
      memcpy(cmnd + sizeof(int), "HANDSHAKE", sizeof("HANDSHAKE"));
      memcpy(cmnd + sizeof("HANDSHAKE") + sizeof(int) + 2, const_cast<char*>(my_partition.c_str()), my_partition.length());
      if(dbug) std::cout << "C: Handshake Command Built Successfully" << std::endl;

      if(dbug) std::cout << "C: Sending Command To Server..." << std::endl;
      if(dbug) std::cout << "C: Sending To: " << client_sock << " @ " << client_sockpath << std::endl;
      send_to_server(client_sock,client_sockpath,cmnd,MAX_COMMAND_SIZE,FLAG);
      if(dbug) std::cout << "C: Command Successfully Sent" << std::endl;

      if(dbug) std::cout << "C: Awaiting Response From Server..." << std::endl;
      char* data = receive_from_server(client_sock,client_sockpath,MAX_COMMAND_SIZE,FLAG);
      if(get_cmnd_id(data) == 999)
      {
         std::cerr << "C: Connection To File System Could Not Be Established; Exiting..." << std::endl;
         if(dbug) std::cout << "C: Closing Client Socket Connection..." << std::endl;
         if(close(client_sock) < 0)
         {
            std::string where = "[cli.cpp::start()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
         }
         if(dbug) std::cout << "C: Client Socket Closed Successfully" << std::endl;
         if(dbug) std::cout << "C: Removing Client Socket..." << std::endl;
         if(unlink(client_sockpath.c_str()) < 0)
         {
            std::string where = "[cli.cpp::start()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
         }
         if(dbug) std::cout << "C: Client Socket Removed Successfully" << std::endl;
         if(dbug) std::cout << "C: Waiting For Child Process to Complete..." << std::endl;
   
         int status;
         waitpid(pid,&status,0);
         if(dbug) std::cout << "C: Child Process Completed Successfully" << std::endl;
         if(dbug) std::cout << "C: Quitting Command Line Interface..." << std::endl;
         exit(1);
      }

      std::string dta = data;
      if(dbug) std::cout << "C: Data Received: " << dta << std::endl;
      if(dbug) std::cout << "C: Data Received From: " << client_sock << " @ " << client_sockpath << std::endl;
      if(dbug) std::cout << "C: Handshake Completed Successfully" << std::endl;
      delete[] cmnd;
      delete[] data;
      if(dbug) std::cout << "=================================================================" << std::endl << std::endl;


      if(dbug) std::cout << "C: Running Command Line Interface..." << std::endl;
      max_string_size = 64;
      run();

      if(dbug) std::cout << "C: Closing Client Socket Connection..." << std::endl;
      if(close(client_sock) < 0)
      {
         std::string where = "[cli.cpp::start()]: ";
         std::string what = "Client Socket Close Failed -- ";
         what += strerror(errno);
         throw arboreal_cli_error(what,where);
      }
      if(dbug) std::cout << "C: Client Socket Closed Successfully" << std::endl;
      if(dbug) std::cout << "C: Removing Client Socket..." << std::endl;
      if(unlink(client_sockpath.c_str()) < 0)
      {
         std::string where = "[cli.cpp::start()]: ";
         std::string what = "Client Socket Unlink Failed -- ";
         what += strerror(errno);
         throw arboreal_cli_error(what,where);
      }
      if(dbug) std::cout << "C: Client Socket Removed Successfully" << std::endl;
      if(dbug) std::cout << "C: Waiting For Child Process to Complete..." << std::endl;

      int status;
      waitpid(pid,&status,0);
      if(dbug) std::cout << "C: Child Process Completed Successfully" << std::endl;
      if(dbug) std::cout << "C: Quitting Command Line Interface..." << std::endl;
   }
}
//[================================================================================================]
// Begin The Command Reading Loop
//[================================================================================================]
void CLI::run()
{


   // user input
   std::string input;
   // last 10 inputs
   std::vector<std::string> history;
                           
   bool from_history = false; // unimportant at the moment
   bool print_prompt = false; // Reprint prompt after bad command (but only after a bad command)
   
   // Print the welcome header
   print_header();


   // Operations will differ slightly if reading commands from a text file
   if(is_script == "-s")
   {
      std::cout << "Reading from input file...\n";
      std::cout << "+---------------------------------------\n\n\n";
   }

   // Begin read loop
   while(true)
   {
      if(print_prompt)
      {
         std::cout << "Arboreal >> ";
         print_prompt = false;
      }
      // Check for ENTER pressed (prints out a new line with 'Arboreal >> ')
      char c = std::cin.get();
      if(c == '\n')
      {
         if(is_script != "-s"){std::cout << "Arboreal >> ";}
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
            std::cin.putback(c);

            // Get the command
            getline(std::cin,input);
   
            // Add command to history
            history.push_back(input);
            if(is_script == "-s"){std::cout << "Arboreal >> " << input << std::endl;}
         }
         from_history = false;

         if(input == "quit" || input == "q" || input == "Q")
         {
            // May need to do more than return in order to make
            // sure we dont corrupt data
            std::cout << "Are you sure you would like to quit? (Y/N)\n";
            std::cin >> input;

            if(input == "Y" || input == "y")
            {
               /* Send QUIT Command to Liaison Process */
               char* quit = new char[MAX_COMMAND_SIZE];
               memset(quit,'\0',MAX_COMMAND_SIZE);
               int val = 999;
               memcpy(quit,&val,sizeof(int));
               memcpy(quit + sizeof(int), "QUIT", sizeof("QUIT"));
               send_cmnd(quit);
               return;
            }
            else{continue;}
         }
         else if(input == "h" || input == "help")
         {
            print_help();
         }
         else if(input == "history")
         {
            for(unsigned int i = 0; i < history.size(); i++)
            {
               std::cout << i << ": " << history[i] << "\n";
            }
            if(history.size() >= MAX_HISTORY_SIZE)
            {
               history.clear();
            }
            if(is_script == "-s"){std::cout << "\n";}
            if(is_script != "-s"){std::cout << "Arboreal >> ";}
         }

         // If using a text file to read in commands
         if(input == "end" && is_script == "-s")
         {
            /* Send QUIT Command to Liaison Process */
            char* quit = new char[MAX_COMMAND_SIZE];
            memset(quit,'\0',MAX_COMMAND_SIZE);
            int val = 999;
            memcpy(quit,&val,sizeof(int));
            memcpy(quit + sizeof(int), "QUIT", sizeof("QUIT"));
            send_cmnd(quit);
            return;
         }
         else
         {
            // Test if the command is valid
            int rtrn = check_command(input);
            if(!check_name_size(input,rtrn,max_string_size))
            {
               std::cerr << "One or more tag or file names is too long!" << std::endl;
               std::cerr << "Maximum name size is: " << max_string_size << std::endl;
               print_prompt = true;
               continue;
            }

            if(rtrn != 0)
            {
               send_cmnd(build(rtrn,input));
               await_response();
               if(is_script != "-s"){std::cout << "Arboreal >> ";}
            }
            else
            {
               std::cerr << "Comand Not Valid\n";
               if(is_script != "-s"){std::cout << "Arboreal >> ";}
            }

         }
      }
   }
   return;
}
//[================================================================================================]
// Send Command To Liaison Process
// 
// @ cmnd: Command to be sent
//[================================================================================================]
void CLI::send_cmnd(char* cmnd)
{

   if(dbug) std::cout << "C: Sending Command To Server..." << std::endl;
   if(dbug) std::cout << "C: Sending To: " << client_sock << " @ " << client_sockpath << std::endl;
   send_to_server(client_sock,client_sockpath,cmnd,MAX_COMMAND_SIZE,FLAG);
   if(dbug) std::cout << "C: Command Successfully Sent" << std::endl;
   return;
}
//[================================================================================================]
// Await Data From Liaison Process
//[================================================================================================]
void CLI::await_response()
{
   if(dbug) std::cout << "C: Awaiting Response From Server..." << std::endl;
   char* data = receive_from_server(client_sock,client_sockpath,MAX_COMMAND_SIZE,FLAG);
   std::string dta = data;
   if(dbug) std::cout << "C: Data Received: " << dta << std::endl;
   return;
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
char* CLI::build(int id, std::string input)
{
   char* command = new char[MAX_COMMAND_SIZE];
   int offset = 0;

   // Zero out the command buffer
   memset(command,'\0',MAX_COMMAND_SIZE);


   switch(id)
   {
      case(4): // find -t [tagName,...] }--> Find file(s) by tag(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);

         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         if(dbug) std::cout << "Size of built command: " << sizeof(command) << std::endl;
         return command;
      }
//[================================================================================================]
      
      case(5): // find -t {tagName,...} }--> Find file with all tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,EXCLUSIVE,max_string_size);

         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]
      
      case(6): // find -f [filename.ext,...] }--> Find files by name
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]
      
      case(7): // new -t [tagName,...] }--> Create tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);

         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]
      
      case(8): // new -f [filename.ext,...] }--> Create files
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);

         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(9): // new -f [filename.ext,...] -t [tagName,...] }--> Create & Tag Files 1:1
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,NEW_AND_TAG,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(10): // new -f [filename.ext,...] -t {tagName,...} }--> Create & Tag Files w/ ALL tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,NEW_AND_TAG_EXC,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(11): // delete -t [tagName,...] }--> Delete tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(12): // delete -f [filename.ext,...] }--> Delete files  
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(13): // fdelete -t [tagName,...] }--> Force delete tags 
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case (14): // open [filename.ext,...] }--> Open file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,OPEN,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(15): // close [filename.ext,...] }--> Close file(s)  
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(16): // rename -t [tagName,...] -n [newName,...] }--> Rename Tag(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,NEW_AND_TAG,max_string_size);

         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(17): // rename -f [filename.ext,...] -n [newName.ext,...] }--> Rename file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,NEW_AND_TAG,max_string_size);

         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(18): // get [filename.ext,...] }--> Get attributes of file(s)
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,INCLUSIVE,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(19): // merge tagName1 -> tagName2 }--> Merge two existing tags
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,MERGE_1,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(20): // merge [tagName,...] -> tagName }--> Merge multiple tags into one
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,MERGE_2,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(21): // tag filename.ext -> tagName }--> Tag a file with a tag
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,TAG_1,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(22): // tag filename.ext -> [tagName,...] }--> Add tags to file
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,TAG_2,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
//[================================================================================================]

      case(23): // tag [filename.ext,...] -> tagName }--> Tag multiple files
      {
         // Add the command Id to the command,
         // then update offset and convert the command
         memcpy(command,&id,sizeof(int));
         offset += sizeof(int);
         write_to_cmnd(command,input,offset,TAG_3,max_string_size);
         
         // Some Debug Printing
         std::cout << "Value: " << get_cmnd_id(command) << std::endl;
         print_buffer(command,MAX_COMMAND_SIZE);
         check_buffer_partitioning(command,MAX_COMMAND_SIZE);

         // Check Buffer Integerity
         if(!check_buffer(command,MAX_COMMAND_SIZE))
         {
            fix_buffer(command,MAX_COMMAND_SIZE);

            // Debug Print
            print_buffer(command,MAX_COMMAND_SIZE);
         }
         return command;
      }
   }

   // For completeness' sake
   return command;
}
//[================================================================================================]














