/*filesystem.cpp
 * Arboreal
 * October, 12, 2017
 */

#include "filesystem.h"
#include "Trees.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <unordered_map>
#include<vector>
#include<string>
using namespace std;

bool EncryptionFlag = false;


FileSystem::FileSystem(DiskManager *dm, char fileSystemName)
{
  myfileSystemName = fileSystemName;
  myDM = dm;
  myfileSystemSize = myDM->getPartitionSize(myfileSystemName);
  /* Create a partition manager for my partition */
  myPM = new PartitionManager(myDM, myfileSystemName, myfileSystemSize);
  
  /*Zero out root dir if need be*/
  
  char buffer[myPM->getBlockSize()];
  int ret = myPM->readDiskBlock(1, buffer);
  if(ret == -1) cerr << "Disk could not be opened!" << endl;
  if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(ret != 0) exit(1);
  if(buffer[1] == 'c')
  {
    memset(buffer, 0, myPM->getBlockSize());
    ret = myPM->writeDiskBlock(1, buffer);
    if(ret == -1) cerr << "Disk could not be opened!" << endl;
    if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(ret != 0) exit(1);
  }
}

FileSystem::~FileSystem()
{
  delete myPM;
}

vector<FileInfo*>* FileSystem::tagSearch(vector<string> tags)
{
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  //  - Single tag:
  
  if(tags.size() == 1)
  {
    //   * find the tag in root tree
    auto it = _RootTree.find(tags[0]);
    it->second->getTree();
    
    //   * list files in tag tree pointed to by root tree
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    for(auto it2 = treeptr->begin(); it2 != treeptr->end(); it2++)
    {
      ret->push_back(it2->second);
    }
    
  }
  else
  {
    //   - Multi Tag:
    
    //   * Use size field in root node of tag tree to find smallest tree among the tags you want to search
    //create vector of tagtrees that I want to search
    vector<TagTree*> searchTrees;
    for(unsigned int i = 0; i < tags.size(); i++)
    {
      auto it = _RootTree.find(tags[i]);
      if(it == _RootTree.end())
      {
        cerr << tags[i] + " excluded from search : Tag Does not exist" << endl;
      }
      else
      {
        searchTrees.push_back(it->second);
      }
    }
    
    TagTree* smallest = searchTrees[0];
    for(unsigned int i = 1; i < searchTrees.size(); i++)
    {
      if(searchTrees[i]->getTree()->size() < smallest->getTree()->size())
        smallest = searchTrees[i];
    }
    
    //   * Search the smallest tree:
    unordered_map<string, FileInfo*>* treeptr = smallest->getTree();
    for(auto it = treeptr->begin(); it != treeptr->end(); it++)
    {
      //   # elimnate all nodes with tag count < the number of tags you are searching for
      if(it->second->getTags()->size() >= tags.size())
      {
        //could be a matching file
        bool match = true;
        //   # search remainng files for exact tag match O(nlogn) worst case, average case O(n)
        for(unsigned int i = 0; i < tags.size(); i++)
        {
          if(it->second->getTags()->find(tags[i]) == it->second->getTags()->end())
          {
            match = false;
            break;
          }
        }
        if(match)
        {
          ret->push_back(it->second);
        }
      }
    }
  }

  //   # return (list) the found file(s)
  return ret;
}

FileInfo* FileSystem::fileSearch(string name)
{
  //   - Binary search for file name from largest to smallest tag tree
  // not sure how to do that right now...
  //   - Worst case is (number of trees) * log(n)
    
  for(auto it = _RootTree.begin(); it != _RootTree.end(); it++)
  {
    auto it2 = it->second->getTree()->find(name);
    if(it2 != it->second->getTree()->end())
    {
      //found the file 
      return it2->second;
    }
  }
  //never found the file
  
  return 0;
}

void FileSystem::createTag(string tagName)
{
  
  //   - Make sure tag is unique
  if(_RootTree.find(tagName) != _RootTree.end())
  {
    throw invalid_argument(tagName + " is not unique");
  }
  
  //   - Get a block from disk to store tag tree
  BlkNumType blknum = myPM->getFreeDiskBlock();
  //   - initialize tree in main memory
  TagTree* newTree = new TagTree(blknum);
  //   - add respective node to root tree (write TagTree block num to Node as well as TagTree memory address to Node)
  
  _RootTree.insert(pair<string, TagTree*>(tagName, newTree));
  //   - set up block in disk as empty tag tree, ie write out tag tree
  newTree->writeOut();
  //   - rebalance  Root tree, done automatically with map
  //   - write Root tree to disk
  writeRoot();
  
}

void FileSystem::deleteTag(string tagName, bool force)
{
  //   - find tagTree
  auto it = _RootTree.find(tagName);
  if(it == _RootTree.end())
  {
    throw invalid_argument(tagName + " cannot be deleted : Tag Does not exist");
  }
  
  unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
  
  //   - CANNOT delete tag if tag tree Size > 0
  
  if((treeptr->size() > 0) && (!force))
  {
    throw invalid_argument(tagName + " cannot be deleted: Tag has files associated with it, Consider running with force option");
    //cerr << "Warning! : Tag has files associated with it. This may take a while."
  }
  
  //TODO: consider threading here to keep the programming running while deleting stuff in the background
  if(force)
  {
    for(auto it2 = treeptr->begin(); it2 != treeptr->end(); it2++)
    {
      //   - delete all references to the tag tree from Fileinodes
      it2->second->getTags()->erase(tagName);
      it2->second->writeOut();
    }
  }

  //   - encryption requires no zero-ing of disk
  if(!EncryptionFlag) it->second->zeroDisk();
  //   - remove node from Root tree
  _RootTree.erase(it);
  
  //   - write the root tree out to disk
  writeRoot();
  
}

