//////////////////////////////////////////////////////////////////////////////////////////////////// 
//
// cli.cpp
// Comand Line Interface Class Definitions
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
// 
//  Sun. | Jan. 28th | 2018 | 8:30 PM
// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "cli.h"
#include "cmnd_validation.h"
#include "cli_helper.hpp"

#define PERMISSIONS 0666
#define DEBUG false



// Constructor 1 (Basic Command Line Interaction)
//[================================================================================================]
CLI::CLI(char** partition)
{
   my_partition = partition[1];
   my_pid = std::to_string(getpid());
   client_socket_path = "cli-" + my_pid + "-socket";
   server_socket_path = "lson-" + my_pid + "-socket";
   start();
}
//[================================================================================================]







//[================================================================================================]
CLI::CLI(char** partition, bool debug)
{
   my_partition = partition[1];
   my_pid = std::to_string(getpid());
   client_socket_path = "cli-" + my_pid + "-socket";
   server_socket_path = "lson-" + my_pid + "-socket";
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
   client_socket_path = "cli-" + my_pid + "-socket";
   server_socket_path = "lson-" + my_pid + "-socket";
   start();
}
//[================================================================================================]






//[================================================================================================]
CLI::CLI(char** partition, char* isScript, bool debug)
{
   my_partition = partition[1];
   is_script = isScript;
   my_pid = std::to_string(getpid());
   client_socket_path = "cli-" + my_pid + "-socket";
   server_socket_path = "lson-" + my_pid + "-socket";
   dbug = debug;
   start();
}
//[================================================================================================]






// Default Destructor
//[================================================================================================]
CLI::~CLI()
{
   delete my_partition;
   delete shared_mem;
}
//[================================================================================================]







// Run Initial Setup (Store Info, Perform Handshakes, etc.)
//[================================================================================================]
void CLI::start()
{

   /* Data for shared memory operations */
   key_t shm_key;
   char *shm;
   shm_key = atoi(my_pid.c_str());

   /* Create char** to send to Liason via main() argv param */
   std::vector<char*> argv;
   argv.push_back(const_cast<char*>(server_socket_path.c_str()));
   argv.push_back(const_cast<char*>(client_socket_path.c_str()));
   argv.push_back(const_cast<char*>(my_pid.c_str()));
   argv.push_back(NULL);

   /* Zero the server and client socket addresses */
   memset(&server_sockaddr, '\0', sizeof(struct sockaddr_un));
   memset(&client_sockaddr, '\0', sizeof(struct sockaddr_un));
//-------------------------------------------------------------------------------------------------


   /* Fork a Liason Process that will be tied to this CLI */
   pid_t pid = fork();

   if(pid == 0)
   {
      /* Child (will be replaced with Liason process) */
      execv("liason",argv.data());
   }
   else if(pid < 0)
   {
      /* Fork Failed */
      CLI_EX err;
      err.what = "[CLI:WHAT] | FORK FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \'man fork\' for more information\n";
      throw err;
   }
   else
   {

            // Begin Shared Memory Set-Up \\
         

      /* Returned to parent */
      if(dbug) std::cout << "Going To Sleep...\n";

      /* Sleep a sec in order to allow Liason to get the ball rolling */
      sleep(2);


      /* Get the shared memory segment so that the CLI knows when it can continue on */
      if ((shm_id = shmget(shm_key, SHMSZ, PERMISSIONS)) < 0) 
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | SHARED MEMORY GET FAILED - E#: " << std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man shmget\' for more information\n";
         throw err;
      }

      /* Attach the shared memory to this process so the CLI can access it */
      if ((shm = (char*)shmat(shm_id, NULL, 0)) == (char *) -1) 
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | SHARED ATTACH FAILED - E#: " << std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man shmat\' for more information\n";
         throw err;  
      }

      /* Not honestly sure why this is necessary but all of the UNIX examples had this */
      shared_mem = shm;

      if(dbug) std::cout << "Waiting on Server...\n";

      /* Block until Liason gives the OK */
      while(shared_mem[0] == 0);
//-------------------------------------------------------------------------------------------------

               // Begin Socket Creation \\


      /* Create a socket for the client i.e. CLI */
      client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
      if (client_sock == -1) 
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | SOCKET CREATION FAILED - E#: " + std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man socket\' for more information\n";
         throw err;
      }


      /* Set-Up Client Socket */
      client_sockaddr.sun_family = AF_UNIX;   
      strcpy(client_sockaddr.sun_path, client_socket_path.c_str()); 
      len = sizeof(client_sockaddr);

      /* Avoid "This socket is already in use" errors */ 
      unlink(client_socket_path.c_str());

      /* Bind the client address to the socket */
      rc = bind(client_sock,(struct sockaddr *)&client_sockaddr, len);
      if(rc == -1)
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | SOCKET BIND FAILED - E#: " + std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man bind \' for more information\n";
         close(client_sock);
         throw err;
      }
