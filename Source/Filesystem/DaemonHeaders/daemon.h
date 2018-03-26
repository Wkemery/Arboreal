///////////////////////////////////////////////////////////////////////////////////////////////////
//
// daemon.h
//
// Defines functions and values used by daemon.cpp
//
// Author:    Adrian Barberis
// For:       Arboreal Project
// Dated:     March | 3rd | 2018
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "../../SharedHeaders/Parser.h"
#include "../../SharedHeaders/DebugMessages.hpp"
/*************************************************************************************************/


static const int BACKLOG = 10;              /* Number of Connection Requests that the Server Can Queue */
static const int FLAG = 0;                  /* Flag for recv() */
static const int TIMEOUT = 10;              /* How Long Retries Should Take */
static const int TRUE = 1;                  /* Integer Boolean True */
static const int FALSE = 0;                 /* Integer Boolean False */
static const int PORT = 70777;              /* File System Port Number */
static const int MAX_COMMAND_SIZE = 4096;   /* Maximum Buffer Size */
static const int WRITE_CHANGES_WAIT = 1;    /* How Long To Wait Before Writing Changes */
static const bool WILL_TIME = false;
DebugMessages Debug;                       /* For Debugging */

fd_set master_set;                         /* Used for call to select() holds file descriptors */
int my_fid = 999;                          /* File system socket ID */
int max_fid = 0;                           /* Used by call to select() max_fid == 0 is FS socket */
int current_command_id = 0;                /* The Command Being Operated On's ID */
bool quit_signaled = false;
bool quit_writing = false;
bool verbose = false;
std::vector<std::string> data;

std::map<int, FileSystem*> fd_fs_map;                   /* Maps a file descriptor (socket) to a Partition */
std::map<std::string,FileSystem*> part_fs_map;          /* Maps a partition name to and FS object */
std::map<std::string, unsigned int> path_filedesc_map;  /* Maps a pathname to a file descriptor (socket) */

Disk* d = 0;               /* Disk Object */
DiskManager* dm = 0;       /* Disk Manager */
/*************************************************************************************************/
#define CREATEFILEDATA "Data/create_file_time.txt"
#define CREATETAGDATA "Data/create_tag_time.txt"
#define TAGSEARCHDATA "Data/tag_search_time.txt"
#define FILESEARCHDATA "Data/file_search_time.txt"
#define TAGFILEDATA "Data/tag_file_time.txt"
#define RENAMETAGDATA "Data/rename_tag_time.txt"


//[================================================================================================]
/*!
 * Catch either a user generated or system generated termination signal
 *
 * @param sig The generated signals ID, passed to the function by the call
 *            to signal(), DO NOT supply this yourself, it is supplied
 *            automatically by the system.
 *
 * @return VOID
 */
//[================================================================================================]
void sig_caught(int sig)
{
  quit_writing = true;
  printf("\nD: [Fatal Error] Daemon Received Signal - %s\n", strsignal(sig));
  for(auto it = begin(fd_fs_map); it != end(fd_fs_map); ++it)
  {
    it->second->write_changes();
  }
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
  }
  exit(-1);
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Quit the Daemon;
 * Delete data properly and signal
 * other processes that need to be aware of the quit
 *
 * This function is run by a thread that is detatched from the main process
 *
 * @return VOID
 */
//[================================================================================================]
// void quit_fs(void)
// {
//   std::string input;
//   quit_writing = true;
//   quit_signaled = true;
//   while(true)
//   {
//     std::cin >> input;
//     if(input == "Q" || input == "q" || input == "quit") break;
//   }
//   for(auto it = begin(fd_fs_map); it != end(fd_fs_map); ++it)
//   {
//     it->second->write_changes();
//   }
//   for (int i=0; i <= max_fid; ++i)
//   {
//     if (FD_ISSET(i, &master_set)) close(i);
//   }
//   for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
//   {
//     delete it->second;
//   }
//   return;
// }
//[================================================================================================]
//[================================================================================================]
/*!
 * Periodically write all changes to disk
 * Interval in between writes can be adjusted by changing the value of WRITE_CHANGES_WAIT
 *
 * This function is run by a thread that is detatched from the main process
 *
 * @return VOID
 */
