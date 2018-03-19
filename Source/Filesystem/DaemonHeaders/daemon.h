

#include "../../SharedHeaders/Parser.h"

#define BACKLOG 10                  /* Number of Connection Requests that the Server Can Queue */
#define FLAG 0                      /* Flag for recv() */
#define TIMEOUT 10
#define TRUE 1
#define FALSE 0
#define PORT 70777
#define MAX_COMMAND_SIZE 4096

bool DEBUG = false;

fd_set master_set;
int my_fid = 999;
int max_fid = 0;
int current_command_id = 0;

std::map<int, FileSystem*> fd_fs_map;
std::map<std::string,FileSystem*> part_fs_map;
std::map<std::string, unsigned int> path_filedesc_map;

Disk* d = 0;
DiskManager* dm = 0;


//////////////////////////////////////////////////////////////////////////////////////////////
//---------------- BEGIN FUNCTION DEFINITIONS ------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
///
///
void sig_caught(int sig)
{
  printf("\nD: [Fatal Error] Daemon Received Signal - %s\n", strsignal(sig));
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
//--------------------------------------------------------------------------------------------



void quit_fs(void)
{
  std::string input;
  while(true)
  {
    std::cin >> input;
    if(input == "Q" || input == "q" || input == "quit") break;
  }
  for (int i=0; i <= max_fid; ++i)
  {
    if (FD_ISSET(i, &master_set)) close(i);
  }
  for(auto it = begin(part_fs_map); it != end(part_fs_map); ++it)
  {
    delete it->second;
  }
  return;
}
//--------------------------------------------------------------------------------------------



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
//--------------------------------------------------------------------------------------------



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
//--------------------------------------------------------------------------------------------



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
//--------------------------------------------------------------------------------------------



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
//--------------------------------------------------------------------------------------------



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
//--------------------------------------------------------------------------------------------

//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
//
// @ cmnd : The command buffer
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
//--------------------------------------------------------------------------------------------

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

bool is_number(const char* str)
{
  char* ptr;
  strtol(str, &ptr, 10);
  return *ptr == '\0';
}

std::string pad_string(std::string string, int size, char value)
{
    std::string padded = string;
  for(unsigned int i = 0; i < size; i++)
  {
    padded += value;
  }
  return padded;
}


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

std::unordered_set<std::string> get_set(std::vector<std::string> vec)
{
    std::unordered_set<std::string> set;
    for(unsigned int i = 0; i < vec.size(); i++)
    {
        set.emplace(vec[i]);
    }
    return set;
}

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


std::vector<std::string> serialize_fileinfo(std::vector<FileInfo*>* fileinfo)
{
  std::vector<std::string> data;
  if(fileinfo != 0)
  {
    for(unsigned int i = 0; i < fileinfo->size(); i++)
    {
      if(fileinfo->at(i) != 0)
      {
        File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(fileinfo->at(i))->c_str()));
        data.push_back(get_file_info(info));
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




std::vector<std::string> execute(int id, char* command, int fd)
{
  std::vector<std::string> data;
  switch(id)
  {
    case(FIND_TS): // find tag
    {
      std::unordered_set<std::string> tags = get_set(command,',');
      std::vector<FileInfo*>* rval;
      try
      {
        rval = fd_fs_map[fd]->tag_search(tags);
        data = serialize_fileinfo(rval);
        if(data.size() == 0)
        {
          data.push_back("Tag Exists But Has No Associated Files");
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }

      delete rval;
      return data;
    }
    case(FIND_FS): //find file
    { 
      std::string file = command;
      std::vector<FileInfo*>* rval;
      try
      {
        rval = fd_fs_map[fd]->file_search(file);
        data = serialize_fileinfo(rval);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }

      delete rval;
      return data;
    }
    case(NEW_TS): // Create Tag
    {
      std::string tag = command;
      try
      {
        fd_fs_map[fd]->create_tag(tag);
        std::string success = "New Tag [";
        success += (tag + "] Created");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(NEW_FS): //create file in CWD
    {
      std::unordered_set<std::string> tags;
      std::string filename;
      char tag[MAX_COMMAND_SIZE];
      memset(tag,'\0',MAX_COMMAND_SIZE);
      int index = 0;
      while(command[index] != '-'){filename += command[index]; index += 1;}
      printf("Filename: %s\n",filename.c_str());
      memcpy(tag,(command + filename.length() + 1), MAX_COMMAND_SIZE - (filename.length() + 1));
      tags = get_set(tag,'-');

      try
      {
        FileInfo* finfo = fd_fs_map[fd]->create_file(filename,tags);
        if(finfo != 0)
        {
          File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(info));
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
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
        FileInfo* finfo = fd_fs_map[fd]->create_file(filename,tags);
        if(finfo != 0)
        {
          File* info = File::read_buff(const_cast<char*>(FileInfo::serialize(finfo)->c_str()));
          data.push_back(get_file_info(info));
        }
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
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
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
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
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
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
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
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
        return data;
      }

      return data;
    }
    case(OPEN_F):
    {
      std::string s = command;
      data.push_back(command);
      return data;
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
        return data;
      }
      return data;
    }
    case(CLOSE_F):
    {
      std::string s = command;
      data.push_back(command);
      return data;
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
        fd_fs_map[fd]->rename_tag(names[0],names[1]);
        std::string success = ("Tag [" + names[0] + "] Successfully Renamed To [" + names[1] + "]");
        data.push_back(success);
      }
      catch(arboreal_exception& e)
      {
        data.push_back(e.what());
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(RNAME_FP): // rename file
    {
      std::string to_split = command;
      std::vector<std::string> split = Parser::split_on_delim(to_split,'/');
      std::string new_name = split[split.size() - 1];
      std::cout << new_name << std::endl;
      std::cout << split.size() << std::endl;
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
        return data;
      }
      fd_fs_map[fd]->write_changes();
      return data;
    }
    case(RNAME_FS):
    {
      std::string s = command;
      data.push_back(command);
      return data;
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
        return data;
      }
      return data;
    }
    case(ATTR_FS):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(MERG_1_1): // merge 1:1
    {
      data.push_back("Building in Progress...");
      return data;
    }
    case(MERG_M_1): // merge many:1
    {
      data.push_back("Building in Progress...");
      return data;
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
            fd_fs_map[fd]->tag_file(vpath,stags);
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
            return data;
        }
        fd_fs_map[fd]->write_changes();
        return data;
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
            fd_fs_map[fd]->tag_file(vpath,stags);
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
            return data;
        }
        fd_fs_map[fd]->write_changes();
        return data;
    }
    case(UTAG_FP):
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
            return data;
        }
        fd_fs_map[fd]->write_changes();
        return data;
    }
    case(UTAG_FS):
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
            return data;
        }
        fd_fs_map[fd]->write_changes();
        return data;
    }
    case(READ_XP):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(READ_XCWD):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(READ_FP):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(READ_FCWD):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(WRITE_FP):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(WRITE_FCWD):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(WRITE_XFPF):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(WRITE_XFCWDF):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(CPY_FP):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    case(CPY_FCWD):
    {
      std::string s = command;
      data.push_back(command);
      return data;
    }
    default:
    {
        data.push_back("Unrecognized Command");
        return data;
    }
    //case(23) switch directories is handled by the liaison process
  }

}

