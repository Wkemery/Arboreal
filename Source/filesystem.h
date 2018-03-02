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
private:
  FileInfo* _file;
  size_t _seek;
  char _mode;
  bool _EOF;
  PartitionManager* _myPartitionManager;
  
  BlkNumType levelInc(size_t RelativeBlock, BlkNumType LedgerBlock, short level);
public:  
  FileOpen(FileInfo* file, char mode, PartitionManager* pm);
  FileInfo* getFile();
  size_t getSeek();
  char getMode();
  void incrementSeek(size_t bytes, bool write = false);
  void decrementSeek(size_t bytes);
  
  Index byteToIndex(short offset);
    /* will return the index value of the seek pointer plus the offset. If the offset forces it to go past the end of 
     * the current block(i.e the allocated space for file data), it will return an Index with a blknum of 0 as an "error",
     * Also for now, do not call with an offset other than 1 or 0.*/
  Index incrementIndex();
    /* Will always allocate a new block for the next bit of data. Only call when 2 conditions are satisfied:
     * 1. Seek pointer is pointing to very last byte in file.
     * 2. You are at the end of a block. so seek pointer is divisible by 512
     * This will not change the status of EOF
     */
  void setEOF();
  void resetSeek();
  bool getEOF();
  void gotoPastLastByte();
  void refresh();
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
  
  vector<FileInfo*>* tagSearch(unordered_set<string>& tags);
  vector<FileInfo*>* fileSearch(string name);
  void createTag(string tagName);
  void deleteTag(string tagName);
  void mergeTags(string tag1, string tag2);
  void tagFile(FileInfo* file, unordered_set<string> tagsToAdd);
  void untagFile(FileInfo* file, unordered_set<string> tagsToRemove, bool deleting = false);
  void tagFile(vector<string>& filePath, unordered_set<string> tags);
  void untagFile(vector<string>& filePath, unordered_set<string> tags);
  
  void renameTag(string originalTagName, string newTagName);
  FileInfo* createFile(string filename, unordered_set<string>& tags);
  void deleteFile(FileInfo* file);
  void deleteFile(vector<string>& filePath);
  void writeChanges();
  
  void renameFile(vector<string>& originalFilePath, string newFileName);
  int openFile(vector<string>& filePath, char mode);
  void closeFile(unsigned int fileDesc);
  size_t readFile(unsigned int fileDesc, char* data, size_t len);
  size_t writeFile(unsigned int fileDesc, const char* data, size_t len);
  size_t appendFile(unsigned int fileDesc, const char* data, size_t len);
  void seekFileAbsolute(unsigned int fileDesc, size_t offset);
  void seekFileRelative(unsigned int fileDesc, long int offset);
  Attributes* getAttributes(vector<string>& filePath);
  void setPermissions(vector<string>& filePath, char* perms);

  
  
  FileInfo* pathToFile(vector<string>& path);
  
  /* IPC Related */

  int getFileNameSize();
  
  /*DEBUG functions*/
  void printRoot();
  void printTags();
  void printFiles();
  
  
  
private:
  void insertModification(TreeObject* object);
  
};


#endif
