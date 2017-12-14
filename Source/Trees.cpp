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

void insert(string tagName, BlkNumType blknum){
  //TODO: stub
  
}
void erase(string tagName){
  //TODO:stub
  
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

void FileInfo::del(PartitionManager* pm)
{}

Index* FileInfo::getIndex()
{
  return & _index;
}

/******************************************************************************/

TagTree::TagTree(BlkNumType blknum): _blockNumber(blknum)
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int offset = 0;
  BlkNumType currentBlkNum = _blockNumber;
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  
  /*_blockNumber is the super block for this tagTree*/
  /*Layout - 
   * TagName - fileNameSize - 64 for now
   * Size - size_t - 4 ?
   * Index of end of tag tree on disk - sizeof(Index)
   x*/
  memcpy(buff + offset, _tagName.c_str(), _tagName.size());
  offset+=  _tagName.size();
  
  size_t treeSize = _tree.size();
  memcpy(buff + offset, &treeSize, sizeof(size_t));
  offset+=  sizeof(size_t);
  
  memcpy(buff + offset, &_lastEntry, sizeof(Index));
  offset+=  sizeof(Index);
  
  /*Write out TagTree superblock*/
  try{pm->writeDiskBlock(currentBlkNum, buff);}
  catch(...){cerr << "Error TagTree::writeOut" << endl;}
  
  /****************************************************************************/
  /*Write out additions*/
  
  /*Check to see if last entry completes a block*/
  currentBlkNum = _lastEntry.blknum;
  if((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType))
  {
    /*Need to allocate a new block to start appending*/
    BlkNumType newblknum = 0;
    try{newblknum = pm->getFreeDiskBlock();}
    catch(...) {cerr << "Error TagTree::writeOut" << endl;}
    currentBlkNum = newblknum;
    offset = 0;
  }
  
  /*Begin appending additions*/
  for(size_t i; i < _additions.size(); i++)
  {
    
    /* write out key to buffer.*/
    int keySize = _additions[i]->getFilename().size();
    if(keySize > pm->getFileNameSize())
    {
      //TODO: throw error
      cerr << "Error TagTree::writeOut" << endl;
    }
    
    strncpy(buff + offset, _additions[i]->getFilename().c_str(), keySize);
    offset += keySize;
    
    /*pad to 64 bytes*/
    for(int i = keySize; i < pm->getFileNameSize(); i++) { offset++; }
    
    /*write out the blocknumber of the fidentifier to buffer, probably 8 bytes?*/
    BlkNumType fiden = _additions[i]->getFidentifier();
    memcpy(buff + offset, &fiden, sizeof(BlkNumType));
    offset += sizeof(BlkNumType);
    
    if((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType))
    {
      /*Need to allocate a new block to start appending*/
      BlkNumType newblknum = 0;
      try{newblknum = pm->getFreeDiskBlock();}
      catch(...) {cerr << "Error TagTree::writeOut" << endl;}
      currentBlkNum = newblknum;
      offset = 0;
      
      /*got a new block, set it as continuation.*/
      memcpy(buff + (pm->getBlockSize() - sizeof(BlkNumType)), &newblknum, sizeof(BlkNumType));
    }
    
    /*Write out buff to currentBlkNum*/
    //TODO: fix catch statement.
    try {pm->writeDiskBlock(currentBlkNum, buff);}
    catch(...){cerr << "Error TagTree::writeOut" << endl;}
  }
  
  /****************************************************************************/
  /*Write out deletions*/
  
  /*For every entry with the same BlockNumber*/
  for(size_t i = 0; i < _deletions.bucket_count(); i++)
  {
    currentBlkNum = _deletions.begin(i)->first;
    /*Read in currentBlkNum*/
    try {pm->readDiskBlock(currentBlkNum, buff);}
    catch(...){cerr << "Error TagTree::writeOut" << endl;}
    
    /* Modify all the entries in this block */
    for(auto local_it = _deletions.begin(i); local_it != _deletions.end(i); local_it ++)
    {
      offset = local_it->second->getIndex()->offset;
      
      /*Zero out name and blocknumber*/
      memset(buff + offset, 0, pm->getFileNameSize() + sizeof(BlkNumType));
  
      //TODO: some delete on local_it->second
    }
    
    /*Write out buff to currentBlkNum*/
    //TODO: fix catch statement.
    try {pm->writeDiskBlock(currentBlkNum, buff);}
    catch(...){cerr << "Error TagTree::writeOut" << endl;}
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
    
    /*Insert key and value into tagtree in memory*/
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
        /*Cont. block exists. read it in and start over.*/
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

void TagTree::deleteContBlocks(PartitionManager* pm, BlkNumType blknum)
{
  //TODO: stub
}

void TagTree::del(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  
  /*Read in the first block of this tagTree*/
  try{pm->readDiskBlock(_blockNumber, buff);}
  catch(...){cerr << "Error TagTree::del()" << endl;}
  
  
  /*Look for a continuation block*/
  BlkNumType contBlkNum;
  memcpy(&contBlkNum, buff + (pm->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
  
  if(contBlkNum != 0)
  {
    //TODO: catch
    try {deleteContBlocks(pm, contBlkNum);}
    catch(...){cerr << "Error TagTree::del" << endl;}
  }
  //TODO: catch
  try {pm->returnDiskBlock(_blockNumber);}
  catch(...){cerr << "Error TagTree::del" << endl;}
  
}


