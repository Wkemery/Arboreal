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
#include <sys/types.h>
#include <sys/wait.h>


//[================================================================================================]
// Constructor 1 (Basic Command Line Interaction)
//[================================================================================================]
CLI::CLI(char** partition)
{
   _my_partition = partition[1];
   _my_pid = std::to_string(getpid());
   _client_sockpath = "cli-" + _my_pid + "-socket";
   _server_sockpath = "lson-" + _my_pid + "-socket";
   start();
}
//[================================================================================================]
// Constructor 2 (Basic Command Line Interaction + Debugging Enabled)
//[================================================================================================]
CLI::CLI(char** partition, bool debug)
{
   _my_partition = partition[1];
   _my_pid = std::to_string(getpid());
   _client_sockpath = "cli-" + _my_pid + "-socket";
   _server_sockpath = "lson-" + _my_pid + "-socket";
   _dbug = debug;
   start();
}
//[================================================================================================]
// Constructor 2 (Used For Reading From A Text File)
//[================================================================================================]
CLI::CLI(char** partition, char* isScript)
{
   _my_partition = partition[1];
   _is_script = isScript;
   _my_pid = std::to_string(getpid());
   _client_sockpath = "cli-" + _my_pid + "-socket";
   _server_sockpath = "lson-" + _my_pid + "-socket";
   start();
}
//[================================================================================================]
// Constructor 3 (Used For Reading From A Text File + Debugging Enabled)
//[================================================================================================]
CLI::CLI(char** partition, char* isScript, bool debug)
{
   _my_partition = partition[1];
   _is_script = isScript;
   _my_pid = std::to_string(getpid());
   _client_sockpath = "cli-" + _my_pid + "-socket";
   _server_sockpath = "lson-" + _my_pid + "-socket";
   _dbug = debug;
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

   if(_dbug) std::cout << std::endl << std::endl;
   if(_dbug) std::cout << "C: Command Line Interface Process Id: " << _my_pid << std::endl;
   if(_dbug) std::cout << "C: Command Line Interface Client Socket Path: " << _client_sockpath << std::endl;
   if(_dbug) std::cout << "C: Server Socket Path: " << _server_sockpath << std::endl;
   if(_dbug) std::cout << "C: Initiating Handshake With Liaison Process..." << std::endl;
   if(_dbug) std::cout << "-----------------------------------------------------------------" << std::endl <<std::endl;

   /* Data for shared memory operations */
   key_t shm_key;
   int shm_id;

   if(_dbug) std::cout << "C: Generating Shared Memory Key..." << std::endl;
   shm_key = atoi(_my_pid.c_str());
   if(_dbug) std::cout << "C: Shared Memory Key: " << shm_key << std::endl;


   if(_dbug) std::cout << "C: Generating Argument Vector For Liason Process..." << std::endl;
   std::string hostname;
   char temp[MAX_COMMAND_SIZE];
   memset(temp,'\0',MAX_COMMAND_SIZE);
   hostname = gethostname(temp,MAX_COMMAND_SIZE);
   /* Create char** to send to Liason via main() argv param */
   std::vector<char*> argv;
   argv.push_back(const_cast<char*>(_client_sockpath.c_str()));
   argv.push_back(const_cast<char*>(_server_sockpath.c_str()));
   argv.push_back(const_cast<char*>(_my_pid.c_str()));
   argv.push_back(const_cast<char*>(hostname.c_str()));
   argv.push_back(const_cast<char*>(_my_partition.c_str()));
   argv.push_back(const_cast<char*>(_cwd.c_str()));
   if(_dbug) argv.push_back(const_cast<char*>("-d"));
   argv.push_back(NULL);
   if(_dbug) std::cout << "C: Argument Vector Generated Successfully" << std::endl;


   if(_dbug) std::cout << "C: Creating a Shared Memory Segment For Inter-Process Synchronization" << std::endl;
   char* shm = create_shm_seg(shm_key,shm_id);
   if(_dbug) std::cout << "C: Shared Memory Segment Creation Successfull" << std::endl;
   shm[0] = 0;

   if(_dbug) std::cout << "C: Initiating Liason Process Fork..." << std::endl;
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
      if(_dbug) std::cout << "C: Fork Successful; Returned to CLI Process" << std::endl;
      if(_dbug) std::cout << "C: Waiting For Liaison Process to Initiate Server Socket..." << std::endl;
      if(_dbug) std::cout << "-----------------------------------------------------------------" << std::endl <<std::endl;

      /* Wait for Server to be set up */
      while(shm[0] == 0);

      if(_dbug) std::cout << "C: Server Found" << std::endl;
      if(_dbug) std::cout << "C: Initializing Server and Client Address Structures..." << std::endl;
      memset(&_server_sockaddr, 0, sizeof(struct sockaddr_un));
      memset(&_client_sockaddr, 0, sizeof(struct sockaddr_un));
      if(_dbug) std::cout << "C: Server and Client Address Structures Successfully Initialized" << std::endl;

      if(_dbug) std::cout << "C: Creating Client Socket..." << std::endl;
      _client_sock = set_up_socket(_client_sockpath,_client_sockaddr);
      if(_dbug) std::cout << "C: Client Socket Creation Successfull" << std::endl;
      if(_dbug) std::cout << "C: Signaling Server..." << std::endl;

      /* Signal that Client is done setting up */
      shm[0] = 2;

      if(_dbug) std::cout << "C: Deleting Shared Memory Segment..." << std::endl;
      delete_shm(shm_id,shm);
      if(_dbug) std::cout << "C: Shared Memory Segment Deletion Successfull" << std::endl;


      if(_dbug) std::cout << "C: Attempting to Connect to Server..." << std::endl;
      socklen_t length = sizeof(_client_sockaddr);
      connect_to_server(_client_sock,_client_sockpath,_server_sockpath,_server_sockaddr,length);
      if(_dbug) std::cout << "C: Server Connection Successfull" << std::endl;

      if(_dbug) std::cout << "C: Building Handshake Command..." << std::endl;
      char* cmnd = new char[MAX_COMMAND_SIZE];
      int cmnd_id = 0;
      memset(cmnd,'\0',MAX_COMMAND_SIZE);
      memcpy(cmnd,&cmnd_id,sizeof(int));
      memcpy(cmnd + sizeof(int), "HANDSHAKE-", sizeof("HANDSHAKE-"));
      memcpy(cmnd + (sizeof(int) + sizeof("HANDSHAKE-")) - 1, const_cast<char*>(_my_partition.c_str()), _my_partition.length());
      if(_dbug) std::cout << "C: Handshake Command Built Successfully" << std::endl;

      if(_dbug) std::cout << "C: Sending Command To Server..." << std::endl;
      if(_dbug) std::cout << "C: Sending To: " << _client_sock << " @ " << _client_sockpath << std::endl;
      send_to_server(_client_sock,_client_sockpath,cmnd,MAX_COMMAND_SIZE,FLAG);
      if(_dbug) std::cout << "C: Command Successfully Sent" << std::endl;

      if(_dbug) std::cout << "C: Awaiting Response From Server..." << std::endl;
      char* data = receive_from_server(_client_sock,_client_sockpath,MAX_COMMAND_SIZE,FLAG);
      if(get_cmnd_id(data) == 999)
      {
         std::cerr << "C: Connection To File System Could Not Be Established; Exiting..." << std::endl;
         if(_dbug) std::cout << "C: Closing Client Socket Connection..." << std::endl;
         if(close(_client_sock) < 0)
         {
            std::string where = "[cli.cpp::start()]: ";
            std::string what = "Client Socket Close Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
         }
         if(_dbug) std::cout << "C: Client Socket Closed Successfully" << std::endl;
         if(_dbug) std::cout << "C: Removing Client Socket..." << std::endl;
         if(unlink(_client_sockpath.c_str()) < 0)
         {
            std::string where = "[cli.cpp::start()]: ";
            std::string what = "Client Socket Unlink Failed -- ";
            what += strerror(errno);
            throw arboreal_cli_error(what,where);
         }
         if(_dbug) std::cout << "C: Client Socket Removed Successfully" << std::endl;
         if(_dbug) std::cout << "C: Waiting For Child Process to Complete..." << std::endl;

         int status;
         waitpid(pid,&status,0);
         if(_dbug) std::cout << "C: Child Process Completed Successfully" << std::endl;
         if(_dbug) std::cout << "C: Quitting Command Line Interface..." << std::endl;
         exit(1);
      }

      std::string dta = data;
      if(_dbug) std::cout << "C: Data Received: " << dta << std::endl;
      if(_dbug) std::cout << "C: Data Received From: " << _client_sock << " @ " << _client_sockpath << std::endl;
      _max_string_size = get_cmnd_id(data);
      if(_dbug) std::cout << "C: Maximum Filename Size: " << _max_string_size << std::endl;
      if(_dbug) std::cout << "C: Handshake Completed Successfully" << std::endl;
      delete[] cmnd;
      delete[] data;
      if(_dbug) std::cout << "=================================================================" << std::endl << std::endl;


      if(_dbug) std::cout << "C: Running Command Line Interface..." << std::endl;
      run();

      if(_dbug) std::cout << "C: Closing Client Socket Connection..." << std::endl;
      if(close(_client_sock) < 0)
      {
         std::string where = "[cli.cpp::start()]: ";
         std::string what = "Client Socket Close Failed -- ";
         what += strerror(errno);
         throw arboreal_cli_error(what,where);
      }
      if(_dbug) std::cout << "C: Client Socket Closed Successfully" << std::endl;
      if(_dbug) std::cout << "C: Removing Client Socket..." << std::endl;
      if(unlink(_client_sockpath.c_str()) < 0)
      {
         std::string where = "[cli.cpp::start()]: ";
         std::string what = "Client Socket Unlink Failed -- ";
         what += strerror(errno);
         throw arboreal_cli_error(what,where);
      }
      if(_dbug) std::cout << "C: Client Socket Removed Successfully" << std::endl;
      if(_dbug) std::cout << "C: Waiting For Child Process to Complete..." << std::endl;

      int status;
      waitpid(pid,&status,0);
      if(_dbug) std::cout << "C: Child Process Completed Successfully" << std::endl;
      if(_dbug) std::cout << "C: Quitting Command Line Interface..." << std::endl;
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
   if(_is_script == "-s")
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
         if(_is_script != "-s"){std::cout << "Arboreal >> ";}
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
            if(_is_script == "-s"){std::cout << "Arboreal >> " << input << std::endl;}
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
            if(_is_script == "-s"){std::cout << "\n";}
            if(_is_script != "-s"){std::cout << "Arboreal >> ";}
         }

         // If using a text file to read in commands
         if(input == "end" && _is_script == "-s")
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
            if(rtrn != 0)
            {
               if(rtrn == 21)
               {
                  std::string old = _cwd;
                  std::string temp = input.substr(3,input.length());
                  _cwd = temp;
                  char* new_cwd = new char[MAX_COMMAND_SIZE];
                  memset(new_cwd,'\0',MAX_COMMAND_SIZE);
                  memcpy(new_cwd,&rtrn,sizeof(int));
                  memcpy(new_cwd + sizeof(int),_cwd.c_str(),_cwd.length());
                  send_cmnd(new_cwd);
                  await_response();
                  if(_is_script != "-s"){std::cout << "\nArboreal >> ";}
                  continue;
               }
               send_cmnd(build(rtrn,input));
               await_response();
               if(_is_script != "-s"){std::cout << "Arboreal >> ";}
            }
            else
            {
               std::cerr << "Comand Not Valid\n";
               if(_is_script != "-s"){std::cout << "Arboreal >> ";}
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

   if(_dbug) std::cout << "C: Sending Command To Server..." << std::endl;
   if(_dbug) std::cout << "C: Sending To: " << _client_sock << " @ " << _client_sockpath << std::endl;
   send_to_server(_client_sock,_client_sockpath,cmnd,MAX_COMMAND_SIZE,FLAG);
   if(_dbug) std::cout << "C: Command Successfully Sent" << std::endl;
   return;
}
//[================================================================================================]
// Await Data From Liaison Process
//[================================================================================================]
void CLI::await_response()
{
   if(_dbug) std::cout << "C: Awaiting Response From Server..." << std::endl;
   std::string data = receive_from_server(_client_sock,_client_sockpath);
   if(data == "WAIT")
   {
      char* temp = receive_from_server(_client_sock,_client_sockpath, MAX_COMMAND_SIZE, FLAG);
      int read_size = get_cmnd_id(temp);
      delete[] temp;

      char* data = receive_from_server(_client_sock,_client_sockpath, read_size, FLAG);
      std::string to_print = data;
      std::cout << to_print << std::endl;
      delete[] data;
   }
   else
   {
      std::cout << data << std::endl;
   }
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

   // Zero out the command buffer
   memset(command,'\0',MAX_COMMAND_SIZE);

   // Add the Command ID to the command string and convert to char*
   memcpy(command,&id,sizeof(int));
   memcpy(command + sizeof(int),const_cast<char*>(input.c_str()),input.length());
   return command;
}
