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

FileInfo::FileInfo(string filename,BlkNumType blknum):_fileName(filename), _fidentifier(blknum)
{}


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

TagTree::TagTree(BlkNumType blknum): _blockNumber(blknum)
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut(PartitionManager* pm)
{
  //TODO: zero out the first byte of the key on disk for the rest of the block 
  //TODO: and any continuation blocks to account for any deletions when it was in memory 
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
      catch(...) {cerr << "Error trees.cpp3" << endl;}
      
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
    offset += sizeof(BlkNumType);
  }
}

void TagTree::readIn(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int offset = 0;
  BlkNumType currentBlkNum = _blockNumber;
  
  /*Read in first block*/
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentBlkNum, buff);}
  catch(...){cerr << "Error trees.cpp5" << endl;}
  
  
  bool reading = buff[0] == 0;
  
  /*For every entry on disk*/  
  while(reading)//assuming that offset always points to the start of an entry name.
  {
//     char * temp = new char[65]; //65 to accomodate the potential missing null byte
//     memset(temp, 0, 65);
    
//     strncpy(temp, buff + offset, 64);
    string fileName;
    //TODO: this may or may not work... uncomment neaby lines if needed
//     fileName.assign(temp);
    fileName.assign(buff + offset, 64);
//     delete temp;
    offset+= 64;
    
    /*Read in the blknum*/
    BlkNumType blknum;
    memcpy(&blknum, buff + offset, sizeof(BlkNumType));
    
    FileInfo* finode = new FileInfo(fileName, blknum);
    
    /*Insert key an value into tagtree in memory*/
    _tree.insert(pair<string, FileInfo*>(fileName, finode));
    //TODO: check insert ret val, if failed throw error someone fucked up
    offset += sizeof(BlkNumType);
    
    /**************************************************************************/ 
    /*check to see if there is another entry in this block and if there is a cont. block*/
    
    int entrySize = 64 + (sizeof(BlkNumType)*2);
    if((pm->getBlockSize() - offset) < entrySize)
    {
      /*can't be another entry in this block, check for cont. block*/
      memcpy(&currentBlkNum, buff + (pm->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
      
      if(currentBlkNum == 0){reading = false;} //no cont. block. all done reading in tree
      else
      {
        //Cont. block exists. read it in and start over.
        /*Read in cont. block*/
        //TODO: fix catch statement.
        try{pm->readDiskBlock(currentBlkNum, buff);}
        catch(...){cerr << "Error trees.cpp6" << endl;}
        offset = 0;
      }
    }
    
    if(buff[offset] == 0){reading = false;} //not another entry in this block
  }
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