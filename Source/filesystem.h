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
  FileInfo* _file;
  long unsigned int _seek;
  char _mode;
  
  FileOpen(FileInfo* file, char mode);
  FileInfo* getFile();
  long unsigned int getSeek();
  char getMode();
  void incrementSeek(long unsigned int bytes);
  Index byteToIndex(PartitionManager* pm);
  void setEOF();
};

class FileSystem {
  PartitionManager *_myPartitionManager;
  string _FSName;
  
  vector<FileOpen*> _fileOpenTable;   // File open table vector
  
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
  int openFile(vector<string>& filePath, char mode);
  void closeFile(unsigned int fileDesc);
  long unsigned int readFile(unsigned int fileDesc, char* data, long unsigned int len);
  long unsigned int writeFile(unsigned int fileDesc, const char* data, long unsigned int len);
  long unsigned int appendFile(unsigned int fileDesc, const char* data, long unsigned int len);
  void seekFileAbsolute(unsigned int fileDesc, long unsigned int offset);
  void seekFileRelative(unsigned int fileDesc, long unsigned int offset);
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
