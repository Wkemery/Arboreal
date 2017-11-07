/*filesystem.h
 * Arboreal
 * October, 12, 2017
 */

#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
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
  DiskManager *myDM;
  PartitionManager *myPM;
  char myfileSystemName;
  int myfileSystemSize;
  
  unordered_map<int, int> lockIdT;
  unsigned int numLocks = rand();
  int open;
  vector<FileOpen> FOT;   // File open table vector
  
/******************************************************************************/
  unordered_map<string, TagTree*> _RootTree;
/******************************************************************************/

  void writeRoot();
  void readRoot();
/******************************************************************************/

  int findEmpty(int& blknum, char* name, int nameLen, char type);
  /* Parameters: 
   *    blknum - acts as return value
   *    name - path and filename or just path, in the case of directories
   *    nameLen - name length
   *    type - 'f' for files 'd' for directories
   * Description:
   *    findEmpty will parse name following the directory structure. In the correct directory inode, it will search 
   *    for an empty spot in the inode to store the file/dir mapping. if the inode is full, it will extend the blocks 
   *    for the directory inode. It assumes a valid filename. This is designed to work for creating directories too.
   * Return Value:
   *    returns the index value of an open spot in the directory inode 
   *    -1 if file already exists
   *    -2 is unable to allocate disk space for a new block for the directory inode
   *    -4 if something else goes wrong
   *    blknum will be set to the block number of the directory inode corresponding to the index return value
   */
  
  int findIndex(int& blknum, char* name, int nameLen, char type);
  /* Parameters:
   *    blknum - acts as a return value
   *    name - filename or directory name WITHOUT path, E.G /a/b/c/d should be just /d
   *    nameLen - name length should always be 2
   *    type - 'f' for files 'd' for directories
   * Description:
   *    In the correct directory inode, it will search for the the file/dir mapping in the inode containing name. It 
   *    assumes a valid filename. This is designed to work for finding directories and files. Assumes valid name. It 
   *    Will not follow directory path. pathSearch will do that instead.
   * Return Value:
   *    the index value of the file/dir mapping in the directory inode
   *    -1 if file/directory does not exist
   *    -2 if file/directory name exists but is wrong type
   *    -4 if something else goes wrong
   *    blknum will be set to the block number of the directory inode corresponding to the index return value
   */
  
  int dirCleanUp(char* name, int nameLen, int blknum);
  /* Parameters:
   *    name - filename or directory name WITHOUT path
   *    nameLen - length of name, should always be 2
   *    blknum - the block number returned by findIndex, possibly an extension of the parent directory inode.
   * Description:
   *    dirCleanUp will check blknum to see if it is empty. If so, it will remove blknum as an extension from the 
   *    directory inode, keeping all the extention structure intact. If blknum is the first block of a directory 
   *    inode, it will do nothing. Assumes valid name.
   * Return Value:
   *    -4 if something goes wrong
   *    0 successful
   */
  
  int newFileDataBlock(int finodeBlknum);
  /* Parameters:
   *    finodeBlknum - the block number of the file inode that you wish to allocate space to
   * Description:
   *    newFileDataBlock will allocate a new block for file data and map that block into the correct spot in the 
   *    finode.
   * Return Value:
   *    -2 is unable to allocate disk space for a new block for the file data
   *    -4 if something goes wrong
   *    0 successful
   */
  
  int pathSearch(char* path, int parentDinodeBlknum);
  /* Parameters:
   *    path - the file path with filename/directory name
   *    parentDinodeBlknum - the dinode block number of the first directory containing the path
   * Description:
   *    pathSearch  will parse path following the directory structure. 
   *    pathSearch is a recursive function. Call it for the first time with the full path and the root directory inode
   *    block number. pathSearch assumes that in the first call, its given the correct dinode block number and the first 
   *    directory in path exists
   * Return Value:
   *    the block number of the final dinode in the path. E.G. /a/b/c/d will return the dinode block number of 
   *    directory /a/b/c since /d is assumed to be a file.  
   *    -4 if something went wrong
   *    -1 if a directory along the path does not exist
   */
  
  int findPos(int seek, int& blknum, int finodeblknum);
public:
  FileSystem(DiskManager *dm, char fileSystemName);
  ~FileSystem();
  
  vector<FileInfo*>* tagSearch(vector<string> tags);
  FileInfo* fileSearch(string name);
  void createTag(string tagName);
  void deleteTag(string tagName);
  void mergeTags(string tag1, string tag2);
  void tagFile(FileInfo* file, vector<string> tags);
  void untagFile(FileInfo* file, vector<string> tags);
  
  
  
  int createFile(char *filename, int fnameLen);
  int deleteFile(char *filename, int fnameLen);
  int openFile(char *filename, int fnameLen, char mode, int lockId);
  int closeFile(int fileDesc);
  int readFile(int fileDesc, char *data, int len);
  int writeFile(int fileDesc, const char *data, int len);
  int appendFile(int fileDesc, char *data, int len);
  int seekFile(int fileDesc, int offset, int flag);
  int renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2);
  int getAttributes(char *filename, int fnameLen, char* buffer, int flag);
  int setAttributes(char *filename, int fnameLen, char* buffer, int flag);
  
  
};


#endif