void save_to_disk(void)
{
  while(!quit_writing)
  {
    sleep(WRITE_CHANGES_WAIT);
    std::cout << "Wrote Changes" <<std::endl;
    for(auto it = begin(fd_fs_map); it != end(fd_fs_map); ++it)
    {
      it->second->write_changes();
    }
  }
  std::cout << "QUIT SUCCESS" << std::endl;
  return;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Convert a command line interface command buffer into a string
 * Used only for debugging puposes
 *
 * @param  cmnd The command to be converted
 * @param  size The size of the command buffer
 *
 * @return      A std::string of the data within the buffer minus the first X bytes where
 *              X is the size of an integer
 */
//[================================================================================================]
std::string command_to_string(char* cmnd, int size)
{
  std::string s;
  int index = 0;

  while(index < size){s += cmnd[index]; index += 1;}
  return s;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Create the daemon socket
 * If socket creation fails, keep trying until you hit TIMEOUT
 *
 * @param  timeout Length of time in seconds which the function
 *                 should attempt to create socket in the case of failure
 *
 * @return         An integer, socket ID
 */
//[================================================================================================]
int create_sock(int timeout)
{
  int daemon_sock = 0;
  int timer = 0;
  int printer = 0;
  while(((daemon_sock = socket(AF_INET,SOCK_STREAM,0)) <= 0) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Create Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    std::string where = "[ConnectionDaemon.cpp::create_sock()]: ";
    std::string what = "Socket Creation Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(what,where);
  }
  return daemon_sock;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Set socket options, this mainly allows the same socket address to be reused by the program
 * when it starts up again.  Normally socket addresses are one time use, this causes issues
 * if you would like to quit the FS and then begin it again, so we must force a reuse
 *
 * @param daemon_sock Daemon socket ID
 * @param sock_opt    Used by setsockopt() see man pages
 * @param timeout     Time in seconds the function should retry for if seet options fails
 *
 * @return VOID
 */
//[================================================================================================]
void set_socket_opt(int daemon_sock, int sock_opt, int timeout)
{
  int timer = 0;
  int printer = 0;
  while((setsockopt(daemon_sock,SOL_SOCKET,SO_REUSEADDR,&sock_opt,sizeof(sock_opt))) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Set Socket Options Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::set_socket_opt()]: ";
    std::string what = "Set Socket Options Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Set socket to nonblocking mode in order to have continuous data streams
 * this will also set any connecting sockets to nonblocking
 *
 * @param daemon_sock Daemon socket ID
 * @param is_on       Wether nonblocking mode should be turned on or off (1 == ON | 0 == OFF)
 *
 * @return VOID
 */
//[================================================================================================]
void set_nonblocking(int daemon_sock, int is_on)
{
  int rval = ioctl(daemon_sock,FIONBIO,(char*)&is_on);
  if(rval < 0)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::set_nonblocking()]: ";
    std::string what = "Set Non-Blocking Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Bind socket to Port number
 *
 * @param daemon_sock     Daemon socket ID
 * @param daemon_sockaddr Daemon socket address
 * @param timeout         Retry time length
 *
 * @return VOID
 */
//[================================================================================================]
void bind_socket(int daemon_sock, struct sockaddr_in daemon_sockaddr, int timeout)
{
  int timer = 0;
  int printer = 0;
  int rval = 0;
  while((rval = bind(daemon_sock, (struct sockaddr*)&daemon_sockaddr,sizeof(daemon_sockaddr))) < 0 && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying...", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Bind To Port# %d Failed - %s - Retrying...", PORT, strerror(errno));
      fflush(stdout);
    }
    else if(printer >= 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::bind_socket()]: ";
    std::string what = "Bind Socket Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Mark socket as open for receiving connections
 *
 * @param daemon_sock Daemon socket ID
 * @param backlog     Number of connections that listen can queue up
 * @param timeout     Retry time length
 *
 * @return VOID
 */
//[================================================================================================]
void listen_on_socket(int daemon_sock,int backlog,int timeout)
{
  int timer = 0;
  int printer = 0;
  while((listen(daemon_sock,backlog) < 0) && timer < timeout)
  {
    sleep(1);
    timer += 1;
    if(printer % 3 == 0)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying.", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 1)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying..", strerror(errno));
      fflush(stdout);
    }
    else if(printer % 3 == 2)
    {
      printf("\33[2K\r");
      printf("\rD: Socket Listen Failed - %s - Retrying...", strerror(errno));
      fflush(stdout);
    }
    else if(printer > 120) printer = 0;
    printer += 1;
  }
  if(timer == timeout)
  {
    close(daemon_sock);
    std::string where = "[ConnectionDaemon.cpp::listen_on_socket()]: ";
    std::string what = "Listen On Socket Failed -- ";
    what += strerror(errno);
    throw arboreal_daemon_error(where,what);
  }
  return;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Convert the first X characters in a 'Command Buffer' to an integer value
 * X is the size of an integer
 *
 * @param cmnd : The command buffer
 */
//[================================================================================================]
int get_cmnd_id(char* cmnd)
{
    char temp[sizeof(int)];
    for(unsigned int i = 0; i < sizeof(int); i++)
    {
        temp[i] = cmnd[i];
    }

    int* id = (int*)temp;
    return *id;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Get the partition a Command Line would like to connect to as a std::string rather than char*
 *
 * @param  cmnd Command Line command buffer SPECIFICALLY, the one sent by start() in order
 *              to initiate the handshake process
 *
 * @return      The partition name as a std::string
 */
//[================================================================================================]
std::string get_partition(char* cmnd)
{
  std::string temp = "";
  int index = (int)sizeof(int);
  while(cmnd[index] != '-'){index += 1;}
  index += 1;
  while(cmnd[index] != '\0')
  {
    temp += cmnd[index];
    index += 1;
  }
  return temp;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Returns true if a buffer sent by the Liaison process is a number or not
 * Used to check when the Liaison has issued a new command rather than just
 * more data for the previous command
 * The buffer must first be converted into a string
 * This function will only work with strings sent by the Liaison AFTER having completed
 * a handshake, that is it is only valid for string constructed using the Parser and should NOT
 * contain byte representations of numbers
 *
 * @param  str A string litteral
 *
 * @return     TRUE if the string is a number | FALSE otherwise
 */
//[================================================================================================]
bool is_number(const char* str)
{
  char* ptr;
  strtol(str, &ptr, 10);
  return *ptr == '\0';
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Pad a std::string with a certain character to a certain length
 * Pads from the back only
 *
 * @param  string String to be padded
 * @param  size   Number of characters to append
 * @param  value  Which charachter to pad the string with
 *
 * @return        The padded string
 */
//[================================================================================================]
std::string pad_string(std::string string, int size, char value)
{
    std::string padded = string;
  for(unsigned int i = 0; i < size; i++)
  {
    padded += value;
  }
  return padded;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Return a set representation of the data within a buffer sent by the Liaison process
 * This is most commonly used in order to b reak down a string such as a path
 * into its constituent parts using a charcter delimeter.  For example,
 * sending /tag1/tag2/tag3 to this function will return an unordered set containing
 * [tag1,tag2,tag3]
 *
 * @param command The command that needs to be split into parts
 * @param delim   The charchter that will be used as the delimeter marking where the function
 *                needs to split the command
 *
 * @return An unordered set of the command contents minus the delimiting charachters
 */
//[================================================================================================]
std::unordered_set<std::string> get_set(char* command, char delim)
{
  std::string exec = command;
  std::vector<std::string> temp = Parser::split_on_delim(exec,delim);
  std::unordered_set<std::string> tags;

  for(unsigned int i = 0; i < temp.size(); i++)
  {
    if(temp[i] != ""){tags.emplace(temp[i]);}
  }
  return tags;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Overloaded version of get_set() which takes as its parameter a vector
 * This function does not require a delimiter instead it just pushes the items from
 * the vector into an unordered_set
 *
 * @param vec The vector that needs to be converted into an unordered_set
 *
 * @return A std::unordered_set containing the vector's contents
 *
 */
//[================================================================================================]
std::unordered_set<std::string> get_set(std::vector<std::string> vec)
{
    std::unordered_set<std::string> set;
    for(unsigned int i = 0; i < vec.size(); i++)
    {
        set.emplace(vec[i]);
    }
    return set;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Returns a string containing some of a Files attributes
 *
 * @param  file A pointer to a File object containing the file's attributes
 *
 * @return      A std::string containing some of the file's attributes
 */
//[================================================================================================]
std::string get_file_info(File* file)
{
  std::string file_info = ("[ " + file->get_name() + " | ");
  std::vector<std::string> tags = file->get_tags();
  for(unsigned int i = 0; i < tags.size(); i++)
  {
    if(file_info.length() + 5 == MAX_COMMAND_SIZE)
    {
      file_info += " ...]";
      return file_info;
    }
    if(i + 1 != tags.size()){file_info += (tags[i] + ",");}
    else{file_info += tags[i];}
  }

  //file_info += " |...]";

  if(file_info.length() < MAX_COMMAND_SIZE)
  {
    file_info += " | ";

    FileAttributes attr = file->get_attributes();

    std::tm * ptm = std::localtime(&attr.creationTime);
    char buffer[32];
    memset(buffer,'\0',32);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

    file_info += "Created @ ";
    file_info += buffer;

    file_info += " | ";

    memset(buffer,'\0',32);
    ptm = std::localtime(&attr.lastEdit);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    file_info += "Last Edit @ ";
    file_info += buffer;

    file_info += " ...]";
  }

  return file_info;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Overloaded version of get_file_info() which takes as a parameter a pointer to a
 * FileInfo object rather than a File object
 *
 * @param  file A pointer to a FileInfo object containing the file's attributes
 * @return      A std::string containing some of the file's attributes
 */
//[================================================================================================]
std::string get_file_info(FileInfo* file)
{
  std::string file_info = ("[ " + file->get_name() + " | ");
  std::vector<std::string> tags = file->get_vec_tags();
  for(unsigned int i = 0; i < tags.size(); i++)
  {
    if(file_info.length() + 5 == MAX_COMMAND_SIZE)
    {
      file_info += " ...]";
      return file_info;
    }
    if(i + 1 != tags.size()){file_info += (tags[i] + ",");}
    else{file_info += tags[i];}
  }

  //file_info += " |...]";

  if(file_info.length() < MAX_COMMAND_SIZE)
  {
    file_info += " | ";

    FileAttributes attr = file->get_file_attributes();

    std::tm * ptm = std::localtime(&attr.creationTime);
    char buffer[32];
    memset(buffer,'\0',32);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

    file_info += "Created @ ";
    file_info += buffer;

    file_info += " | ";

    memset(buffer,'\0',32);
    ptm = std::localtime(&attr.lastEdit);
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    file_info += "Last Edit @ ";
    file_info += buffer;

    file_info += " ...]";
  }

  return file_info;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Get A shortened version of the file information
 * The shortened file info conatains the file name,
 * the first X tags were X = num_tags and the creation timestamp
 * The number os tags is less than the value for num_tags, the actual number of tags
 * will be used instead
 *
 * @param  file     The file who's info we want
 * @param  num_tags Number of tags to display
 *
 * @return          A std::string containing the file information
 *
 */
//[================================================================================================]
std::string get_short_file_info(FileInfo* file, int num_tags)
{
  std::string file_info = ("[ " + file->get_name() + " | ");
  std::vector<std::string> tags = file->get_vec_tags();

  if(tags.size() > num_tags)
  {
    for(unsigned int i = 0; i < num_tags; i++){file_info += (tags[i] + ",");}
    file_info += "... | ";
  }
  else
  {
    for(unsigned int i = 0; i < tags.size(); i++)
    {
      if(i + 1 >= tags.size()){file_info += tags[i];}
      else{file_info += (tags[i] + ",");}
    }
    file_info += " | ";
  }

  FileAttributes attr = file->get_file_attributes();

  std::tm * ptm = std::localtime(&attr.creationTime);
  char buffer[32];
  memset(buffer,'\0',32);
  // Format: Mo, 15.06.2009 20:20:00
  std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

  file_info += "Created @ ";
  file_info += buffer;

  file_info += "]";

  return file_info;

}
//[================================================================================================]
//[================================================================================================]
/*!
 * Uses get_file_info() to return a vector of file info strings
 * The File System functions which return file attributes, can return as many
 * file attributes as there are files, typically this means that a vector of FileInfo pointers
 * is returned, this function converts all of those FileInfo pointers into
 * strings containing the respective file information
 *
 * @param fileinfo A std::vector of FileInfo pointers
 *
 * @return A std::vector of std::string's returned from get_file_info()
 */
//[================================================================================================]
std::vector<std::string> serialize_fileinfo(std::vector<FileInfo*>* fileinfo)
{
  std::vector<std::string> data;
  if(fileinfo != 0)
  {
    for(unsigned int i = 0; i < fileinfo->size(); i++)
    {
      if(fileinfo->at(i) != 0)
      {
        /* Uncommenting this line can cause Segfaults */
        /* This is because the FileInfo pointers are lost after deserialization */
        /* Future versions may keep the deserialization and attempt to eliminate this problem */
        //File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(fileinfo->at(i))->c_str()));
        if(verbose){ data.push_back(get_file_info(fileinfo->at(i))); }
        else {data .push_back(get_short_file_info(fileinfo->at(i), 3)); }
      }
      else{continue;}
    }
  }
  else
  {
    throw arboreal_daemon_error("Bad Vector Pointer","[ConnectionDaemon.cpp::execute()]");
    return data;
  }

  return data;
}
//[================================================================================================]
//[================================================================================================]
/*!
 * Execute the proper File System action based on a command id and command data
 * Apply those actions to the correct FS object by using the fd_fs_map and the
 * file descriptor passed
 *
 * @param id      The command to be executed's ID
 * @param command The command to be executed's data
 * @param fd      The file descriptor that requested this command,
 *                the resulting data will be passed back to it and the changes
 *                will occure on the FS object that it is tied to
 *
 * @return A std::vector of std::string's comprising the data returned by the command
 *         execution, this could be anything from an error mesage, to a success message,
 *         to a bunch of file information
 */
//[================================================================================================]
void execute(int id, char* command, int fd, std::vector<std::string>& data)
{
  switch(id)
  {
    case(FIND_TS): // find tag
    {
      std::unordered_set<std::string> tags = get_set(command,',');
      std::vector<FileInfo*>* rval;
      try
      {
        /*Timing Code*/
        if(WILL_TIME) {
          std::ofstream outfile;
          outfile.open(TAGSEARCHDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          rval = fd_fs_map[fd]->tag_search(tags);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{rval = fd_fs_map[fd]->tag_search(tags);}
        /*Timing Code*/

        std::vector<std::string> temp = serialize_fileinfo(rval);
        if(temp.size() == 0)
        {
          if(tags.size() == 1)
          {
            data.push_back("Tag [" + *begin(tags) +"] Exists But Contains No Files");
          }
          else
          {
            std::string temp;
            for(auto it = begin(tags); it != end(tags); ++it)
            {
              if(std::next(it,1) == end(tags)){temp += *it;}
              else{temp += (*it + ",");}
            }
            data.push_back("Tags [" + temp + "] Exist But Contain No Files");
          }
        }
        else
        {
          data.push_back("Search Found [" + std::to_string(rval->size()) + "] Files");
          for(unsigned int i = 0; i < temp.size(); i++){data.push_back(temp[i]);}
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }

      delete rval;
      break;
    }
    case(FIND_FS): //find file
    {
      std::string file = command;
      std::vector<FileInfo*>* rval;
      try
      {
        /*Timing Code*/
        if(WILL_TIME){
          std::ofstream outfile;
          outfile.open(FILESEARCHDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          rval = fd_fs_map[fd]->file_search(file);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{rval = fd_fs_map[fd]->file_search(file);}
        /*Timing Code*/

        std::vector<std::string> temp = serialize_fileinfo(rval);

        data.push_back("Search Found [" + std::to_string(rval->size()) + "] Files");
        for(unsigned int i = 0; i < temp.size(); i++){data.push_back(temp[i]);}

      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }

      delete rval;
      break;
    }
    case(NEW_TS): // Create Tag
    {
      std::string tag = command;
      try
      {
        /*Timing Code*/
        if(WILL_TIME){
          std::ofstream outfile;
          outfile.open(CREATETAGDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          fd_fs_map[fd]->create_tag(tag);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{fd_fs_map[fd]->create_tag(tag);}
        /*Timing Code*/


        std::string success = "New Tag [";
        success += (tag + "] Created");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(NEW_FS): //create file in CWD
    {
      std::unordered_set<std::string> tags;
      std::string filename;
      char tag[MAX_COMMAND_SIZE];
      memset(tag,'\0',MAX_COMMAND_SIZE);
      int index = 0;
      while(command[index] != '-'){filename += command[index]; index += 1;}
      //printf("Filename: %s\n",filename.c_str());
      memcpy(tag,(command + filename.length() + 1), MAX_COMMAND_SIZE - (filename.length() + 1));
      tags = get_set(tag,'-');

      try
      {
        /*Timing Code*/
        FileInfo* finfo = 0;
        if(WILL_TIME){
          std::ofstream outfile;
          outfile.open(CREATEFILEDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          finfo = fd_fs_map[fd]->create_file(filename,tags);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{finfo = fd_fs_map[fd]->create_file(filename,tags);}
        /*Timing Code*/

        if(finfo != 0)
        {
//           File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(finfo));
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(NEW_FP): // create file anywhere
    {
      std::string temp = command;
      std::unordered_set<std::string> tags;
      std::string filename;

      std::vector<std::string> path = Parser::split_on_delim(command,'/');
      filename = path[path.size() - 1];
      path.erase(end(path) - 1);

      tags = get_set(path);

      try
      {
        FileInfo* finfo = 0;
        /*Timing Code*/
        if(WILL_TIME){
          std::ofstream outfile;
          outfile.open(CREATEFILEDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          finfo =  fd_fs_map[fd]->create_file(filename,tags);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{finfo =  fd_fs_map[fd]->create_file(filename,tags);}
        /*Timing Code*/

        if(finfo != 0)
        {
//           File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(finfo));
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(DEL_TS): // delete tag(s)
    {
      std::string tagname = command;

      try
      {
        fd_fs_map[fd]->delete_tag(tagname);
        std::string success = ("Tag [" + tagname + "] Successfully Deleted; Current Working Directory Changed To [ / ]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(DEL_FS): // delete files from CWD
    {
      std::string to_delete = command;
      std::vector<std::string> path = Parser::split_on_delim(to_delete,'-');

      try
      {
        fd_fs_map[fd]->delete_file(path);
        std::string success = ("File [" + path[path.size()-1] +"] Successfully Deleted");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(DEL_FP): // delete file from anywhere
    {
      std::string filepath = command;
      std::vector<std::string> path = Parser::split_on_delim(filepath,'/');
      for(uint i = 0; i < path.size(); i++)
      {
        std::cout << path[i] << std::endl;
      }
      try
      {
        fd_fs_map[fd]->delete_file(path);
        std::string success = ("File [" + path[path.size()-1] + "] Successfully Deleted; Current Working Directory Changed To [ / ]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(OPEN_FP): // Open file
    {
      std::string temp = command;
      std::vector<std::string> info = Parser::split_on_delim(temp,'/');

      char mode = info[0].c_str()[0];

      std::string path = temp.substr(2,temp.length());

      info.erase(begin(info));

      try
      {
        unsigned int filedesc = 0;
        filedesc = fd_fs_map[fd]->open_file(info,mode);
        auto it = path_filedesc_map.find(path);
        if(filedesc != 0 && it == end(path_filedesc_map))
        {
          path_filedesc_map.insert(std::pair<std::string, unsigned int>(path,filedesc));
        }
        std::string success = ("File [" + info[info.size()-1] + "] Opened Successfuly For ");
        if(mode == 'r'){success += "Read";}
        else if(mode == 'w'){success += "Write";}
        else if(mode == 'x'){success += "Read and Write";}

        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }

      break;
    }
    case(OPEN_F):
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(CLOSE_FP): // close file
    {
      std::string p = command;
      std::vector<std::string> path = Parser::split_on_delim(p,'/');

      try
      {
        fd_fs_map[fd]->close_file(path_filedesc_map[p]);
        std::string success = ("File [" + path[path.size()-1] + "] Closed Successfully");
        path_filedesc_map.erase(p);
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      break;
    }
    case(CLOSE_F): //close file (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(RNAME_TS): // Rename tag
    {
      std::string rename = command;
      std::vector<std::string> names = Parser::split_on_delim(rename,'-');
      for(unsigned int i = 0; i < names.size(); i++)
      {
        std::cout << names[i] << std::endl;
      }
      try
      {
        /*Timing Code*/
        if(WILL_TIME){
          std::ofstream outfile;
          outfile.open(RENAMETAGDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          fd_fs_map[fd]->rename_tag(names[0],names[1]);
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
          outfile.close();
        }
        else{fd_fs_map[fd]->rename_tag(names[0],names[1]);}
        /*Timing Code*/

        std::string success = ("Tag [" + names[0] + "] Successfully Renamed To [" + names[1] + "]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(RNAME_FP): // rename file
    {
      std::string to_split = command;
      std::vector<std::string> split = Parser::split_on_delim(to_split,'/');
      std::string new_name = split[split.size() - 1];
      split.erase(end(split) - 1);
      std::cout << split.size() << std::endl;

      try
      {
        fd_fs_map[fd]->rename_file(split,new_name);
        std::string success = ("File [" + split[split.size() - 1] + "] Successfully Renamed To [" + new_name +"]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      fd_fs_map[fd]->write_changes();
      break;
    }
    case(RNAME_FS): // rename files
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(ATTR_FP): //get file attr
    {
      std::string path = command;
      std::vector<std::string> vpath = Parser::split_on_delim(path,'/');
      try
      {
        Attributes* attr = fd_fs_map[fd]->get_attributes(vpath);
        FileAttributes fattr = attr->get_file_attributes();
        std::string t1 = ("[ Name -- " + vpath[vpath.size() - 1] + "]\n");

        std::tm * ptm = std::localtime(&fattr.creationTime);
        char buffer[32];
        memset(buffer,'\0',32);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        std::string buf = buffer;

        std::string t2 = ("[ Created -- " + buf + "]\n");

        memset(buffer, '\0', 32);
        ptm = std::localtime(&fattr.lastEdit);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        buf = buffer;

        std::string t3 = ("[ Last Edit -- " + buf + "]\n");

        memset(buffer, '\0', 32);
        ptm = std::localtime(&fattr.lastAccess);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        buf = buffer;
        std::string t4 = ("[ Last Accessed -- " + buf + "]\n");

        int size = (int)fattr.size;
        std::string t5 = ("[ Size -- " + std::to_string(size) + "]\n");

        //char perm[12];
        //for(unsigned int i = 0; i < 12; i++){perm[i] = fattr.permissions[i];}
        std::string t6 = ("[ Permissions -- [TBA] ]\n");
        // for(unsigned int i = 0; i < 12; i++)
        // {
        //   if(i + 1 == 12){t6 += perm[i];}
        //   else{t6 += perm[i]; t6 += ",";}
        // }
        // t6 += "]\n";

        std::string t7 = ("[ Owner -- Root ]\n");

        std::string success = t1 + t2 + t3 + t4 + t5 + t6 + t7;
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        break;
      }
      break;
    }
    case(ATTR_FS): // get file attrs
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(MERG_1_1): // merge 1:1
    {
      data.push_back("Building in Progress...");
      break;
    }
    case(MERG_M_1): // merge many:1
    {
      data.push_back("Building in Progress...");
      break;
    }
    case(TAG_FP): // tag single file from anywhere
    {
        std::string temp1 = command;
        int index = 0;
        while(temp1[index] != '>'){index += 1;}
        std::string path = temp1.substr(0,index);
        index += 1;
        std::string tags = temp1.substr(index,temp1.length());

        std::vector<std::string> vpath = Parser::split_on_delim(path,'/');
        std::unordered_set<std::string> stags = get_set(Parser::split_on_delim(tags,'-'));

        try
        {
          /*Timing Code*/
          if(WILL_TIME){
            std::ofstream outfile;
            outfile.open(TAGFILEDATA, std::ofstream::out | std::ofstream::app);
            auto t_start = std::chrono::high_resolution_clock::now();
            fd_fs_map[fd]->tag_file(vpath,stags);
            auto t_end = std::chrono::high_resolution_clock::now();
            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
            outfile.close();
          }
          else{fd_fs_map[fd]->tag_file(vpath,stags);}
          /*Timing Code*/

            std::string success = ("File [" + vpath[vpath.size() - 1] + "] ");
            success += "Successfully Tagged With [";
            for(auto it = begin(stags); it != end(stags); ++it)
            {
                if(std::next(it,1) != end(stags)){success += (*it + ",");}
                else{success += *it;}
            }
            success += "]";
            data.push_back(success);
        }
        catch(arboreal_exception& e)
        {
            data.push_back(e.what());
            break;
        }
        fd_fs_map[fd]->write_changes();
        break;
    }
    case(TAG_FS): // tag files within CWD
    {
        std::string temp1 = command;
        int index = 0;
        while(temp1[index] != '>'){index += 1;}
        std::string path = temp1.substr(0,index);
        index += 1;
        std::string tags = temp1.substr(index,temp1.length());

        std::vector<std::string> vpath = Parser::split_on_delim(path,'/');
        std::unordered_set<std::string> stags = get_set(Parser::split_on_delim(tags,'-'));

        try
        {
          /*Timing Code*/
          if(WILL_TIME){
            std::ofstream outfile;
            outfile.open(TAGFILEDATA, std::ofstream::out | std::ofstream::app);
            auto t_start = std::chrono::high_resolution_clock::now();
            fd_fs_map[fd]->tag_file(vpath,stags);
            auto t_end = std::chrono::high_resolution_clock::now();
            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags() <<endl;
            outfile.close();
          }
          else{fd_fs_map[fd]->tag_file(vpath,stags);}
          /*Timing Code*/

            std::string success = ("File [" + vpath[vpath.size() - 1] + "] ");
            success += "Successfully Tagged With [";
            for(auto it = begin(stags); it != end(stags); ++it)
            {
                if(std::next(it,1) != end(stags)){success += (*it + ",");}
                else{success += *it;}
            }
            success += "]";
            data.push_back(success);
        }
        catch(arboreal_exception& e)
        {
            data.push_back(e.what());
            break;
        }
        fd_fs_map[fd]->write_changes();
        break;
    }
    case(UTAG_FP): // untag file
    {
        std::string temp1 = command;
        int index = 0;
        while(temp1[index] != '>'){index += 1;}
        std::string path = temp1.substr(0,index);
        index += 1;
        std::string tags = temp1.substr(index,temp1.length());

        std::vector<std::string> vpath = Parser::split_on_delim(path,'/');
        std::unordered_set<std::string> stags = get_set(Parser::split_on_delim(tags,'-'));

        try
        {
            fd_fs_map[fd]->untag_file(vpath,stags);
            std::string success = "Tags [";
            for(auto it = begin(stags); it != end(stags); ++it)
            {
                if(std::next(it,1) != end(stags)){success += (*it + ",");}
                else{success += *it;}
            }
            success += ("] Successfully Removed From File [" + vpath[vpath.size() - 1] + "]");
            data.push_back(success);
        }
        catch(arboreal_exception& e)
        {
            data.push_back(e.what());
            break;
        }
        fd_fs_map[fd]->write_changes();
        break;
    }
    case(UTAG_FS): // untag files (cwd)
    {
        std::string temp1 = command;
        int index = 0;
        while(temp1[index] != '>'){index += 1;}
        std::string path = temp1.substr(0,index);
        index += 1;
        std::string tags = temp1.substr(index,temp1.length());

        std::vector<std::string> vpath = Parser::split_on_delim(path,'/');
        std::unordered_set<std::string> stags = get_set(Parser::split_on_delim(tags,'-'));

        try
        {
            fd_fs_map[fd]->untag_file(vpath,stags);
            std::string success = "Tags [";
            for(auto it = begin(stags); it != end(stags); ++it)
            {
                if(std::next(it,1) != end(stags)){success += (*it + ",");}
                else{success += *it;}
            }
            success += ("] Successfully Removed From File [" + vpath[vpath.size() - 1] + "]");
            data.push_back(success);
        }
        catch(arboreal_exception& e)
        {
            data.push_back(e.what());
            break;
        }
        fd_fs_map[fd]->write_changes();
        break;
    }
    case(READ_XP): // read x bytes
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(READ_XCWD): // read x bytes (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(READ_FP): // read all
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(READ_FCWD): // read all (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(WRITE_FP): // write all to file
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(WRITE_FCWD): // write all (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(WRITE_XFPF): // write x from f1 to f2
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(WRITE_XFCWDF): // write x from f1 to f2 (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(CPY_FP): // copy f1 to f2
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    case(CPY_FCWD): // copy f1 to f2 (cwd)
    {
      std::string s = command;
      data.push_back(command);
      break;
    }
    default:
    {
        data.push_back("Unrecognized Command");
        break;
    }
    //case(23) switch directories is handled by the liaison process
  }

}
//[================================================================================================]
//[================================================================================================]