void FileSystem::mergeTags(string tag1, string tag2)
{
  
//   - create new tag tree if needed
//   - Move all Nodes in largest tag tree to new (assuming new tree was created otherwise add to existsing tree) Tree
//   * delete refrences to old tags in Fionde as you go
//   - Move Nodes of second Tag Tree:
//   * check that node is not already in destination tree
//   * if Yes: Skip
//   * if NO: Add
//   * Repete
  
}

void FileSystem::tagFile(FileInfo* file, vector<string> tags)
{
  //for all tags in the vector
  for(auto t : tags)
  {
    //   - find tagTree
    
    auto it = _RootTree.find(t);
    //   - If tag does not exist create a new Tag tree
    if(it == _RootTree.end())
    {
      createTag(t);
      it = _RootTree.find(t);
    }
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    //   - Add Tag to Finode 
    file->getTags()->insert(pair<string, BlkNumType>(t, it->second->getBlockNum()));
    
    //   - Create and Add new Node to TagTree
    treeptr->insert(pair<string, FileInfo*>(file->getFilename(), file));
    
    //   - Write updated TagTree to disk
    it->second->writeOut();

  }

  // (write updated Finode to disk)
  file->writeOut();
  
}

void FileSystem::untagFile(FileInfo* file, vector<string> tags)
{
  //for all tags in the vector
  for(auto t : tags)
  {
    //   - find tagTree
    
    auto it = _RootTree.find(t);
    //   - If tag does not exist print error and continue
    if(it == _RootTree.end())
    {
      cerr << t + " cannot be removed from " << file->getFilename() << " : Tag Does not exist" << endl;
    }
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    //   - Remove Node from TagTree
    treeptr->erase(file->getFilename());
    
    //   - Delete tag from FileInode
    file->getTags()->erase(t);
    
    //   - Write updated TagTree to disk
    it->second->writeOut();
    
  }
  
  // (write updated Finode to disk)
  file->writeOut();
}


FileInfo* FileSystem::createFile(string filename, vector<string> tags)
{
  //   - Get an open block and intiialize Finode
  BlkNumType blocknum = myPM->getFreeDiskBlock();
  FileInfo* newFile = new FileInfo(filename, blocknum);
  
  //   - If tag not given then add file to "default" tag tree
  //   * File remains in default tag tree until a non-default tag is associated with file
  vector<string> temp;
  temp.push_back("default tag");
  if(tags.size() == 0)
  {
    tagFile(newFile, temp);
  }
  else
  {
    tagFile(newFile, tags);
  }
  
  return newFile;
}