//-------------------------------------------------------------------------------------------------



      /* Set-Up Server Socket and Connect */
      server_sockaddr.sun_family = AF_UNIX;
      strcpy(server_sockaddr.sun_path, server_socket_path.c_str());

      /* Connect to the server socket */
      rc = connect(client_sock, (struct sockaddr *)&server_sockaddr, len);
      if(rc == -1)
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | CONNECT TO SERVER FAILED - E#: " + std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man connect\' for more information\n";
         close(client_sock);
         throw err;
      }

      if (dbug) std::cout << "Initiating Handshake...\n";
//-------------------------------------------------------------------------------------------------



      /* Build "handshake" command */
      char init[MAX_COMAND_SIZE];
      memset(init,'\0',MAX_COMAND_SIZE);
      memcpy(init,"e",sizeof(int));
      memcpy(init+sizeof(int),"init: handshake",sizeof("init: handshake"));


      /* Send the command */
      rc = send(client_sock, init, MAX_COMAND_SIZE, 0);
      if(rc == -1)
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | MESSAGE SEND FAILED - E#: " + std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \' man send\' for more information\n";
         close(client_sock);
         throw err;
      }
      if(dbug) std::cout << "Handshake Established!\n";
      if(dbug) std::cout << "Awaiting Response...\n";
//-------------------------------------------------------------------------------------------------



      /* Prepare to receive response from the Liason */
      char response_buff[MAX_COMAND_SIZE];
      memset(response_buff,'\0',MAX_COMAND_SIZE);


      /* Receive Response */
      rc = recv(client_sock,response_buff,MAX_COMAND_SIZE,0);
      if(rc == -1)
      {
         CLI_EX err;
         err.what = "[CLI:WHAT] | MESSAGE RECEIVE FAILED - E#: " + std::to_string(errno) + "\n";
         err.why = "[CLI:WHY]  | See \'man recv\' for more information\n";
         close(client_sock);
         throw err;
      }

      if(dbug) std::cout << "Response Received: " << response_buff << std::endl;
