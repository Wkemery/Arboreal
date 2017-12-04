/*Trees.cpp
 * Arboreal
 * November 2017
 */


#include "Trees.h"
#include<string>
#include<string.h>
#include<unordered_map>
#include<iostream>

using namespace std;

FileInfo::FileInfo(string filename,PartitionManager* pm):_fileName(filename)
{
  /*Get a block from disk to store FInode*/
  
  //TODO: fix catch statement.
  BlkNumType newblknum = 0;
  try{newblknum = pm->getFreeDiskBlock();}
  catch(...){cerr << "Error trees.cpp1" << endl;}
  
  /*set newblknum as _fidentifier*/
  _fidentifier = newblknum;
}


BlkNumType FileInfo::getFidentifier(){ return _fidentifier;}

string FileInfo::getFilename() {return _fileName;}

unordered_map<string, BlkNumType>* FileInfo::getTags()
{
  return &_tags;
}

void FileInfo::writeOut(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
  //filename - 64 bytes
  // fidentifier - probably 8 bytes
  //file size
  
}

void FileInfo::readIn(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
}

void FileInfo::del()
{}

/******************************************************************************/

TagTree::TagTree(PartitionManager* pm)
{
  /*Get a block from disk to store tag tree*/
  
  //TODO: fix catch statement.
  BlkNumType newblknum = 0;
  try{newblknum = pm->getFreeDiskBlock();}
  catch(...){cerr << "Error trees.cpp1" << endl;}
  
  /*set newblknum as _blockNumber*/
  _blockNumber = newblknum;
  
}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int offset = 0;
  BlkNumType currentBlkNum = _blockNumber;
  
  /*for every entry in the _tree */
  for(auto it = _tree.begin(); it != _tree.end(); it++)
  {
    if(it->first.size() > 64)
    {
      //TODO: throw error someone fucked up
    }
    
    /**************************************************************************/ 
    /*check to see if there is room for another entry and the cont. block*/
    
    int entrySize = 64 + (sizeof(BlkNumType)*2);
    if((pm->getBlockSize() - offset) < entrySize)
    {
      //TODO: fix catch statement.
      BlkNumType newblknum = 0;
      try{newblknum = pm->getFreeDiskBlock();}
      catch(...) {cerr << "Error trees.cpp2" << endl;}
      
      /*got a new block, set it as continuation.*/
      memcpy(buff + (pm->getBlockSize() - sizeof(BlkNumType)), &newblknum, sizeof(BlkNumType));
      
      /*write out buffer to my current _blockNumber*/
      //TODO: fix catch statement.
      try {pm->writeDiskBlock(currentBlkNum, buff);}
      catch(...){cerr << "Error trees.cpp2" << endl;}
      
      /*Update current blocknumber*/
      currentBlkNum = newblknum;
      /*zero out buffer for reuse*/
      memset(buff, 0, pm->getBlockSize()); //zero out memory
      offset = 0;
    }
    /**************************************************************************/ 
    
    //TODO: note file name can be 64 bytes because null termination is not necessary
    
    /* write out key to buffer.*/
    strncpy(buff + (offset*sizeof(char)), it->first.c_str(), 64);
    offset += it->first.size();
    
    /*pad to 64 bytes*/
    for(int i = it->first.size(); i < 64; i++) { offset++; }
    
    /*write out the blocknumber of the fidentifier to buffer, probably 8 bytes?*/
    BlkNumType fiden = it->second->getFidentifier();
    memcpy(buff + (offset*sizeof(char)), &fiden, sizeof(BlkNumType));    
  }
}

void TagTree::readIn(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
}

void TagTree::zeroDisk()
{}

/******************************************************************************/


bool operator==(const pair<string, unsigned int>& lhs, const pair<string, unsigned int>& rhs)
{
  return (lhs.first == rhs.first);
}

size_t PairHash::operator()(const pair<string, unsigned int>& k) const
{
  return hash<string>()(k.first);
}