void FileSystem::deleteFile(FileInfo* file)
{
  //   NOTE: File will be referenced by Finode block number(fidentifier)
  //   - check to make sure file exists, you have a FileInfo*, so file must exist
  vector<string> tags;
  unordered_map<string, BlkNumType>* fileTags = file->getTags();
  for(auto it = fileTags->begin(); it != fileTags->end(); it++)
  {
    tags.push_back(it->first);
  }
  //   - dissasociate all tags from the file (Call untagFile())
  untagFile(file, tags);
  
  //   - Free all data blocks
  //   - Free Finode block on disk
  file->del();
  
  delete file;
  file = 0;
  
  
//  
//   int ret;
//   /* Validate filename */
//   
//   /***************************************************************************/
//   
//   /*Follow directory path*/
//   int parentDinodeStart =  pathSearch(filename, 1); //1 to start at root directory
//   if(parentDinodeStart == -1) return -1; //File does not exist
//   if(parentDinodeStart == -4) return -4; //something went wrong
//   
//   char name[3]; // 3 because directory names can only be 1 character
//   memcpy(name, filename + (fnameLen - 2), 2);
//   name[2] = 0;
//   /***************************************************************************/
//   
//   /* Find file mapping index number in correct directory inode block */
//   int dinodeBlknum = parentDinodeStart;
//   int index = findIndex(dinodeBlknum, name, 2, 'f');
//   if(index == -1) return -1; //file does not exist
//   if(index == -4) return -3; //something went wrong
//   if(index == -2) return -3; //name exists but is wrong type
//   /***************************************************************************/
//   
//   /* Read in directory inode */
//   char dinode[myPM->getBlockSize()];
//   ret = myPM->readDiskBlock(dinodeBlknum, dinode);
//   
//   if(ret == -1) cerr << "Disk could not be opened!" << endl;
//   if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
//   if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
//   if(ret != 0) return -3; //something went wrong
//   /***************************************************************************/
//   
//   
//   int finodeBlknum = charToInt(index + 2, dinode);//save finodeBlknum first
//   
//   if(lockIdT.find(finodeBlknum) != lockIdT.end()) return -2;
//   
//   /*Check if file is open*/
//   /* Use STL find to search for an element in the file open table(FOT) matching filename1*/
//   FileOpen element;
//   element.finodeblk = finodeBlknum;
//   vector<FileOpen>::iterator it;
//   it = find(FOT.begin(), FOT.end(), element);
//   if(it != FOT.end()) return -2; //file is open
//   /***************************************************************************/
//   /* Delete file mapping entry. I will just "zero" out the whole file map entry*/
//   
//   memset(dinode + index, 0, 6);
//   
//   /* Write out modified dinode*/
//   ret = myPM->writeDiskBlock(dinodeBlknum, dinode);
//   if(ret == -1) cerr << "Disk could not be opened!" << endl;
//   if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
//   if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
//   if(ret != 0) return -3; //something went wrong
//   
//   /* check if that was the last mapping in an extension , if so free block */
//   ret = dirCleanUp(name, 2, parentDinodeStart);
//   if(ret == -3) return -3; //something went wrong
//   
//   /***************************************************************************/
//   
//   /* Read in finode*/
//   char finode[myPM->getBlockSize()];
//   ret = myPM->readDiskBlock(finodeBlknum, finode);
//   if(ret == -1) cerr << "Disk could not be opened!" << endl;
//   if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
//   if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
//   if(ret != 0) return -3; //something went wrong
//   /***************************************************************************/
//   
//   
//   /* Free finode block*/
//   ret = myPM->returnDiskBlock(finodeBlknum);
//   if(ret != 0) return -3; //something went wrong
//   
//   /* Delete all file data */
//   /* Free all direct address data */
//   int bufferPos = 6;
//   for(int i = 0; i < 3; i++)
//   {
//     if(finode[bufferPos] != 0)
//     {
//       /* Return blocks with data */
//       ret = myPM->returnDiskBlock(charToInt(bufferPos, finode));
//       if(ret != 0) return -3; //something went wrong
//       
//       bufferPos += 4;
//     }
//   }
//   /* If there is an indirect inode, follow it and free all that data*/
//   if(finode[18] != 0) //indirect inode address @ block 18
//   {
//     int indinodeBlknum = charToInt(18, finode);
//     /* Read in Indirect inode */
//     char indinode[myPM->getBlockSize()];
//     ret = myPM->readDiskBlock(indinodeBlknum, indinode);
//     if(ret == -1) cerr << "Disk could not be opened!" << endl;
//     if(ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
//     if(ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
//     if(ret != 0) return -3; //something went wrong
//     
//     /* Free the Indirect Inode */
//     ret = myPM->returnDiskBlock(indinodeBlknum);
//     if(ret != 0) return -3; //something went wrong
//     
//     bufferPos = 0;
//     for(int i = 0; i < 16; i++)
//     {
//       if(indinode[bufferPos] != 0)
//       {
//         /* Return blocks with data */
//         ret = myPM->returnDiskBlock(charToInt(bufferPos, indinode));
//         if(ret != 0) return -3; //something went wrong
//       }
//       else break;
//       bufferPos += 4;
//     }
//   }
//   
//   return 0;
}

int FileSystem::openFile(char *filename, int fnameLen, char mode, int lockId)
{
//   ** Direct File Operations are pretty much the same as OS **
// change to take a fidentifier
  //   - Get File Blknum
//   - Get Tags from Finode
//   - Steal Code from OS to see what else we need to do
  
  
  
  int ret = 0;
  /*Follow directory path*/
  int dinodeBlknum =  pathSearch(filename, 1); //1 to start at root directory
  if(dinodeBlknum == -1) return -1; //File does not exist
  if(dinodeBlknum == -4) return -4; //something went wrong
  
  char name[3]; // 3 because directory names can only be 1 character
  memcpy(name, filename + (fnameLen - 2), 2);
  name[2] = 0;
  /***************************************************************************/
  
  int index = findIndex(dinodeBlknum, name, 2, 'f');
  if(index == -1) return -1; //file does not exist
  if(index == -4) return -4; //something went wrong
  if(index == -2) return -1; //name exists but as wrong type
  char buffer[64];
  myPM->readDiskBlock(dinodeBlknum, buffer);
  
  //Check Mode conditions
  if(mode != 'r' && mode != 'w' && mode != 'm') return (-2); // Invalid mode
  //Locked or Not
  
  /* check lock the same way as up there. using the find function of lockIdT*/
  int check = charToInt(index + 1, buffer);
  
  if(lockIdT.find(check) != lockIdT.end()){
    unordered_map<int, int>::iterator it = lockIdT.find(check);
    if(it->second != lockId)
      return (-3);
  }else{
    if(lockId != (-1))
      return (-3);
  }
  
  // open the file
  open ++; // increment the # of open files
  FileOpen temp(charToInt(index + 1, buffer), 0, mode);
  temp.mode = mode;
  temp.seek = 0;
  
  unsigned int i = 0;
  for(; i < FOT.size(); i++){
    if(FOT[i].finodeblk == 0) break;
  }
  if(i == FOT.size()){
    FOT.push_back(temp);
    ret = i;
  }
  else{
    FOT[i].seek = temp.seek;
    FOT[i].finodeblk = temp.finodeblk;
    FOT[i].mode = temp.mode;
    ret = i;
  }
  
  return (ret + 1); //returns the unique
  
}

int FileSystem::closeFile(int fileDesc)
{
//   - Get File Blknum
//   - Get Tags from Finode
//   - Steal Code from OS to see what else we need to do
  
  
  if((fileDesc-1) > int(FOT.size() - 1))
    return (-1); //invalid file descriptor
    if((fileDesc - 1) < 0) // if the fileDesc is invalid
      return (-1);
    if(FOT[fileDesc - 1].finodeblk == 0) //if the FOT has no record of the file being open
      return (-1);
    
    // Remove the entry from the FOT
    FOT[fileDesc - 1].seek = 0;
  FOT[fileDesc - 1].finodeblk = 0;
  FOT[fileDesc - 1].mode = '\0';
  
  return (0); // return success
  
}