//-------------------------------------------------------------------------------------------------



      // Eventually need to change this basd on what the FS says
      max_string_size = 64;

      /* Print some info for the user */
      std::cout << "\n\nMaximum Filename/Tagname size is: " << max_string_size << std::endl;
      std::cout << "\n\nInitiating Command Line Interface Using Partition: " << my_partition << std::endl;
      std::cout << ".................................................................................\n\n";
   
      /* Close the socket and run the input gathering loop */
      close(client_sock);
      run();

      /* Wait for child (Liason) process to complete */
      int status;
      waitpid(pid,&status,0);

      if(dbug) std::cout << "Wait Status: " << status << std::endl;
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
   std::string input;

   // last 10 inputs
   std::vector<std::string> history;
                           
   bool from_history = false; // unimportant at the moment
   
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
      // Check for ENTER pressed (prints out a new line with 'Arboreal >> ')
      char c = std::cin.get();
      if(c == '\n')
      {
         if(is_script != "-s"){std::cout << "Arboreal >> \n";}
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

         if(input == "quit" || input == "q")
         {
            // May need to do more than return in order to make
            // sure we dont corrupt data
            std::cout << "Are you sure you would like to quit? (Y/N)\n";
            std::cin >> input;
            if(input == "Y" || input == "y")
            {
               client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
               if (client_sock == -1) 
               {
                  CLI_EX err;
                  err.what = "[CLI:WHAT] | SOCKET CREATION FAILED - E#: " + std::to_string(errno) + "\n";
                  err.why = "[CLI:WHY]  | See \'man socket\' for more information\n";
                  throw err;
               }
            
            
               /* Set-Up Client Socket */
               client_sockaddr.sun_family = AF_UNIX;   
               strcpy(client_sockaddr.sun_path, client_socket_path.c_str()); 
               len = sizeof(client_sockaddr);
               unlink(client_socket_path.c_str());
            
            
               rc = bind(client_sock,(struct sockaddr *)&client_sockaddr, len);
               if(rc == -1)
               {
                  CLI_EX err;
                  err.what = "[CLI:WHAT] | SOCKET BIND FAILED - E#: " + std::to_string(errno) + "\n";
                  err.why = "[CLI:WHY]  | See \'man bind \' for more information\n";
                  close(client_sock);
                  throw err;
               }
            
            
               /* Set-Up Server Socket and Connect */
               server_sockaddr.sun_family = AF_UNIX;
               strcpy(server_sockaddr.sun_path, server_socket_path.c_str());
            
            
               rc = connect(client_sock, (struct sockaddr *)&server_sockaddr, len);
               if(rc == -1)
               {
                  CLI_EX err;
                  err.what = "[CLI:WHAT] | CONNECT TO SERVER FAILED - E#: " + std::to_string(errno) + "\n";
                  err.why = "[CLI:WHY]  | See \'man connect\' for more information\n";
                  close(client_sock);
                  throw err;
               }

               char buf[MAX_COMAND_SIZE];
               memset(buf,'\0',MAX_COMAND_SIZE);
               memcpy(buf,"e",sizeof(int));
               memcpy(buf+sizeof(int), "quit", sizeof("quit"));
               
               rc = send(client_sock, buf, MAX_COMAND_SIZE,0);
               if(rc == -1)
               {
                  CLI_EX err;
                  err.what = "[CLI:WHAT] | MESSAGE SEND FAILED - E#: " + std::to_string(errno) + "\n";
                  err.why = "[CLI:WHY]  | See \' man send\' for more information\n";
                  close(client_sock);
                  throw err;
               }
            
            
               std::cout << "Awaiting Response...\n";
               char buff[MAX_COMAND_SIZE];
               memset(buff,'\0',MAX_COMAND_SIZE);
                  
               rc = recv(client_sock,buff,sizeof(buff),0);
               if(rc == -1)
               {
                  CLI_EX err;
                  err.what = "[CLI:WHAT] | MESSAGE RECEIVE FAILED - E#: " + std::to_string(errno) + "\n";
                  err.why = "[CLI:WHY]  | See \'man recv\' for more information\n";
                  close(client_sock);
                  throw err;
               }
               close(client_sock);
               unlink(client_socket_path.c_str());
               close(shm_id);
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
            std::cout << "\n\n";
            return;
         }
         else
         {
            // Test if the command is valid
            int rtrn = check_command(input);
            if(rtrn != 0)
            {
               // Debug
               std::cout << "Return: " << rtrn << std::endl;
               send_cmnd(build(rtrn,input));
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











//[================================================================================================]
void CLI::send_cmnd(char* cmnd)
{

   // Set-Up
   
   /* Create Socket */
   client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
   if (client_sock == -1) 
   {
      CLI_EX err;
      err.what = "[CLI:WHAT] | SOCKET CREATION FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \'man socket\' for more information\n";
      throw err;
   }


   /* Set-Up Client Socket */
   client_sockaddr.sun_family = AF_UNIX;   
   strcpy(client_sockaddr.sun_path, client_socket_path.c_str()); 
   len = sizeof(client_sockaddr);


   /* Avoid "This socket already in use" errors */
   unlink(client_socket_path.c_str());


   /* Bind client address to the socket */
   rc = bind(client_sock,(struct sockaddr *)&client_sockaddr, len);
   if(rc == -1)
   {
      CLI_EX err;
      err.what = "[CLI:WHAT] | SOCKET BIND FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \'man bind \' for more information\n";
      close(client_sock);
      throw err;
   }


   /* Set-Up Server Socket and Connect */
   server_sockaddr.sun_family = AF_UNIX;
   strcpy(server_sockaddr.sun_path, server_socket_path.c_str());


   rc = connect(client_sock, (struct sockaddr *)&server_sockaddr, len);
   if(rc == -1)
   {
      CLI_EX err;
      err.what = "[CLI:WHAT] | CONNECT TO SERVER FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \'man connect\' for more information\n";
      close(client_sock);
      throw err;
   }
//-------------------------------------------------------------------------------------------------

   // Communication

   rc = send(client_sock, cmnd, MAX_COMAND_SIZE,0);
   if(rc == -1)
   {
      CLI_EX err;
      err.what = "[CLI:WHAT] | MESSAGE SEND FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \' man send\' for more information\n";
      close(client_sock);
      throw err;
   }

   if(dbug) std::cout << "Awaiting Response...\n";

   /* Set-Up Response Buffer */
   char response_buff[MAX_COMAND_SIZE];
   memset(response_buff,'\0',MAX_COMAND_SIZE);

   rc = recv(client_sock,response_buff,sizeof(buff),0);
   if(rc == -1)
   {
      CLI_EX err;
      err.what = "[CLI:WHAT] | MESSAGE RECEIVE FAILED - E#: " + std::to_string(errno) + "\n";
      err.why = "[CLI:WHY]  | See \'man recv\' for more information\n";
      close(client_sock);
      throw err;
   }
   close(client_sock);
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
char* CLI::build(int id,std::string input)
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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
         std::cout << "Value: " << get_cmnd_id(cmnd) << std::endl;
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














