///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileSystem.h
//  FileSystem Header File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////


/*FileSystem.h
 * Arboreal
 * October, 12, 2017
 */

#include "Disk.h"
#include "../types.h"
#include "DiskManager.h"
#include "PartitionManager.h"
#include "../types.h"
#include "../Arboreal_Exceptions.h"

#include "Trees.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <string>

using std::unordered_map;
using std::string;
using std::vector;
using std::map;

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
  void increment_seek(size_t bytes, bool write = false);
  void decrement_seek(size_t bytes);
  
  Index byte_to_index(short offset);
    /* will return the index value of the seek pointer plus the offset. If the offset forces it to go past the end of 
     * the current block(i.e the allocated space for file data), it will return an Index with a blknum of 0 as an "error",
     * Also for now, do not call with an offset other than 1 or 0.*/
  Index increment_index();
    /* Will always allocate a new block for the next bit of data. Only call when 2 conditions are satisfied:
     * 1. Seek pointer is pointing to very last byte in file.
     * 2. You are at the end of a block. so seek pointer is divisible by 512
     * This will not change the status of EOF
     */
  void set_EOF();
  void reset_seek();
  bool get_EOF();
  void go_past_last_byte();
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
  vector<TreeObject*> objsToDelete;
/******************************************************************************/

public:
  FileSystem(DiskManager *dm, string fileSystemName);
  ~FileSystem();
  
  vector<FileInfo*>* tagSearch(unordered_set<string>& tags);
  vector<FileInfo*>* fileSearch(string name);
  void create_tag(string tagName);
  void delete_tag(string tagName);
  void merge_tags(string tag1, string tag2);
  void tag_file(FileInfo* file, unordered_set<string> tagsToAdd);
  void untag_file(FileInfo* file, unordered_set<string> tagsToRemove, bool deleting = false);
  void tag_file(vector<string>& filePath, unordered_set<string> tags);
  void untag_file(vector<string>& filePath, unordered_set<string> tags);
  
  void rename_tag(string originalTagName, string newTagName);
  FileInfo* create_file(string filename, unordered_set<string>& tags);
  void delete_file(FileInfo* file);
  void delete_file(vector<string>& filePath);
  void write_changes();
  
  void rename_file(vector<string>& originalFilePath, string newFileName);
  int open_file(vector<string>& filePath, char mode);
  void close_file(unsigned int fileDesc);
  size_t read_file(unsigned int fileDesc, char* data, size_t len);
  size_t write_file(unsigned int fileDesc, const char* data, size_t len);
  size_t append_file(unsigned int fileDesc, const char* data, size_t len);
  void seek_file_absolute(unsigned int fileDesc, size_t offset);
  void seek_file_relative(unsigned int fileDesc, long int offset);
  Attributes* get_attributes(vector<string>& filePath);
  void set_permissions(vector<string>& filePath, char* perms);

  
  FileInfo* path_to_file(vector<string>& path);
  
  /* IPC Related */

  int get_file_name_size();
  
  /*DEBUG functions*/
  void print_root();
  void print_tags();
  void print_files();
  
  
  
private:
  void insert_modification(TreeObject* object);
  
};


#endif