int FileSystem::readFile(int fileDesc, char *data, int len)
{
  fileDesc --;
  if (fileDesc > int(FOT.size() - 1)) return -1; //Invalid file descriptor
  if (FOT[fileDesc].finodeblk == 0) return -1; //No file to read
  if (len < 0) return -2;	//Negative length
  if (FOT[fileDesc].mode == 'w') return -3; //Only open for write
  
  /* Read in finode */
  char finode[myPM->getBlockSize()];
  int rv = myPM->readDiskBlock(FOT[fileDesc].finodeblk, finode);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  int size = charToInt(2, finode);
  // 	cout << "Size of this file is " << size << endl;
  
  if (FOT[fileDesc].seek == size) return 0; //At end of file
  int dataBlknum;
  int index;
  int bytesRead = 0;
  
  /* Read in data block*/
  char fileData[myPM->getBlockSize()];
  
  for (int i = 0; i < len; i++)
  {
    /* Starting at index, read data into buffer  */
    if(i >= size) return bytesRead;
    index = findPos(FOT[fileDesc].seek, dataBlknum, FOT[fileDesc].finodeblk);
    if (index < 0)	return -8; //Index out of bound or block not allocated
    if (index >= size)	return bytesRead;
    int rv = myPM->readDiskBlock(dataBlknum, fileData);
    if (rv == -1) cerr << "Disk could not be opened!" << endl;
    if (rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if (rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
    if (rv != 0) return -4; //something went wrong
    data[i] = fileData[index];
    bytesRead++;
    FOT[fileDesc].seek++;
  }
  
  return bytesRead;
}

int FileSystem::writeFile(int fileDesc, const char *data, int len)
{
  fileDesc--;
  int bytesWritten = 0; //return value for this function
  int rv; //return value for other functions, used for error checking
  
  if(fileDesc < 0) return -1; //Invalid File desciptor
  if(fileDesc > int(FOT.size() - 1)) return -1; //invalid file descriptor
  if(FOT[fileDesc].finodeblk == 0) return -1; //invalid file descriptor
  
  if(len < 0) return -2; //negative length
  
  /* This line assumes that if the file descriptor is valid (finodeblk != 0), the modes are only 'r' or 'w' or 'm' */
  if(FOT[fileDesc].mode == 'r') return -3; //Only open for read
  
  /* Assuming if file descriptor is valid, seek ptr is valid */
  /* Find block number and index pointed to by seek ptr */
  int dataBlknum;
  int index = findPos(FOT[fileDesc].seek, dataBlknum, FOT[fileDesc].finodeblk);
  if(index == -7) //this file is empty
  {
    rv = newFileDataBlock(FOT[fileDesc].finodeblk);
    if((rv == -2) || (rv == -4)) return -3; //cannot complete operation
    index = findPos(FOT[fileDesc].seek, dataBlknum, FOT[fileDesc].finodeblk);
    if(index == -6 || index == -5 || index == -4) return 0;
    else if(index == -7)
    {
      //I done messed up
      cerr << "Fatal error in writeFile()" << endl;
      exit(1);
    }
  }
  else if(index == -6 || index == -5 || index == -4) return -3;
  
  while(bytesWritten != len)
  {
    /* Read in data block*/
    char fileData[myPM->getBlockSize()];
    rv = myPM->readDiskBlock(dataBlknum, fileData);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    
    /* Read in finode */
    char finode[myPM->getBlockSize()];
    rv = myPM->readDiskBlock(FOT[fileDesc].finodeblk, finode);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    /***************************************************************************/
    
    /* Starting at index, write data into block(buffer) */
    
    int bytesLeft = myPM->getBlockSize() - index; //available bytes left in this block
    int bytesToWrite = len - bytesWritten; //number of bytes left to write
    int bytesOut; //number of bytes we will write on this loop
    bool nextBlock = false;
    
    /* Write bytesToWrite characters into block(buffer), if possible*/
    if(bytesToWrite > bytesLeft)
    {
      /*More data left to write than space in this block. Will need another block, possibly allocated new*/
      bytesOut = bytesLeft;
      nextBlock = true;
    }
    else bytesOut = bytesToWrite;
    
    /* Write bytesOut number of bytes. Could be to end of block or end of file or both*/
    for(int i = 0; i < bytesOut; i++)
    {
      fileData[index + i] = data[bytesWritten];
      bytesWritten++;
      FOT[fileDesc].seek++;
    }
    
    /* Write data block out*/
    rv = myPM->writeDiskBlock(dataBlknum, fileData);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    
    /* Update file size in finode*/
    if(FOT[fileDesc].seek > charToInt(2, finode))
      intToChar(2, (FOT[fileDesc].seek), finode);//File size @ byte 2
      /* Write out finode */
      rv = myPM->writeDiskBlock(FOT[fileDesc].finodeblk, finode);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    /***************************************************************************/
    
    if(nextBlock)
    {
      /* Check if there is another block already allocated for this file's data */
      //findSeek() check ret value
      index =  findPos(FOT[fileDesc].seek, dataBlknum, FOT[fileDesc].finodeblk);
      /* If there is not a next block*/
      if(index == -7)
      {
        rv = newFileDataBlock(FOT[fileDesc].finodeblk);
        if((rv == -2) || (rv == -4)) return -3; //cannot complete operation
        index = findPos(FOT[fileDesc].seek, dataBlknum, FOT[fileDesc].finodeblk);
        if(index == -6 || index == -5 || index == -4) return bytesWritten;
        else if(index == -7)
        {
          //I done messed up
          cerr << "Fatal error in writeFile()" << endl;
          exit(1);
        }      }
        else if(index == -6 || index == -5 || index == -4) return bytesWritten;
        
        
        /* Else if there is a next block, findSeek() already put us at the right spot for repeating the loop */
    }
  }
  
  return bytesWritten;
}

int FileSystem::appendFile(int fileDesc, char *data, int len)
{
  fileDesc--;
  int rv;
  if(fileDesc > int(FOT.size() - 1)) return -1; //invalid file descriptor
  if(FOT[fileDesc].finodeblk == 0) return -1; //invalid file descriptor
  if(FOT[fileDesc].mode == 'r') return -3; // invalid mode
  /* Read in finode*/
  char finode[myPM->getBlockSize()];
  rv = myPM->readDiskBlock(FOT[fileDesc].finodeblk, finode);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  
  /* Get size of file */
  int size = charToInt(2, finode);
  /* Seek to end of file */
  fileDesc++; //increment fileDesc for other public functions
  rv = seekFile(fileDesc, size, 1);
  if(rv != -2) return -3; //someting went wrong
  
  /* Write data to file */
  
  return writeFile(fileDesc, data, len);
  
}

int FileSystem::seekFile(int fileDesc, int offset, int flag)
{
  fileDesc--;
  if (fileDesc > int(FOT.size() - 1)) return -1; //Invalid fileDesc
  
  /* Read in finode */
  char finode[myPM->getBlockSize()];
  int rv = myPM->readDiskBlock(FOT[fileDesc].finodeblk, finode);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  int size = charToInt(2, finode);
  
  
  if (flag == 0)
  {
    if (((FOT[fileDesc].seek + offset) > size) || ((FOT[fileDesc].seek + offset) < 0))	return -2;// Attempt to seek out of bound
    FOT[fileDesc].seek += offset;
  }
  else
  {
    if(offset < 0) return -1; //Invalid offset
    if (offset > size)	return -2; // Attempt to seek out of bound
    FOT[fileDesc].seek = offset;
    if(offset == size) return -2; //allow seeking to end of file, but return error
  }
  
  return 0;
}

int FileSystem::renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2)
{
  /* Validate filenames */

  
  int rv;
  bool file = true;
  
  /*Follow directory path for filename2*/
  int dinodeBlknum2 =  pathSearch(filename2, 1); //1 to start at root directory
  if((dinodeBlknum2 == -1) || (dinodeBlknum2 == -4 )) return -4; //something went wrong
  
  char name2[3]; // 3 because directory names can only be 1 character
  memcpy(name2, filename2 + (fnameLen2 - 2), 2);
  name2[2] = 0;
  
  /* Find file mapping index number in correct directory inode block for filename2*/
  int index2 = findIndex(dinodeBlknum2, name2, 2, 'f');
  if(index2 == -4) return -5; //something went wrong
  if(index2 == -2) return -3; //filename2 does already exist that's a problem.
  if(index2 >= 0) return -3; //filename2 does already exist that's a problem.
  /***************************************************************************/
  
  /*Follow directory path for filename1*/
  int dinodeBlknum1 =  pathSearch(filename1, 1); //1 to start at root directory
  if((dinodeBlknum1 == -1) || (dinodeBlknum1 == -4 )) return -4; //something went wrong
  
  char name1[3]; // 3 because directory names can only be 1 character
  memcpy(name1, filename1 + (fnameLen1 - 2), 2);
  name1[2] = 0;
  
  /* Find file mapping index number in correct directory inode block for filename1*/
  int index1 = findIndex(dinodeBlknum1, name1, 2, 'f');
  if(index1 == -1) return -2; //filename1 does not exist
  if(index1 == -4) return -5; //something went wrong
  if(index1 == -2) // We're actually dealing with a directory to be renamed
  {
    index1 = findIndex(dinodeBlknum1, name1, 2, 'd');
    if(index1 == -1) return -2; //filename1 does not exist
    if(index1 == -4) return -5; //something went wrong
    if(index1 == -2) return -5; // something went wrong
    file = false;
  }
  /***************************************************************************/
  
  /* Read in dinode*/
  char dinode[myPM->getBlockSize()];
  
  rv = myPM->readDiskBlock(dinodeBlknum1, dinode);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -5; //something went wrong
  
  /***************************************************************************/
  
  if(file)
  {
    char finode[myPM->getBlockSize()];
    int finodeBlknum = charToInt(index1 + 1, dinode);
    
    /* Check to see if file is locked*/
    if(lockIdT.find(finodeBlknum) != lockIdT.end()) return -4; //file is locked
    
    /* Check if file is open */
    /* Use STL find to search for an element in the file open table(FOT) matching filename1*/
    FileOpen element;
    element.finodeblk = finodeBlknum;
    vector<FileOpen>::iterator it;
    it = find(FOT.begin(), FOT.end(), element);
    if(it != FOT.end()) return -4; //file is open
    
    /***************************************************************************/
    
    /* Read in finode*/
    rv = myPM->readDiskBlock(finodeBlknum, finode);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -5; //something went wrong
    /***************************************************************************/
    
    /* Change finode name entry to new name */
    finode[0] = filename2[fnameLen2 - 1]; //last character is filename
    
    /* Write finode to disk*/
    rv = myPM->writeDiskBlock(finodeBlknum, finode);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -5; //something went wrong
  }
  
  /* Change file mapping in parent dinode to new name*/
  dinode[index1] = filename2[fnameLen2 - 1];
  
  /* Write dinode to disk*/
  rv = myPM->writeDiskBlock(dinodeBlknum1, dinode);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -5; //something went wrong
  /***************************************************************************/
  
  return 0;
}

int FileSystem::getAttributes(char *filename, int fnameLen, char* buffer, int flag)
{
  /* Validate filenames */
  
  int dinodeBlknum = pathSearch(filename, 1); //1 to start at root directory
  if (dinodeBlknum == -1) return -1; //File does not exist
  if (dinodeBlknum == -4) return -4; //something went wrong
  
  char dinode[myPM->getBlockSize()];
  int ret = myPM->readDiskBlock(dinodeBlknum, dinode);
  if (ret == -1) cerr << "Disk could not be opened!" << endl;
  if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
  if (ret != 0) return -3; //something went wrong
  
  char name[3]; // 3 because directory names can only be 1 character
  memcpy(name, filename + (fnameLen - 2), 2);
  name[2] = 0;
  
  int index = findIndex(dinodeBlknum, name, 2, 'f');
  if (index == -1) return -1; //file does not exist
  if (index == -4) return -3; //something went wrong
  if (index == -2) return -3; //name exists but is wrong type
  
  int finodeBlknum = charToInt(index + 1, dinode);
  char finode[myPM->getBlockSize()];
  
  ret = myPM->readDiskBlock(finodeBlknum, finode);
  if (ret == -1) cerr << "Disk could not be opened!" << endl;
  if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
  if (ret != 0) return -3; //something went wrong
  
  if (flag == 0)	//Search for Color attribute
  {
    for (int i = 0; i < 10; i++)
    {
      buffer[i] = finode[22 + i];
    }
    
    return 0;
  }
  if (flag == 1)	//Search for Owner attribute
  {
    int i = 0;
    for (; i < myPM->getBlockSize() - 32; i++)
    {
      buffer[i] = finode[32 + i];
      if (finode[32 + i] == 0) return 0;
    }
    return 0;
  }
  return -1;
}

int FileSystem::setAttributes(char *filename, int fnameLen, char* buffer, int flag)
{
  /* Validate filenames */
  
  int dinodeBlknum = pathSearch(filename, 1); //1 to start at root directory
  if (dinodeBlknum == -1) return -1; //File does not exist
  if (dinodeBlknum == -4) return -4; //something went wrong
  
  char dinode[myPM->getBlockSize()];
  int ret = myPM->readDiskBlock(dinodeBlknum, dinode);
  if (ret == -1) cerr << "Disk could not be opened!" << endl;
  if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
  if (ret != 0) return -3; //something went wrong
  
  char name[3]; // 3 because directory names can only be 1 character
  memcpy(name, filename + (fnameLen - 2), 2);
  name[2] = 0;
  
  int index = findIndex(dinodeBlknum, name, 2, 'f');
  if (index == -1) return -1; //file does not exist
  if (index == -4) return -3; //something went wrong
  if (index == -2) return -3; //name exists but is wrong type
  
  int finodeBlknum = charToInt(index + 1, dinode);
  char finode[myPM->getBlockSize()];
  ret = myPM->readDiskBlock(finodeBlknum, finode);
  if (ret == -1) cerr << "Disk could not be opened!" << endl;
  if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
  if (ret != 0) return -3; //something went wrong
  
  if (flag == 0)	//Search for Color attribute
  {
    for (int i = 0; i < 10; i++)
    {
      finode[22 + i] = buffer[i];
    }
    ret = myPM->writeDiskBlock(finodeBlknum, finode);
    if (ret == -1) cerr << "Disk could not be opened!" << endl;
    if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
    if (ret != 0) return -3; //something went wrong
    return 0;
  }
  if (flag == 1)	//Search for Owner attribute
  {
    for (int i = 0; i < myPM->getBlockSize() - 32; i++)
    {
      finode[32 + i] = buffer[i];
      if (buffer[i] == 0)	break;
    }
    ret = myPM->writeDiskBlock(finodeBlknum, finode);
    if (ret == -1) cerr << "Disk could not be opened!" << endl;
    if (ret == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if (ret == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
    if (ret != 0) return -3; //something went wrong
    return 0;
  }
  return -1;
}

/* Start Helper Functions */

void FileSystem::writeRoot()
{
  //hardcoded blocknumber
  //for every entry in the map:
  // write out tagname = key
  //write out value = blocknum of tagtree
  //remember to leave space at end of block for continuation
  //write out sentinel value to denote end - blocknumtype size bytes long of 0 bytes
  // in order to just zero out continuation in the event we fill up a block perfectly
}

void FileSystem::readRoot()
{
  //hardcoded blocknumber
  //until reading in the sentinel:
  // read in key = tagname
  //read in value = blocknumber of tagtree
  // insert in map
  //remember to read extension blocks when necessary
}


int FileSystem::findEmpty(int& blknum, char* name, int nameLen, char type)
{
  int rv;
  int retIndex;
  
  int currentBlock = blknum; //the block number of the start of the directory to search in
  int index = 0;
  bool looking = true; //looking until hiting end of directory
  bool found = false; //for empty spot
  char buffer[64];
  
  rv = myPM->readDiskBlock(currentBlock, buffer);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  
  while(looking)
  {
    /* start of each file map entry is multiple of 6*/
    if(!found && (buffer[index] == 0))
    {
      /* found empty spot, but need to keep looking for duplicate file name */
      found = true;
      retIndex = index;
      blknum = currentBlock;
    }
    
    if(buffer[index] == name[nameLen - 1])
    {
      /* duplicate name encountered, check if file or dir */
      if(buffer[index + 5] == type) return -1;
      else return -4; //opposity of type exists with same name
    }
    if(index == 54)
    {
      /* Reached end of block, go to extension block if it exists and we haven't found spot*/
      if(!found)index = 0;
      if(buffer[60] == 0)
      {
        /* No extension block */
        looking = false; //reached end of directory inode, done looking
        if(!found)
        {
          /* No spot found yet, Attempt to allocate block for file/directory */
          int extBlockAddr = myPM->getFreeDiskBlock();
          if(extBlockAddr == -1) return -2; // No free blocks
          blknum = extBlockAddr;
          retIndex = 0;
          /* update directory extension pointer to point to new block*/
          intToChar(60, extBlockAddr, buffer);
          rv = myPM->writeDiskBlock(currentBlock, buffer);
          if(rv == -1) cerr << "Disk could not be opened!" << endl;
          if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
          if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
          if(rv != 0) return -4; //something went wrong
          /*Zero out new block*/
          memset(buffer, 0, myPM->getBlockSize());
          rv = myPM->writeDiskBlock(extBlockAddr, buffer);
          
          if(rv == -1) cerr << "Disk could not be opened!" << endl;
          if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
          if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
          if(rv != 0) return -4; //something went wrong
        }
      }
      else
      {
        /* Go to extension block*/
        int extBlockAddr = charToInt(60, buffer);
        currentBlock = extBlockAddr;
        rv = myPM->readDiskBlock(currentBlock, buffer);
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
      }
    }
    else index += 6;
  }
  
  return retIndex;
}

int FileSystem::findIndex(int& blknum, char* name, int nameLen, char type)
{
  int rv;
  int retIndex;
  
  int currentBlock = blknum; //the block number of the start of the directory to search in
  int index = 0;
  bool looking = true; //looking until hiting end of directory, or found file
  bool found = false; //for when/if we find the file/dir
  char buffer[64];
  
  rv = myPM->readDiskBlock(currentBlock, buffer);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  
  while(looking && !found)
  {
    /* Check if current index is the correct file */
    if(buffer[index] == name[nameLen - 1])
    {
      if(buffer[index + 5] != type) return -2; //name exists but is not the correct type
      found = true;
      retIndex = index;
      blknum = currentBlock;
    }
    
    if(index == 60)
    {
      index = 0;
      /* Reached end of block, go to extension block if it exists*/
      if(buffer[60] == 0)
      {
        /* No extension block */
        looking = false; //reached end of directory inode, done looking
      }
      else
      {
        /* Go to extension block*/
        int extBlockAddr = charToInt(60, buffer);
        currentBlock = extBlockAddr;
        rv = myPM->readDiskBlock(currentBlock, buffer);
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
      }
    }
    else index += 6;
  }
  if(found == false) return -1;//file/directory did not exist
  return retIndex;
}


int FileSystem::dirCleanUp(char* name, int nameLen, int blknum)
{
  int rv;
  /* Starting out simple, will add complexity as needed */
  
  int parentBlknum = blknum; //Will later be value from above
  int extBlknum = 0; //will set before using
  char parentBlock[64];
  char extBlock[64];
  
  /* Read in currentBlock */
  rv = myPM->readDiskBlock(parentBlknum, parentBlock);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  
  
  if(parentBlock[60] == 0) return 0; //no extension block, all done!
  
  
  /* There is an extension block, let the fun begin */
  bool looking = true;
  
  while(looking)
  {
    /* Read in ext block*/
    extBlknum = charToInt(60, parentBlock);
    
    rv = myPM->readDiskBlock(extBlknum, extBlock);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    
    /* Check if extBlock is empty*/
    bool empty = true;
    for(int i = 0; i < 60; i += 6)
    {
      if(extBlock[i] != 0)
      {
        empty = false;
        break;
      }
    }
    
    /* If ext block is empty, free block*/
    if(empty)
    {
      /* If ext block has an extension send that up to parent block*/
      if(extBlock[60] != 0)
      {
        intToChar(60, charToInt(60, extBlock), parentBlock);
        rv = myPM->writeDiskBlock(parentBlknum, parentBlock);
        
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
      }
      else //zero out parent block ext address
      {
        looking = false; /* All done */
        memset(parentBlock + 60, 0, 4);
        rv = myPM->writeDiskBlock(parentBlknum, parentBlock);
        
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
        
        
      }
      
      /* Free block */
      rv = myPM->returnDiskBlock(extBlknum);
      if(rv != 0) return -4; //something went wrong
      
    }
    /* If ext block is not empty set it as parent block and get ready to read in next extBlock*/
    else
    {
      parentBlknum = extBlknum;
      rv = myPM->readDiskBlock(parentBlknum, parentBlock);
      if(rv == -1) cerr << "Disk could not be opened!" << endl;
      if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
      if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
      if(rv != 0) return -4; //something went wrong
      
      if(parentBlock[60] == 0) looking = false; //if no ext block all done
    }
  }
  
  return 0;
}

int FileSystem::newFileDataBlock(int finodeBlknum)
{
  int rv;
  /* Allocate next block for data */
  int dataBlknum = myPM->getFreeDiskBlock();
  if(dataBlknum == -1) return -4; // No free blocks
  
  int blknum = finodeBlknum;
  /* Read in finode */
  char buffer[myPM->getBlockSize()];
  rv = myPM->readDiskBlock(blknum, buffer);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  
  /* Need to update finode to point to newly allocated block */
  int i = 0;
  int index = 6; //Direct Addresses start @ byte 6
  while(i < 19)
  {
    if(i == 3)// Need to start looking at indirect address
    {
      index = 0;
      /* If indirect Inode does not exist create it. */
      if(buffer[18] == 0)
      {
        blknum = myPM->getFreeDiskBlock();
        if(blknum == -1) return -4; // No free blocks
        
        /* Write out indirect inode address to finode buffer */
        intToChar(18, blknum, buffer);
        /* Write finode to disk */
        rv = myPM->writeDiskBlock(finodeBlknum, buffer);
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
        
        /* Zero out indirect Inode */
        char temp[myPM->getBlockSize()];
        memset(temp, 0, myPM->getBlockSize());
        
        rv = myPM->writeDiskBlock(blknum, temp);
        if(rv == -1) cerr << "Disk could not be opened!" << endl;
        if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
        if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
        if(rv != 0) return -4; //something went wrong
        
      }
      else blknum = charToInt(18, buffer); //set blknum to indinode address
      /* Read in indinode*/
      rv = myPM->readDiskBlock(blknum, buffer);
      if(rv == -1) cerr << "Disk could not be opened!" << endl;
      if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
      if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
      if(rv != 0) return -4; //something went wrong
    }
    
    
    /* Look for open spot for file addressing */
    
    if(buffer[index] == 0) break; //found open spot
    index+=4;
    
    i++;
  }
  if(i == 19) return -2; //No open spots
  /* map to index */
  intToChar(index, dataBlknum, buffer);
  
  /* Write out either finode or indinode */
  rv = myPM->writeDiskBlock(blknum, buffer);
  if(rv == -1) cerr << "Disk could not be opened!" << endl;
  if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
  if(rv != 0) return -4; //something went wrong
  return 0;
}

int FileSystem::findPos(int seek, int& blknum, int finodeblknum)
{
  /* Read in finode */
  char finode[myPM->getBlockSize()];
  int rv = myPM->readDiskBlock(finodeblknum, finode);
  if (rv == -1) cerr << "Disk could not be opened!" << endl;
  if (rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
  if (rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
  if (rv != 0) return -4; //something went wrong
  
  //char buffer[64];
  if (seek < 0)	return -5; //Seek out of "negative" bound
  else if (seek < 64)
  {
    blknum = charToInt(6, finode);
    if (blknum <= 0)	return -7;	//Block not allocated
    return seek;
  }
  else if (seek < 128)
  {
    blknum = charToInt(10, finode);
    if (blknum <= 0)	return -7;	//Block not allocated
    return seek % 64;
  }
  else if (seek < 192)
  {
    blknum = charToInt(14, finode);
    if (blknum <= 0)	return -7;	//Block not allocated
    return seek % 64;
  }
  else if (seek < 1216)
  {
    blknum = charToInt(18, finode);
    if (blknum <= 0)	return -7;	//Block not allocated
    char buffer[myPM->getBlockSize()];
    rv = myPM->readDiskBlock(blknum, buffer);
    if (rv == -1) cerr << "Disk could not be opened!" << endl;
    if (rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if (rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist" << endl;
    if (rv != 0) return -4; //something went wrong
    
    int offset = (seek - 192) / 64;
    blknum = charToInt(offset * 4, buffer);
    if (blknum <= 0)	return -7;	//Block not allocated
    return seek % 64;
  }
  else return -6; // Seek out of positive bound
}

int FileSystem::pathSearch(char* path, int parentDinodeBlknum)
{
  int rv;
  if(strlen(path) == 2)
  {
    /* Base case, return the current dinodeBlknum*/
    return parentDinodeBlknum;
  }
  else
  {
    /* find directory mapping in parentDinodeBlknum*/
    char name[3]; // 3 because directory names can only be 1 character
    memcpy(name, path , 2);
    name[2] = 0;
    
    
    int index = findIndex(parentDinodeBlknum, name, 2, 'd');
    if(index == -1) return -1; //file does not exist
    if(index == -4) return -4; //something went wrong
    
    /* Read in parent dinode*/
    char parentDinode[myPM->getBlockSize()];
    
    rv = myPM->readDiskBlock(parentDinodeBlknum, parentDinode);
    if(rv == -1) cerr << "Disk could not be opened!" << endl;
    if(rv == -2) cerr << "Invalid blocknumber for partition: " << myfileSystemName << endl;
    if(rv == -3) cerr << "Partition: " << myfileSystemName << " does not exist"<< endl;
    if(rv != 0) return -4; //something went wrong
    
    return pathSearch(path + 2, charToInt(index + 1, parentDinode));
  }
}

bool operator==(const FileOpen& lhs, const FileOpen& rhs)
{
  if((lhs.finodeblk == rhs.finodeblk)) return true;
  else return false;
}
