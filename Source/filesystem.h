/*filesystem.h
 * Arboreal
 * October, 12, 2017
 */

#include "disk.h"
#include "types.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "types.h"
#include "Arboreal_Exceptions.h"

#include "Trees.h"
#include <vector>
#include <unordered_map>
#include<map>
#include<string>

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class FileOpen{
public:
  int finodeblk;
  int seek;
  char mode;
  
  FileOpen(int fblk = 0, int sk = 0, int md = 0) : finodeblk(fblk), seek(sk), mode(md) {}
  FileOpen(const FileOpen& rhs)
  {
    finodeblk = rhs.finodeblk;
    seek = rhs.seek;
    mode = rhs.mode;
  }
};

bool operator==(const FileOpen& lhs, const FileOpen& rhs);


class FileSystem {
  PartitionManager *_myPartitionManager;
  string _FSName;
  
  
  unordered_map<int, int> lockIdT;
  unsigned int numLocks = rand();
  int open;
  vector<FileOpen> FOT;   // File open table vector
  
/******************************************************************************/
  RootTree* _RootTree;
  map<TreeObject*, int> _modifiedObjects;
  void insertModification(TreeObject* object);
  unordered_multimap<string, FileInfo*> _allFiles;

/******************************************************************************/

public:
  FileSystem(DiskManager *dm, string fileSystemName);
  ~FileSystem();
  
  vector<FileInfo*>* tagSearch(vector<string> tags);
  vector<FileInfo*>* fileSearch(string name);
  void createTag(string tagName);
  void deleteTag(string tagName);
  void mergeTags(string tag1, string tag2);
  void tagFile(FileInfo* file, vector<string>& tags);
  void untagFile(FileInfo* file, vector<string>& tags);
  FileInfo* createFile(string filename, vector<string>& tags);
  void deleteFile(FileInfo* file);
  void writeChanges();
  
  
  int openFile(char *filename, int fnameLen, char mode, int lockId);
  int closeFile(int fileDesc);
  int readFile(int fileDesc, char *data, int len);
  int writeFile(int fileDesc, const char *data, int len);
  int appendFile(int fileDesc, char *data, int len);
  int seekFile(int fileDesc, int offset, int flag);
  int renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2);
  int getAttributes(char *filename, int fnameLen, char* buffer, int flag);
  int setAttributes(char *filename, int fnameLen, char* buffer, int flag);

  /* IPC Related */

  int getFileNameSize();
  void listen();
  
  /*DEBUG functions*/
  void printRoot();
  void printTags();
  void printFiles();
  
};


#endif
