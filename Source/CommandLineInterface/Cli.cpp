///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Cli.cpp
// Comand Line Interface Class Definitions and Driver
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
//
//  Tue. | Mar. 20th | 2018 | 10:50 PM | Stable | Documented
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "CLHeaders/Cli.h"
#include "CLDependancies/cli_helper.hpp"

//[===============================================================================================]
/*!
 * The Command Line has several different modes of execution
 *
 * Mode 1:
 *   The most standard mode requires that a partition name be passed
 *   as an argument. The partition name must exist on the filesystem
 *   if it does not, the commandline will quit. This mode expects the user
 *   to manually type commands into the command line interface.
 *   This mode's run command looks like:
 *     './commandline PartitionName'
 *
 * Mode 2:
 *   The second mode adds debugging information to Mode 1
 *   The flag that must be passed to enable this mode is '-d'
 *   This mode's run command looks like:
 *     './commandline PartitionName -d'
 *
 * Mode 3:
 *   The third mode operates simmilar to Mode 1 except that rather than
 *   expecting users to manually type commands in, it expects a file
 *   containing all of the commands that will be executed, to be piped to it.
 *   This mode still requires that a legal partition be passed.
 *   The flag that must be passed to enable this mode is '-d'
 *   This mode's run command looks like:
 *     '.commandline PartitionName -s'
 *
 * Mode 4:
 *   The fourth and final mode adds debugging support to Mode 3.
 *   The flag that must be passed to enable this mode is '-d'
 *   This mode's run command looks like:
 *     './commandline PartitionName -s -d'
 *
 * @param  argc The argument count (Not passed by user)
 * @param  argv The argument values (Passed by user)
 * @return      An integer always equal to 0
 */
//[===============================================================================================]
int main(int argc, char** argv)
{
  signal(SIGABRT,bad_clean);
  //signal(SIGTERM,clean);
  signal(SIGINT,clean);
  //signal(SIGQUIT,clean);
  /***********************************************************************************************/


  std::string args;
  for(unsigned int i = 0; i < argc; i++)
  {
    args += argv[i];
    args += " ";
  }
  Debug.log(("\nC: Command Line Process Arguments: " + args),true);
  std::string flag;
  if(argc == 3) flag = argv[2];
  if(argc == 4) flag = argv[3];
  /***********************************************************************************************/


  if(argc == 2)
  {
    Debug.log("C: Command Line Initiated",true);
    Debug.log("C: Reading From Stdin",true);
    try
    {
      CLI cli(argv);
    }
    catch(arboreal_cli_error e)
    {
      std::string s1 = e.where();
      std::string s2 = e.what();
      Debug.log("C: " + s1 + " " + s2,true);
      std::cerr << e.where() <<std::endl;
      std::cerr << e.what() << std::endl;;
      exit(1);
    }
  }
  else if(argc == 3 && (flag == "-s" || flag == "-sp"))
  {
    Debug.log("C: Command Line Initiated",true);
    Debug.log("C: Reading From Fstream",true);
    try
    {
      CLI cli(argv,argv[2]);
    }
    catch(arboreal_cli_error e)
    {
      std::string s1 = e.where();
      std::string s2 = e.what();
      Debug.log("C: " + s1 + " " + s2,true);
      std::cerr << e.where() <<std::endl;
      std::cerr << e.what() << std::endl;;
      exit(1);
    }
  }
  else if(argc == 3 && flag == "-d")
  {
    Debug.ON();
    Debug.log("C: Command Line Initiated");
    Debug.log("C: Reading From Stdin");
    Debug.log("C: Debugging Turned On");
    try
    {
      CLI cli(argv,true);
    }
    catch(arboreal_cli_error e)
    {
      std::string s1 = e.where();
      std::string s2 = e.what();
      Debug.log("C: " + s1 + " " + s2,true);
      std::cerr << e.where() <<std::endl;
      std::cerr << e.what() << std::endl;;
      exit(1);
    }
  }
  else if(argc == 4 && flag == "-d")
  {
    Debug.ON();
    Debug.log("C: Command Line Initiated");
    Debug.log("C: Reading From Fstream");
    Debug.log("C: Debugging Turned On");
    try
    {
      CLI cli(argv,argv[2],true);
    }
    catch(arboreal_cli_error e)
    {
      std::string s1 = e.where();
      std::string s2 = e.what();
      Debug.log("C: " + s1 + " " + s2,true);
      std::cerr << e.where() <<std::endl;
      std::cerr << e.what() << std::endl;;
      exit(1);
    }
  }
  else
  {
    Debug.log("Invalid Arg Count - \
               Must Define A Partition On Which To Initialize The Command Line Interface",true);
    perror("Invalid Arg Count - \
            Must Define A Partition On Which To Initialize The Command Line Interface\n");
  }
  /***********************************************************************************************/

  Debug.log("C: Command Line Interface Quit Successfully; Goodbye",true);
  printf("Command Line Interface Quit Successfully; Goodbye\n\n");
  return 0;
}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//[===============================================================================================]
//                                CLI Class Methods Definitions
//[===============================================================================================]
/*!
 * Constructor for use in Mode 1 of the Command Line Interface
 * Reads from explicit user input
 * Does NOT print debug data to log
 */
