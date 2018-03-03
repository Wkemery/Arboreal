/*filesystem.h
 * Arboreal
 * October, 12, 2017
 */

#include "disk.h"
#include "types.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "Arboreal_Exceptions.h"

#include "Trees.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <string>

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class FileOpen{
public:
  FileInfo* _file;
  long unsigned int _seek;
  char _mode;
  bool _EOF;
  
  FileOpen(FileInfo* file, char mode);
  FileInfo* getFile();
  long unsigned int getSeek();
  char getMode();
  void incrementSeek(long unsigned int bytes);
  Index byteToIndex(PartitionManager* pm);
  void setEOF();
  void resetSeek();
  bool getEOF();
};

class FileSystem {
  PartitionManager *_myPartitionManager;
  std::string _FSName;
  
  std::vector<FileOpen*> _fileOpenTable;   // File open table std::vector
  
/******************************************************************************/
  RootTree* _RootTree;
  std::map<TreeObject*, int> _modifiedObjects;
  std::unordered_multimap<std::string, FileInfo*> _allFiles;

/******************************************************************************/

public:
  FileSystem(DiskManager *dm, std::string fileSystemName);
  ~FileSystem();
  
  std::vector<FileInfo*>* tagSearch(std::vector<std::string> tags);
  std::vector<FileInfo*>* fileSearch(std::string name);
  void createTag(std::string tagName);
  void deleteTag(std::string tagName);
  void mergeTags(std::string tag1, std::string tag2);
  void tagFile(FileInfo* file, std::vector<std::string>& tags);
  void untagFile(FileInfo* file, std::vector<std::string>& tags);
  void renameTag(std::string originalTagName, std::string newTagName);
  FileInfo* createFile(std::string filename, std::vector<std::string>& tags);
  void deleteFile(FileInfo* file);
  void writeChanges();
  
  void renameFile(std::vector<std::string>& originalFilePath, std::string newFileName);
  int openFile(std::vector<std::string>& filePath, char mode);
  void closeFile(unsigned int fileDesc);
  long unsigned int readFile(unsigned int fileDesc, char* data, long unsigned int len);
  long unsigned int writeFile(unsigned int fileDesc, const char* data, long unsigned int len);
  long unsigned int appendFile(unsigned int fileDesc, const char* data, long unsigned int len);
  void seekFileAbsolute(unsigned int fileDesc, long unsigned int offset);
  void seekFileRelative(unsigned int fileDesc, long unsigned int offset);
  Attributes* getAttributes(std::vector<std::string>& filePath);
  void setAttributes(std::vector<std::string>& filePath, Attributes* atts);

  /* IPC Related */

  int getFileNameSize();
  
  /*DEBUG functions*/
  void printRoot();
  void printTags();
  void printFiles();
  
private:
  FileInfo* pathToFile(std::vector<std::string>& path);
  void insertModification(TreeObject* object);
  
};


#endif
