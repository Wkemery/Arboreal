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

//TODO: fix writing out so it doesn't take so long, keep track of additions or deletions to tree. 
// additions need only append new stuff on. 
// maybe for deletions, we somehow store the position of each entry, blknum and index, or something like that
// this will allow for quick deletions.
//However, this will require some sort of periodic cleanup. but that's allright because at least
// it will be fast in the short term

FileSystem::FileSystem(DiskManager *dm, string fileSystemName)
{
  myfileSystemName = fileSystemName;
//   myDM = dm;
  /* set partition manager for my partition */
  myPM = new PartitionManager(dm, fileSystemName);
  
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
    auto it = _RootTree.find(tags[0]);  //Complexity: avg. 1, worst linear
    it->second->getTree();
    
    //   * list files in tag tree pointed to by root tree
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    for(auto it2 = treeptr->begin(); it2 != treeptr->end(); it2++)//Complexity: number of files in answer
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
    for(unsigned int i = 0; i < tags.size(); i++)//Complexity: avg. number of tags specified, worst # tags specified*total number of user defined tags on filesystem
    {
      auto it = _RootTree.find(tags[i]);//complexity: avg. 1, worst linear
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
    for(unsigned int i = 1; i < searchTrees.size(); i++)//Complexity: number of tags specified
    {
      if(searchTrees[i]->getTree()->size() < smallest->getTree()->size())
        smallest = searchTrees[i];
    }
    
    //   * Search the smallest tree:
    unordered_map<string, FileInfo*>* treeptr = smallest->getTree();
    for(auto it = treeptr->begin(); it != treeptr->end(); it++)//Complexity: size of smallest specified tag tree * # of tags specified (worst case: size of smallest specified tag tree*# of tags specified* number of tags associated with file in question)
    {
      //   # elimnate all nodes with tag count < the number of tags you are searching for
      if(it->second->getTags()->size() >= tags.size())
      {
        //could be a matching file
        bool match = true;
        //   # search remainng files for exact tag match O(1) average case, worst case O(n)
        for(unsigned int i = 0; i < tags.size(); i++)//Complexity: # of tags specified(worst case: # of tags specified* number of tags associated with file in question)
        {
          if(it->second->getTags()->find(tags[i]) == it->second->getTags()->end()) //complexity: avg. O(1), worst number of tags associated with file in question
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
  //TODO: change to default tree search, try to make default tree an ordered map
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
  if(_RootTree.find(tagName) != _RootTree.end())//Complexity: avg 1, worst # tags on system
  {
    throw invalid_argument(tagName + " is not unique");
  }
  
  /*Get a block from disk to store tag tree*/
  
  //TODO: fix catch statement.
  BlkNumType newblknum = 0;
  try{newblknum = myPM->getFreeDiskBlock();}//Complexity: not our fucking problem, but linear with # of blocks on partition
  catch(...){cerr << "Error trees.cpp1" << endl;}
  
  //   - initialize tree in main memory
  TagTree* newTree = new TagTree(newblknum);
  //   - add respective node to root tree (write TagTree block num to Node as well as TagTree memory address to Node)
  
  _RootTree.insert(pair<string, TagTree*>(tagName, newTree));//Complexity: avg. 1, worst # of tags in filesystem
  //   - set up block in disk as empty tag tree, ie write out tag tree
  newTree->writeOut(myPM); //Complexity: size of tag tree, this case 0;
  //   - rebalance  Root tree, done automatically with map
  //   - write Root tree to disk
  writeRoot();
  
}

void FileSystem::deleteTag(string tagName, bool force)
{
  //   - find tagTree
  auto it = _RootTree.find(tagName);//Complexity: avg 1, worst # tags on system
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
    for(auto it2 = treeptr->begin(); it2 != treeptr->end(); it2++)//Complexity: avg: size of tag tree * sizeof file/block size, worst size of tag tree * avg tags associated with file * sizeof file/block size
    {
      //   - delete all references to the tag tree from Fileinodes
      it2->second->getTags()->erase(tagName);//Complexity: avg 1, worst # of tags associated with file
      it2->second->writeOut(myPM);//Complexity: size of file/ block size
    }
  }

  //   - encryption requires no zero-ing of disk
  if(!EncryptionFlag) it->second->zeroDisk();//Complexity: skip for now
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
  for(auto t : tags)//Complexity: avg:number of tags specified , worst: number of tags specified * size of root tree
  {
    //   - find tagTree
    
    auto it = _RootTree.find(t);//Complexity: avg: 1, worst: size of root tree
    //   - If tag does not exist create a new Tag tree
    //not doing this
//     if(it == _RootTree.end()) 
//     {
//       createTag(t);//Complexity: go to function
//       it = _RootTree.find(t);//Complexity: Complexity: avg: 1, worst: size of root tree
//     }
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    //   - Add Tag to Finode 
    file->getTags()->insert(pair<string, BlkNumType>(t, it->second->getBlockNum()));//Complexity: Complexity: avg: 1, worst: avg number of tags associated iwth file
    
    //   - Create and Add new Node to TagTree
    treeptr->insert(pair<string, FileInfo*>(file->getFilename(), file));//Complexity: Complexity: avg: 1, worst: size of tag tree
    
    //   - Write updated TagTree to disk
    it->second->writeOut(myPM);

  }

  // (write updated Finode to disk)
  file->writeOut(myPM);
  
}

void FileSystem::untagFile(FileInfo* file, vector<string> tags)
{
  //for all tags in the vector
  for(auto t : tags)
  {
    //   - find tagTree
    
    auto it = _RootTree.find(t);//Complexity: avg: 1, worst: size of root tree
    //   - If tag does not exist print error and continue
    if(it == _RootTree.end())
    {
      cerr << t + " cannot be removed from " << file->getFilename() << " : Tag Does not exist" << endl;
    }
    unordered_map<string, FileInfo*>* treeptr = it->second->getTree();
    
    //   - Remove Node from TagTree
    treeptr->erase(file->getFilename()); //Complexity: Complexity: avg: 1, worst: size of tag tree
    
    //   - Delete tag from FileInode
    file->getTags()->erase(t);//Complexity: Complexity: avg: 1, worst: avg number of tags associated iwth file
    
    //   - Write updated TagTree to disk
    it->second->writeOut(myPM);
    
  }
  
  // (write updated Finode to disk)
  file->writeOut(myPM);
}


FileInfo* FileSystem::createFile(string filename, vector<string> tags)
{
  /*Get a block from disk to store FInode*/
  
  //TODO: fix catch statement.
  BlkNumType newblknum = 0;
  try{newblknum = myPM->getFreeDiskBlock();}
  catch(...){cerr << "Error trees.cpp1" << endl;}
  
  FileInfo* newFile = new FileInfo(filename, newblknum);
  
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
  for(auto it = fileTags->begin(); it != fileTags->end(); it++)//Complexity: number of tags associated with file
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
  return 0;
}

int FileSystem::closeFile(int fileDesc)
{
  return (0); // return success
}

int FileSystem::readFile(int fileDesc, char *data, int len)
{return 0;}

int FileSystem::writeFile(int fileDesc, const char *data, int len)
{return 0;}

int FileSystem::appendFile(int fileDesc, char *data, int len)
{return 0;}

int FileSystem::seekFile(int fileDesc, int offset, int flag)
{return 0;}

int FileSystem::renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2)
{return 0;}

int FileSystem::getAttributes(char *filename, int fnameLen, char* buffer, int flag)
{return 0;}

int FileSystem::setAttributes(char *filename, int fnameLen, char* buffer, int flag)
{return 0;}

/* Start Helper Functions */

void FileSystem::writeRoot()
{
  
  char* buff = new char[myPM->getBlockSize()];
  memset(buff, 0, myPM->getBlockSize()); //zero out memory
  int offset = 0;
  BlkNumType currentBlkNum = 1;
  int entrySize = 64 + (sizeof(BlkNumType)*2);
  
  /*for every entry in root */
  for(auto it = _RootTree.begin(); it != _RootTree.end(); it++)
  {
    if(it->first.size() > 64)
    {
      //TODO: throw error someone fucked up
    }
    
    /**************************************************************************/ 
    /*check to see if there room for another entry and the cont. block*/
    
    
    if((myPM->getBlockSize() - offset) < entrySize)
    {
      /* There is not room for another entry. Check if cont. block exists.
        * if so, go to that block. if not, allocate new block 
        */
      
      /*Read in current block*/
      //TODO: fix catch statement.
      char* temp = new char[myPM->getBlockSize()];
      try{myPM->readDiskBlock(currentBlkNum, temp);}
      catch(...){cerr << "Error trees.cpp10" << endl;}
      
      /*Read in the cont. blocknum*/
      BlkNumType contBlkNum;
      memcpy(&contBlkNum, temp + (myPM->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
      delete temp;
      
      /************************************************************************/ 
      
      if(contBlkNum == 0)
      {
        /*No cont. block, allocate new block*/
        
        //TODO: fix catch statement.
        BlkNumType newblknum = 0;
        try{newblknum = myPM->getFreeDiskBlock();}
        catch(...) {cerr << "Error trees.cpp3" << endl;}
        
        /*got a new block, set it as continuation.*/
        memcpy(buff + (myPM->getBlockSize() - sizeof(BlkNumType)), &newblknum, sizeof(BlkNumType));
        
        /*write out buffer to my current _blockNumber*/
        //TODO: fix catch statement.
        try {myPM->writeDiskBlock(currentBlkNum, buff);}
        catch(...){cerr << "Error trees.cpp2" << endl;}
        
        /*Update current blocknumber*/
        currentBlkNum = newblknum;
        /*zero out buffer for reuse*/
        memset(buff, 0, myPM->getBlockSize()); //zero out memory
        offset = 0;
      }
      else
      {
        /*Set currentBlkNum to cont. blocknum*/
        currentBlkNum = contBlkNum;
      }
    }
    /**************************************************************************/ 
    
    //TODO: note file name can be 64 bytes because null termination is not necessary
    
    /* write out key to buffer.*/
    strncpy(buff + (offset*sizeof(char)), it->first.c_str(), 64);
    offset += it->first.size();
    
    /*pad to 64 bytes*/
    for(int i = it->first.size(); i < 64; i++) { offset++; }
    
    /*write out the blocknumber of the fidentifier to buffer, probably 8 bytes?*/
    BlkNumType fiden = it->second->getBlockNum();
    memcpy(buff + (offset*sizeof(char)), &fiden, sizeof(BlkNumType));
    offset += sizeof(BlkNumType);
  }
  
  /*Clean up block structure. zero out the first byte of the key on disk 
    * for the rest of the block and deallocate any continuation blocks to 
    * account for any deletions when it was in memory */
  
  /*Read in current block*/
  //TODO: fix catch statement.
  try{myPM->readDiskBlock(currentBlkNum, buff);}
  catch(...){cerr << "Error trees.cpp10" << endl;}
  
  /*Read in the cont. blocknum*/
  BlkNumType contBlkNum;
  memcpy(&contBlkNum, buff + (myPM->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
  
  
  /*Will zero out the key bits of all the cont. blocks*/
//   deleteContBlocks(contBlkNum);//TODO: function
  
  /*Zero out the rest of the entries in the currentBlkNum*/
  
  while((myPM->getBlockSize() - offset) > entrySize)
  {
    /*There can be more entries in this block*/
    buff[offset] = 0;
    offset += 64 + sizeof(BlkNumType);
  }
  
  /*Write out buff to currentBlkNum*/
  //TODO: fix catch statement.
  try {myPM->writeDiskBlock(currentBlkNum, buff);}
  catch(...){cerr << "Error trees.cpp2" << endl;}
  
  delete buff;
  
  //hardcoded blocknumber - 1
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

bool operator==(const FileOpen& lhs, const FileOpen& rhs)
{
  if((lhs.finodeblk == rhs.finodeblk)) return true;
  else return false;
}
