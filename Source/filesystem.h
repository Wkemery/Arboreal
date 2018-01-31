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
  void renameTag(string originalTagName, string newTagName);
  FileInfo* createFile(string filename, vector<string>& tags);
  void deleteFile(FileInfo* file);
  void writeChanges();
  
  void renameFile(vector<string>& originalFilePath, string newFileName);
  int openFile(vector<string>& filePath);
  void closeFile(int fileDesc);
  int readFile(int fileDesc, char* data, int len);
  int writeFile(int fileDesc, const char* data, int len);
  int appendFile(int fileDesc, char* data, int len);
  void seekFileAbsolute(int fileDesc, long unsigned int offset);
  void seekFileRelative(int fileDesc, long unsigned int offset);
  Attributes* getAttributes(vector<string>& filePath);
  void setAttributes(vector<string>& filePath, Attributes* atts);

  /* IPC Related */

  int getFileNameSize();
  
  /*DEBUG functions*/
  void printRoot();
  void printTags();
  void printFiles();
  
private:
  FileInfo* pathToFile(vector<string>& path);
  void insertModification(TreeObject* object);
  
};


#endif
