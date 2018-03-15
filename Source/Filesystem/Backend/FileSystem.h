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
  /*!
   * @param dm the Disk manager for the disk that this Filesystem will be accessing
   * @param partitionName the name of the partition that this FileSystem will be associated with
   */
  FileSystem(DiskManager *dm, string partitionName);
  ~FileSystem();
  
  /*!
   * Since the FileSystem is journaling. The changes to tag trees and the Root tree are only written out
   * when this is called. File Operations are not journaled.
   */
  void write_changes();
  
  /*!
   * Will find a FileInfo object if it exists, given the full path
   * @param path The full path to the file. The filename must be the last entry in the vector. an file name with no 
   * path is considered to be in the default path
   * @returns the found FileInfo object
   */
  FileInfo* path_to_file(vector<string>& path);
  
  /*!
   * @returns the Maximum file name size for the partition associated with this FileSystem object
   */
  int get_file_name_size();
  
  
  /** @name Tag Operations 
   */
  ///@{
  
  
  /*!
   * Search for files by tags. The tag search is an "and" operation, meaning the files returned will have at least all 
   * the specified tags. 
   * @param tags the tags that the files will be tagged with in the return vector
   * @returns a pointer to a vector of the FileInfo objects which then can be serialized. The returned vector should
   * be freed by the calling code
   */
  vector<FileInfo*>* tag_search(unordered_set<string>& tags);
  
  /*!
   * Will create a new tag if that tag name does not already exist
   * @param tagName the name of the Tag to create
   */
  void create_tag(string tagName);
  
  /*!
   * Will delete the specified tag only if it has no files associated with it(it is empty) and it does in fact exist.
   * @param tagName the name of the tag to be deleted
   */
  void delete_tag(string tagName);
  
  /*!
   *TODO: description and Function
   * @param tag1
   * @param tag 2
   */
  void merge_tags(string tag1, string tag2);
  
  /*!
   * Will tag a file with the specified tags. If some or all of the tags do not exist, a warning is printed and the 
   * operation continues. The file must exist. The file that 
   * would be created by adding tags must not already exist.
   * @param file the FileInfo* that will be tagged with the specified tags
   * @param tagsToAdd the tags that will be added to the file's tag set
   * @sa tag_file(vector<string>&, unordered_set<string>)
   */
  void tag_file(FileInfo* file, unordered_set<string> tagsToAdd);
  
  /*!
   * An alternate way to tag a file using a file path instead.Will tag a file with the specified tags. If some or all 
   * of the tags do not exist, a warning is printed and the operation continues. The file must exist.  The file that 
   * would be created by adding tags must not already exist.
   * @param filePath the FileInfo* that will be tagged with the specified tags
   * @param tagsToAdd the tags that will be added to the file's tag set
   * @sa tag_file(FileInfo*, unordered_set<string>)
   */
  void tag_file(vector<string>& filePath, unordered_set<string> tags);
  
  /*!
   * Will remove tags associated with the specified file. The tags must exist. The file must exist. The file that 
   * would be created by removing tags must not already exist.
   * @param file the FileInfo* that will be untagged with the specified tags
   * @param tagsToRemove the tags that will be removed from the file's tag set
   * @param deleting this is a tag only used by the FileSystem itself for deleting a file
   * @sa tag_file(FileInfo*, unordered_set<string>)
   */
  void untag_file(FileInfo* file, unordered_set<string> tagsToRemove, bool deleting = false);
  
  /*!
   * Will remove tags associated with the specified file. The tags must exist. The file must exist. The file that 
   * would be created by removing tags must not already exist.
   * @param file the FileInfo* that will be untagged with the specified tags
   * @param tagsToRemove the tags that will be removed from the file's tag set
   * @param deleting this is a tag only used by the FileSystem itself for deleting a file
   * @sa tag_file(FileInfo*, unordered_set<string>)
   */
  void untag_file(vector<string>& filePath, unordered_set<string> tags);
  
  /*!
   *Will rename the tag. The tag must exist. The new tag name must already exist.
   * This is a slow operation.
   * @param originalTagName the name of the tag to be renamed
   * @param newTagName the new tag name for that tag
   */
  void rename_tag(string originalTagName, string newTagName);
  
  ///@}
  
  
  /** @name File Operations 
   */
  ///@{
  
  /*!
   * Will search for a specified file name. Searches the entire FileSystem
   * @param name the name of the file to search for.
   * @returns a pointer to a vector of FileInfo objects that have the specified name. This should be freed by the 
   * calling code
   */
  vector<FileInfo*>* file_search(string name);
  
  /*!
   * Will create a new file with the specified name and tags. The new file must not already exist.
   * @param filename the name of the new file
   * @param tags the tag set to tag the file with. If empty, will be tagged with default.
   * @returns a FileInfo to the created file, in case the calling code needs it
   */
  FileInfo* create_file(string filename, unordered_set<string>& tags);
  
  
  /*!
   * Will open a file. The file must exist. There is a cap on the Maximum number of open files. You can open the same
   * file as many times as you want.
   * @param filePath the full path including the file name as the last entry
   * @param mode the mode to open the file with. r, w, or x. x is read and write ability.
   * @returns a file descriptor that can later be used to reference the opened file
   */
  int open_file(vector<string>& filePath, char mode);
  
  /*!
   * Will close a file. the File must have been opened. 
   * @param fileDesc the file descriptor returned from open_file
   */
  void close_file(unsigned int fileDesc);
  
  /*!
   * Will read a number of bytes from an open file. The file must have been opened with read permissions. If you read
   * past the end of the file, EOF will be tripped and you cannot continue reading. will return all the data up to that
   * point
   * @param fileDesc the file descriptor returned from open_file
   * @param data a buffer to store the read data must be at least len size
   * @param len the number of bytes to read.
   */
  size_t read_file(unsigned int fileDesc, char* data, size_t len);
  
  /*!
   * Will write a number of bytes to an open file. The file must have been opened with write permissions. You can write 
   * past the EOF with no problems.
   * @param fileDesc the file descriptor returned from open_file
   * @param data a buffer to be read from to write to the file. must be at least of len size
   * @param len the number of bytes to write from data.
   */
  size_t write_file(unsigned int fileDesc, const char* data, size_t len);
  
  /*!
   * Will Append a number of bytes to an open file. The file must have been opened with write permissions. 
   * @param fileDesc the file descriptor returned from open_file
   * @param data a buffer to be read from to write to the file. must be at least of len size
   * @param len the number of bytes to write from data.
   */
  size_t append_file(unsigned int fileDesc, const char* data, size_t len);
  
  /*!
   * Seek to an absolute position in the file. Will trip EOF if the offset is larger than the file size. The posistion in
   * the file is indexed at 1.
   * @param fileDesc the file descriptor returned from open_file
   * @param offset the absolute position in the file to seek to.
   */
  void seek_file_absolute(unsigned int fileDesc, size_t offset);
  
  /*!
   * Seek to a relative position in the file. Will trip EOF if you try to seek too far in a direction. The posistion in
   * the file is indexed at 1.
   * @param fileDesc the file descriptor returned from open_file
   * @param offset the relative position in the file to seek to. may be a negative number.
   */
  void seek_file_relative(unsigned int fileDesc, long int offset);
  
  /*!
   * Will rename a file. The new file must not already exist in the emulated directory
   * @param originalFilePath the full path to the file to be renamed
   * @param newFileName the name that the file will be renamed to.
   */
  void rename_file(vector<string>& originalFilePath, string newFileName);
  
  /*!
   * Will search for a file and return its Attributes
   * @param filePath the full path to the file to you wish to get the Attributes of
   * @returns the Attributes object associated with a particular file.
   */
  Attributes* get_attributes(vector<string>& filePath);
  
  /*!
   * Set the permissions for a file. The format is defined in FileInfo.
   * @param filePath the full path to the file to you wish to get the Attributes of
   * @param perms the permissions following the correct format to set to this file
   * @sa FileInfo::set_permissions()
   */
  void set_permissions(vector<string>& filePath, char* perms);
  
  /*!
   *Delete a particular file. The file must exist.
   * @param file the FileInfo object to be deleted.
   * @sa delete_file(vector<string>&)
   */
  void delete_file(FileInfo* file);
  
  /*!
   * Delete a particular file. The file must exist.
   * @param filePath the full path to the file to you wish to delete
   * @sa delete_file(FileInfo*)
   */
  void delete_file(vector<string>& filePath);
  
  ///@}
  
  
  /** @name Debug Functions 
   */
  ///@{ 
  /*!
   * Print out the root Tree
   */
  void print_root();
  /*!
   * Print out all the tag trees and their contents
   */
  void print_tags();
  /*!
   * Print out all files and their blocknumbers
   */
  void print_files();
  
  ///@}
  
private:
  void insert_modification(TreeObject* object);
  
};


#endif