//[==============================================================================================]
CLI::CLI(char** partition)
{
  _my_partition = partition[1];
  _my_pid = std::to_string(getpid());
  _client_sockpath = "cli-" + _my_pid + "-socket";
  _server_sockpath = "lson-" + _my_pid + "-socket";

  start();
}
//[===============================================================================================]
/*!
 * Constructor for use in Mode 2 of the Command Line Interface
 * Reads from explicit user input
 * Does PRINTS DEBUG data to log
 */
//[================================================================================================]
CLI::CLI(char** partition, bool debug)
{
  _my_partition = partition[1];
  _my_pid = std::to_string(getpid());
  _client_sockpath = "cli-" + _my_pid + "-socket";
  _server_sockpath = "lson-" + _my_pid + "-socket";

  Debug.log(_my_partition);
  Debug.log(_my_pid);
  Debug.log(_client_sockpath);
  Debug.log(_server_sockpath);
  Debug.log("C: Running Start()...");
  start();
}
//[===============================================================================================]
/*!
 * Constructor for use in Mode 3 of the Command Line Interface
 * Reads from file
 * Does NOT print debug data to log
 */
//[===============================================================================================]
CLI::CLI(char** partition, char* isScript)
{
  _my_partition = partition[1];
  std::string script = isScript;
  _my_pid = std::to_string(getpid());
  _client_sockpath = "cli-" + _my_pid + "-socket";
  _server_sockpath = "lson-" + _my_pid + "-socket";
  if(script == "-s")
  {
    _is_script = true;
    _script_print = false;
  }
  else if(script == "-sp")
  {
    _is_script = true;
    _script_print = true;
  }

  start();
}
//[===============================================================================================]
/*!
 * Constructor for use in Mode 3 of the Command Line Interface
 * Reads from file
 * Does PRINTS DEBUG data to log
 */
//[===============================================================================================]
CLI::CLI(char** partition, char* isScript, bool debug)
{
  _my_partition = partition[1];
  std::string script = isScript;
  _my_pid = std::to_string(getpid());
  _client_sockpath = "cli-" + _my_pid + "-socket";
  _server_sockpath = "lson-" + _my_pid + "-socket";

  if(script == "-s")
  {
    _is_script = true;
    _script_print = false;
  }
  else if(script == "-sp")
  {
    _is_script = true;
    _script_print = true;
  }

  Debug.log(_my_partition);
  Debug.log(_my_pid);
  Debug.log(_client_sockpath);
  Debug.log(_server_sockpath);
  Debug.log("C: Running Start()...");
  start();
}
//[================================================================================================]
//[================================================================================================]




/*!
 * Default Destructor
 */
CLI::~CLI(){}
//[================================================================================================]
//[================================================================================================]






//[===============================================================================================]
/*!
 * Run initial Command Line Interface setup operations:
 *
 * 1) Generate Shared Memory Segment For Process Synchronization
 * 2) Fork And Run A Liaison Process
 * 3) Create Sockets For Connection To Liaison
 * 4) Send Handshake Command To File System
 * 5) Run The Command Line
 *
 */
//[===============================================================================================]
void CLI::start()
{

  Debug.log("C: Beginning Initial Setup");
  key_t shm_key;
  int shm_id;
  shm_key = atoi(_my_pid.c_str());
  Debug.log(("C: Shared Memory Segment Key: " + std::to_string((int)shm_key)));
  /***********************************************************************************************/


  /* Create char** to send to Liason via main() argv param */
  Debug.log("C: Generating Liaison Process Arguments");
  std::vector<char*> argv;
  argv.push_back(const_cast<char*>(std::to_string((int)shm_key).c_str()));
  argv.push_back(const_cast<char*>(_client_sockpath.c_str()));
  argv.push_back(const_cast<char*>(_server_sockpath.c_str()));
  argv.push_back(const_cast<char*>(_my_partition.c_str()));
  argv.push_back(const_cast<char*>(_cwd.c_str()));
  argv.push_back(NULL); /* argv MUST be null terminated */
  /***********************************************************************************************/


  Debug.log("C: Creating A New Shared Memory Segment");
  char* shm = create_shm_seg(shm_key,shm_id);
  shm[0] = 0;
  Debug.log(("C: Memory Segment Of Size 1 Created With Initial Value: " + std::to_string(shm[0])));
  /***********************************************************************************************/


  Debug.log("C: Attempting To Fork A Liaison Process");
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
    Debug.log("C: Fork Successful; Returned To Parent Process");
    Debug.log("C: Waiting On Liaison Process To Finish Setup...");
    while(shm[0] == 0);
    /**********************************************************************************************/


    Debug.log("C: Liaison Process Completed Setup");
    Debug.log("C: Setting Up [Command Line --> Liaison] Socket");
    memset(&_server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&_client_sockaddr, 0, sizeof(struct sockaddr_un));
    _client_sock = set_up_socket(_client_sockpath,_client_sockaddr);
    /**********************************************************************************************/


    Debug.log("C: Signal Liaison: [Command Line --> Liaison] Socket Setup Completed");
    shm[0] = 2;
    /**********************************************************************************************/


    Debug.log("C: Shared Memory No Longer Needed Processes Synchronized; Deleting Segment");
    delete_shm(shm_id,shm);
    /**********************************************************************************************/


    Debug.log("C: Attempt To Connect To Liaison Process Server Socket");
    socklen_t length = sizeof(_client_sockaddr);
    connect_to_server(_client_sock,_client_sockpath,_server_sockpath,_server_sockaddr,length);
    /**********************************************************************************************/


    Debug.log("C: Building Handshake Message");
    char* cmnd = new char[MaxBufferSize];
    int cmnd_id = HANDSHK;
    memset(cmnd,'\0',MaxBufferSize); /* Get rid of junk data in buffer */
    memcpy(cmnd,&cmnd_id,sizeof(int));
    memcpy(cmnd + sizeof(int), "HANDSHAKE-", sizeof("HANDSHAKE-"));
    memcpy(cmnd + (sizeof(int) + sizeof("HANDSHAKE-")) - 1,
           const_cast<char*>(_my_partition.c_str()),
           _my_partition.length());
    /**********************************************************************************************/


    std::string send_to = (std::to_string(_client_sock) + "@" + _client_sockpath);
    Debug.log(("C: Sending Handshake Message To: " + send_to));
    send_to_server(_client_sock,_client_sockpath,cmnd,MaxBufferSize,Flag);
    /**********************************************************************************************/


    Debug.log("C: Attempting To Receive Response From Liaison Process");
    char* data = receive_from_server(_client_sock,_client_sockpath,MaxBufferSize,Flag);
    Debug.log("C: Data Received");
    /*********************************************************************************************/


    if(get_cmnd_id(data) == QUIT)
    {
      Debug.log("C: Error While Connecting To Filesystem; Exiting...");
      perror("C: Error While Connecting To Filesystem; Exiting...");

      if(close(_client_sock) < 0)
      {
        std::string where = "[cli.cpp::start()]: ";
        std::string what = "Client Socket Close Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
      }
      if(unlink(_client_sockpath.c_str()) < 0)
      {
        std::string where = "[cli.cpp::start()]: ";
        std::string what = "Client Socket Unlink Failed -- ";
        what += strerror(errno);
        throw arboreal_cli_error(what,where);
      }
      /********************************************************************************************/


      Debug.log("C: Waiting For Child Process To Complete...");
      int status;
      waitpid(pid,&status,0);
      Debug.log("C: Exiting Command Line Interface");

      delete[] cmnd;
      delete[] data;
      exit(1);
    }
    /**********************************************************************************************/


    std::string dta = data;
    _max_string_size = get_cmnd_id(data);
    Debug.log(("C: Maximum Filename/Tagname Size: " + std::to_string(_max_string_size)));

    Debug.log("C: Removing Potential Memory Leaks");
    delete[] cmnd;
    delete[] data;

    Debug.log("C: Setup Completed Successfully");
    Debug.log("C: Running Command Line");
    run();

    Debug.log("C: User Initiated Exit Procedure...");
    if(close(_client_sock) < 0)
    {
      std::string where = "[cli.cpp::start()]: ";
      std::string what = "Client Socket Close Failed -- ";
      what += strerror(errno);
      throw arboreal_cli_error(what,where);
    }
    if(unlink(_client_sockpath.c_str()) < 0)
    {
      std::string where = "[cli.cpp::start()]: ";
      std::string what = "Client Socket Unlink Failed -- ";
      what += strerror(errno);
      throw arboreal_cli_error(what,where);
    }

    Debug.log("C: Waiting For Child Process To Complete...");
    int status;
    waitpid(pid,&status,0);
    Debug.log("C: Child Process Completed Successfully");
    Debug.log("C: Command Line Exiting; Goodbye");
    /**********************************************************************************************/
  }

  return;
}
//[================================================================================================]
//[================================================================================================]





//[===============================================================================================]
/*!
 * Reads input from user and sends it to the Liaison Process.
 * Waits for corresponding data from the File System.
 */
//[===============================================================================================]
void CLI::run()
{

  Debug.log("C: Beginning To Gather User Input");
  std::string input;

  /* Reprint prompt after bad command (but only after a bad command) */
  bool print_prompt = false;

  /* Print the welcome header */
  if(!_is_script){print_header();}

  /* Operations will differ slightly if reading commands from a text file */
  if(_is_script)
  {
    std::cout << "Reading from input file...\n";
    std::cout << "+---------------------------------------\n\n\n";
  }
  /***********************************************************************************************/


  Debug.log("C: Read User Input...");
  while(true)
  {
    if(print_prompt)
    {
      if(!_is_script) std::cout << "Arboreal >> ";
      print_prompt = false;
    }
    /*********************************************************************************************/
    /* Check for ENTER pressed (prints out a new line with 'Arboreal >> ') */
    char c = std::cin.get();
    if(c == '\n')
    {
      /* Don't print prompt if reading from file */
      if(!_is_script){std::cout << "Arboreal >> ";}
      continue;
    }
    /*********************************************************************************************/
    else
    {
      /*
       * Since we read one chrachter in to make sure we did not press ENTER,
       * Need to put that charcter back or else commands will be short by 1 charachter
       */
      std::cin.putback(c);
      /* Get the command */
      getline(std::cin,input);
      Debug.log(("C: Command Received: " + input));
      /*******************************************************************************************/
      if(input == "quit" || input == "q" || input == "Q")
      {
        Debug.log("C: User Initiated Program Exit");
        std::cout << "Are you sure you would like to quit? (Y/N)\n";
        std::cin >> input;
        if(input == "Y" || input == "y")
        {

          Debug.log("C: Notifying Liaison Process Of Program Termination");
          char* quit = new char[MaxBufferSize];
          int val = QUIT;
          memset(quit,'\0',MaxBufferSize);
          memcpy(quit,&val,sizeof(int));
          memcpy(quit + sizeof(int), "QUIT", sizeof("QUIT"));
          send_cmnd(quit);
          return;
        }
        else{continue;}
      }
      /*******************************************************************************************/
      else if((input == "help" || input == "h") && !_is_script )
      {
        help();
        continue;
      }
      /*******************************************************************************************/
      else if(input[1] == 'h' && !_is_script)
      {
        /* Single Command Usage Help */
        switch(check_help(input))
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
          default:{std::cout << "Invalid Command" << std::endl;}
        }
        std::cout << "Arboreal >> ";
        continue;
      }
      /*******************************************************************************************/
      else if(input == "end" && _is_script)
      {
        Debug.log("C: Notifying Liaison Process Of Program Termination");
        char* quit = new char[MaxBufferSize];
        int val = QUIT;
        memset(quit,'\0',MaxBufferSize);
        memcpy(quit,&val,sizeof(int));
        memcpy(quit + sizeof(int), "QUIT", sizeof("QUIT"));
        send_cmnd(quit);
        return;
      }
      /*******************************************************************************************/
      else
      {
        Debug.log("C: Validating Input...");
        int rtrn = check_command(input);

        if(rtrn != 0)
        {
          Debug.log("C: Input Is Valid");
          if(rtrn == CD_ABS || rtrn == CD_RLP)
          {
            Debug.log("C: User Requested Directory Change");
            std::string old = _cwd;
            std::string temp = input.substr(3,input.length());
            _cwd = temp;

            char* new_cwd = new char[MaxBufferSize];
            memset(new_cwd,'\0',MaxBufferSize);
            memcpy(new_cwd,&rtrn,sizeof(int));
            memcpy(new_cwd + sizeof(int),_cwd.c_str(),_cwd.length());

            send_cmnd(new_cwd);
            await_response();
            if(!_is_script){std::cout << "\nArboreal >> ";}
            continue;
          }
        }
        else
        {
          std::cerr << "Comand Not Valid\n";
          if(!_is_script){std::cout << "Arboreal >> ";}
          continue;
        }

        send_cmnd(build(rtrn,input));
        await_response();
        if(!_is_script){std::cout << "Arboreal >> ";}
      }
      /*******************************************************************************************/
      //if(_is_script) sleep(1);
    }
  }
  return;
}
//[================================================================================================]
//[================================================================================================]



//[================================================================================================]
/*!
 * Send user input (A filesystem command) to the Liaison Process
 * @param cmnd The input to send
 */
//[================================================================================================]
void CLI::send_cmnd(const char* cmnd)
{
  std::string t1 = cmnd;
  Debug.log(("C: Sending " + t1 + "To " + _client_sockpath +
             " @ Socket ID: " + std::to_string(_client_sock)));
  send_to_server(_client_sock,_client_sockpath,cmnd,MaxBufferSize,Flag);
  return;
}
//[================================================================================================]
//[================================================================================================]




//[===============================================================================================]
/*! \brief Block while waiting for response from filesystem
 *
 * Most filesystem commands operate on a 1:1 ratio, that is,
 * sending one command will generate one response.  However,
 * some commands (most notably 'find' & 'read') may have a ratio
 * of 1:Many (For example 'find -t [tag1]' may return any number of files
 * but it is only a single command).  In situations such as these it is necssary
 * to tell the Command Line to wait until the filesystem has sent all data.
 * Thus, if the Command Line receives "WAIT" it will know to continue to block on
 * a call to receive until the Liaison has gathered all of the nescessary data.
 * However this is still not enough and it is also nescessary to tell the
 * Command Line how much data it must read, for this reason, the first piece of
 * data that the Liaison will send, will be the number of bytes the Command Line
 * needs to read.  After this value is received the actual data is sent.
 */
//[===============================================================================================]
void CLI::await_response()
{

  Debug.log("C: Receiving Data From File System...");
  char* temp2 = receive_from_server(_client_sock, _client_sockpath, MaxBufferSize, Flag);

  std::string temp3 = temp2;
  Debug.log("C: Received: \n" + temp3);
  delete temp2;
  return;
}
//[================================================================================================]
//[================================================================================================]




//[===============================================================================================]
/*!
 * Format user input for use by Liaison process:
 *
 * 1) Prepend a byte representation of the command ID to the array
 * 2) Copy the user input into the the array (skip the first X indecies
 *    were X is the size of an integer (we don't want to overwrite the command ID))
 *
 * @param  id    Comand ID
 * @param  input User input string
 * @return       A pointer to a charachter array
 */
//[===============================================================================================]
char* CLI::build(const int id, const std::string input)
{
  char* command = new char[MaxBufferSize];

  /* Zero out the command buffer */
  memset(command,'\0',MaxBufferSize);

  /* Add the Command ID to the command string and convert to char[] */
  memcpy(command,&id,sizeof(int));
  memcpy(command + sizeof(int),const_cast<char*>(input.c_str()),input.length());
  return command;
}
//[================================================================================================]
//                                         [END Cli.cpp]
//[================================================================================================]
