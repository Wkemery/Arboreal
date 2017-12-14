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

bool operator ==(Index& lhs, Index& rhs)
{
  return ((lhs.blknum == rhs.blknum) && (lhs.offset == rhs.offset));
}

bool operator !=(Index& lhs, Index& rhs)
{
  return !(lhs == rhs);
}

/******************************************************************************/
TreeObject::TreeObject(string name, BlkNumType blknum)
:_name(name), _blockNumber(blknum)
{}

TreeObject::~TreeObject(){}

string TreeObject::getName(){return _name;}

void TreeObject::setIndex(Index index){_index.blknum = index.blknum; _index.offset = index.offset;}

Index* TreeObject::getIndex(){return &_index;}

BlkNumType TreeObject::getBlockNumber(){return _blockNumber;}

/******************************************************************************/
RootTree::RootTree():TreeObject("Root", 1)
{
  _lastEntry.blknum = 1;
  _lastEntry.offset = 0;
  _startBlock = 0;
}

RootTree::~RootTree(){}

unordered_map<string, TagTree*>* RootTree::getMap(){return &_tree;}

void RootTree::insertAddition(TagTree* tag)
{
  _additions.push_back(tag);
}

void RootTree::insertDeletion(TagTree* tag)
{
  _deletions.insert(pair<BlkNumType, TagTree*>(tag->getBlockNumber(), tag));
}

void RootTree::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  
  /*_blockNumber is the super block for the Root Tree */
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.length());
  currentIndex.offset+=  _name.length() + 1;
  
  size_t treeSize = _tree.size();
  
  RootSuperBlock rootInfo{treeSize, _lastEntry, _startBlock};
  
  memcpy(buff + currentIndex.offset, &rootInfo, sizeof(RootSuperBlock));
  currentIndex.offset+=  sizeof(RootSuperBlock);
  
  /*Write out RootTree superblock*/
  try{pm->writeDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error RootTree::writeOut" << endl;}
  
  /****************************************************************************/
  /*Write out additions*/
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  /*Check to see if last entry completes a block*/
  currentIndex.blknum = _lastEntry.blknum;
  if((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType)
    || ((_startBlock == 0) && treeSize > 0))
  {
    /*Need to allocate a new block to start appending*/
    BlkNumType newblknum = 0;
    try{newblknum = pm->getFreeDiskBlock();}
    catch(...) {cerr << "Error RootTree::writeOut" << endl;}
    currentIndex.blknum = newblknum;
    currentIndex.offset = 0;
    if(_startBlock == 0)
    {
      _startBlock = currentIndex.blknum;
    }
  }
  
  /*Begin appending additions*/
  if(_additions.size() > 0)
  {
    for(size_t i = 0; i < _additions.size(); i++)
    {
      
      /* write out key to buffer.*/
      int keySize = _additions[i]->getName().size();
      if(keySize > pm->getFileNameSize())
      {
        //TODO: throw error
        cerr << "Error RootTree::writeOut" << endl;
      }
      
      strncpy(buff + currentIndex.offset, _additions[i]->getName().c_str(), keySize);
      
      currentIndex.offset+= pm->getFileNameSize();
      
      /*write out the blocknumber of the tagTree to buffer, probably 8 bytes?*/
      BlkNumType tagBlk = _additions[i]->getBlockNumber();
      memcpy(buff + currentIndex.offset, &tagBlk, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
      
      if((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType))
      {
        
        /*Need to allocate a new block to keep appending*/
        BlkNumType newblknum = 0;
        try{newblknum = pm->getFreeDiskBlock();}
        catch(...) {cerr << "Error RootTree::writeOut" << endl;}
        
        /*got a new block, set it as continuation.*/
        memcpy(buff + (pm->getBlockSize() - sizeof(BlkNumType)), &newblknum, sizeof(BlkNumType));
        /*Write out buff to currentIndex.blknum*/
        //TODO: fix catch statement.
        try {pm->writeDiskBlock(currentIndex.blknum, buff);}
        catch(...){cerr << "Error RootTree::writeOut" << endl;}
        memset(buff, 0, pm->getBlockSize());
        currentIndex.blknum = newblknum;
        currentIndex.offset = 0;
      }
    }
    //TODO: fix catch statement.
    try {pm->writeDiskBlock(currentIndex.blknum, buff);}
    catch(...){cerr << "Error RootTree::writeOut" << endl;}
  }

  

  
  /*update _lastEntry*/
  _lastEntry.blknum = currentIndex.blknum;
  _lastEntry.offset = currentIndex.offset;
  
  /****************************************************************************/
  /*Write out deletions*/
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  /*For every entry with the same BlockNumber*/
  if(_deletions.size() > 0)
  {
    for(size_t i = 0; i < _deletions.bucket_count(); i++)
    {
      currentIndex.blknum = _deletions.begin(i)->first;
      /*Read in currentIndex.blknum*/
      try {pm->readDiskBlock(currentIndex.blknum, buff);}
      catch(...){cerr << "Error RootTree::writeOut" << endl;}
      
      /* Modify all the entries in this block */
      for(auto local_it = _deletions.begin(i); local_it != _deletions.end(i); local_it ++)
      {
        currentIndex.offset = local_it->second->getIndex()->offset;
        
        /*Zero out name and blocknumber*/
        memset(buff + currentIndex.offset, 0, pm->getFileNameSize() + sizeof(BlkNumType));
      }
      
      /*Write out buff to currentIndex.blknum*/
      //TODO: fix catch statement.
      try {pm->writeDiskBlock(currentIndex.blknum, buff);}
      catch(...){cerr << "Error RootTree::writeOut" << endl;}
    }
  }

}

void RootTree::readIn(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  Index currentIndex{_blockNumber, 0};
  
  /*Read in RootTree superblock*/
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error RootTree::readIn" << endl;}
  
  /*Store values from Root Tree superblock*/
//   _name.assign(buff + currentIndex.offset, 5);
  currentIndex.offset+=  5;
    
  RootSuperBlock rootInfo;
  
  memcpy(&rootInfo, buff + currentIndex.offset, sizeof(RootSuperBlock));
  currentIndex.offset+=  sizeof(RootSuperBlock);
  
  _lastEntry.blknum = rootInfo.lastEntry.blknum;
  _lastEntry.offset = rootInfo.lastEntry.offset;
  _startBlock = rootInfo.startBlock;
  /****************************************************************************/
  
  
  /*Read in the startBlock - first block with entries*/
  if(_startBlock == 0)
  {
    /*RootTree is empty*/
    return;
  }
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error RootTree::readIn" << endl;}
  
  /*Read in every entry on disk, up to and including the last entry, taking into account
   * there may be empty spaces*/ 
  do
  {
    string tagName;
    BlkNumType blknum;
    /*if this is a valid entry, read in filename and blknum*/
    if(buff[currentIndex.offset] != 0)
    {
      /*Read in tagName*/
      tagName.assign(buff + currentIndex.offset, pm->getFileNameSize());
      tagName = tagName.substr(0, tagName.find_first_of('\0'));
      currentIndex.offset+= pm->getFileNameSize();
      
      /*Read in blocknumber for that RootTree object*/
      memcpy(&blknum, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset+=  sizeof(BlkNumType);
      if(blknum == 0)
      {
        //TODO: throw error
        cerr << "Error RootTree::readIn" << endl;
      }
      
      /*Creat RootTree object*/
      TagTree* tagTree = new TagTree(tagName, blknum);
      tagTree->setIndex(currentIndex);
      if(tagTree == 0)
      {
        //TODO: throw error
        cerr << "Error RootTree::readIn" << endl;
      }
      
      /*Insert key and value into tagtree in memory*/
      auto it_ret = _tree.insert(pair<string, TagTree*>(tagName, tagTree));
      if(!it_ret.second)
      {
        //TODO: throw error
        cerr << "Error RootTree::readIn" << endl;
      }
    }
    else
    {
      currentIndex.offset += entrySize;
    }
    
    /**************************************************************************/ 
    /*check to see if there is room for another entry in this block.*/
    if((pm->getBlockSize() - currentIndex.offset) < entrySize + sizeof(BlkNumType))
    {
      /*Can't be another entry in this block, check for cont. block*/
      memcpy(&currentIndex.blknum, buff + (pm->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
      if(currentIndex.blknum == 0 && currentIndex != _lastEntry)
      {
        //TODO: throw error, the cont block chain is fucked
        cerr << "Error RootTree::readIn" << endl;
      }
      
      if(currentIndex.blknum != 0)
      {
        /*Cont. block exists. read it in and start over.*/
        /*Read in cont. block*/
        //TODO: fix catch statement.
        try{pm->readDiskBlock(currentIndex.blknum, buff);}
        catch(...){cerr << "Error RootTree::readIn" << endl;}
        currentIndex.offset = 0;
      }
    }
  }
  while(currentIndex != _lastEntry);
}

void RootTree::deleteContBlocks(PartitionManager* pm, BlkNumType blknum)
{
  //TODO: stub
  
}

void RootTree::del(PartitionManager* pm)
{
  //TODO: stub
  
}

/******************************************************************************/

TagTree::TagTree(string tagName, BlkNumType blknum):TreeObject(tagName, blknum)
{
  _lastEntry.blknum = blknum;
  _lastEntry.offset = 0;
  _startBlock = 0;
}

TagTree::~TagTree(){}

unordered_map<string, FileInfo*>* TagTree::getMap() {return &_tree;}

void TagTree::insertAddition(FileInfo* file)
{
  _additions.push_back(file);
}

void TagTree::insertDeletion(FileInfo* file)
{
  _deletions.insert(pair<BlkNumType, FileInfo*>(file->getBlockNumber(), file));
}

void TagTree::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  
  /*_blockNumber is the super block for this tagTree*/
  /*Layout - 
   * TagName - fileNameSize - 64 for now
   * Size - size_t - 4 ?
   * Index of end of tag tree on disk - sizeof(Index)
   * startBlocknumber - start of entries on disk - sizeof(BlkNumType)
   */
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.size());
  currentIndex.offset+= pm->getFileNameSize();
  
  TagTreeSuperBlock tagInfo{_tree.size(), _lastEntry, _startBlock};
  memcpy(buff + currentIndex.offset, &tagInfo, sizeof(TagTreeSuperBlock));
  currentIndex.offset+=  sizeof(TagTreeSuperBlock);
  
  /*Write out TagTree superblock*/
  try{pm->writeDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::writeOut" << endl;}
  
  /****************************************************************************/
  /*Write out additions*/
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  /*Check to see if last entry completes a block, or it has no start block*/
  currentIndex.blknum = _lastEntry.blknum;
  if(((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType))
    || ((_startBlock == 0) && _tree.size() > 0))
  {
    /*Need to allocate a new block to start appending*/
    BlkNumType newblknum = 0;
    try{newblknum = pm->getFreeDiskBlock();}
    catch(...) {cerr << "Error TagTree::writeOut" << endl;}
    currentIndex.blknum = newblknum;
    currentIndex.offset = 0;
    
    if(_startBlock == 0)
    {
      _startBlock = currentIndex.blknum;
    }
  }
  
  /*Begin appending additions*/
  for(size_t i; i < _additions.size(); i++)
  {
    
    /* write out key to buffer.*/
    int keySize = _additions[i]->getName().size();
    if(keySize > pm->getFileNameSize())
    {
      //TODO: throw error
      cerr << "Error TagTree::writeOut" << endl;
    }
    
    strncpy(buff + currentIndex.offset, _additions[i]->getName().c_str(), keySize);
    currentIndex.offset += keySize;
    
    /*pad to 64 bytes*/
    for(int i = keySize; i < pm->getFileNameSize(); i++) { currentIndex.offset++; }
    
    /*write out the blocknumber of the fidentifier to buffer, probably 8 bytes?*/
    BlkNumType fiden = _additions[i]->getBlockNumber();
    memcpy(buff + currentIndex.offset, &fiden, sizeof(BlkNumType));
    currentIndex.offset += sizeof(BlkNumType);
    
    if((pm->getBlockSize() - _lastEntry.offset) < entrySize + sizeof(BlkNumType))
    {
      /*Need to allocate a new block to keep appending*/
      BlkNumType newblknum = 0;
      try{newblknum = pm->getFreeDiskBlock();}
      catch(...) {cerr << "Error TagTree::writeOut" << endl;}
      
      /*got a new block, set it as continuation.*/
      memcpy(buff + (pm->getBlockSize() - sizeof(BlkNumType)), &newblknum, sizeof(BlkNumType));
      /*Write out buff to currentIndex.blknum*/
      //TODO: fix catch statement.
      try {pm->writeDiskBlock(currentIndex.blknum, buff);}
      catch(...){cerr << "Error TagTree::writeOut" << endl;}
      
      currentIndex.blknum = newblknum;
      currentIndex.offset = 0;
    }
  }
  
  /*Write out buff to currentIndex.blknum*/
  //TODO: fix catch statement.
  try {pm->writeDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::writeOut" << endl;}
  
  /*update _lastEntry*/
  _lastEntry.blknum = currentIndex.blknum;
  _lastEntry.offset = currentIndex.offset;
  
  /****************************************************************************/
  /*Write out deletions*/
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  if(_deletions.size() > 0)
  {
    /*For every entry with the same BlockNumber*/
    for(size_t i = 0; i < _deletions.bucket_count(); i++)
    {
      currentIndex.blknum = _deletions.begin(i)->first;
      /*Read in currentIndex.blknum*/
      try {pm->readDiskBlock(currentIndex.blknum, buff);}
      catch(...){cerr << "Error TagTree::writeOut" << endl;}
      
      /* Modify all the entries in this block */
      for(auto local_it = _deletions.begin(i); local_it != _deletions.end(i); local_it ++)
      {
        currentIndex.offset = local_it->second->getIndex()->offset;
        
        /*Zero out name and blocknumber*/
        memset(buff + currentIndex.offset, 0, pm->getFileNameSize() + sizeof(BlkNumType));
      }
      
      /*Write out buff to currentIndex.blknum*/
      //TODO: fix catch statement.
      try {pm->writeDiskBlock(currentIndex.blknum, buff);}
      catch(...){cerr << "Error TagTree::writeOut" << endl;}
    }
  }
}

void TagTree::readIn(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  Index currentIndex{_blockNumber, 0};
  
  /*Read in tagTree superblock*/
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::readIn" << endl;}
  
  /*Store values from superblock*/
  _name.assign(buff + currentIndex.offset, pm->getFileNameSize());
  _name = _name.substr(0, _name.find_first_of('\0'));
  currentIndex.offset+=  pm->getFileNameSize();
  
  TagTreeSuperBlock tagInfo;
  memcpy(&tagInfo, buff + currentIndex.offset, sizeof(TagTreeSuperBlock));
  currentIndex.offset+=  sizeof(TagTreeSuperBlock);
  
  _lastEntry.blknum = tagInfo.lastEntry.blknum;
  _lastEntry.offset = tagInfo.lastEntry.offset;
  _startBlock = tagInfo.startBlock;
  /****************************************************************************/
  /*Read in the startBlock - first block with entries*/
  if(_startBlock == 0)
  {
    /*TagTree is empty*/
    return;
  }
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::readIn" << endl;}
  
  /*Read in every entry on disk, up to and including the last entry, taking into account
   * there may be empty spaces*/ 
  do
  {
    string fileName;
    BlkNumType blknum;
    /*if this is a valid entry, read in filename and blknum*/
    if(buff[currentIndex.offset] != 0)
    {
      /*Read in Filename*/
      fileName.assign(buff + currentIndex.offset, pm->getFileNameSize());
      fileName = fileName.substr(0, fileName.find_first_of('\0'));
      currentIndex.offset+= pm->getFileNameSize();
      
      /*Read in blocknumber for that fileinfo object*/
      memcpy(&blknum, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset+=  sizeof(BlkNumType);
      if(blknum == 0)
      {
        //TODO: throw error
        cerr << "Error TagTree::readIn" << endl;
      }
      
      /*Creat finode object*/
      FileInfo* finode = new FileInfo(fileName, blknum);
      finode->setIndex(currentIndex);
      if(finode == 0)
      {
        //TODO: throw error
        cerr << "Error TagTree::readIn" << endl;
      }
      
      /*Insert key and value into tagtree in memory*/
      auto it_ret = _tree.insert(pair<string, FileInfo*>(fileName, finode));
      if(!it_ret.second)
      {
        //TODO: throw error
        cerr << "Error TagTree::readIn" << endl;
      }
    }
    else
    {
      currentIndex.offset += entrySize;
    }
    
    /**************************************************************************/ 
    /*check to see if there is room for another entry in this block.*/
    if((pm->getBlockSize() - currentIndex.offset) < entrySize + sizeof(BlkNumType))
    {
      /*Can't be another entry in this block, check for cont. block*/
      memcpy(&currentIndex.blknum, buff + (pm->getBlockSize() - sizeof(BlkNumType)), sizeof(BlkNumType));
      if(currentIndex.blknum == 0 && currentIndex != _lastEntry)
      {
        //TODO: throw error, the cont block chain is fucked
        cerr << "Error TagTree::readIn" << endl;
      }
      
      if(currentIndex.blknum != 0)
      {
        /*Cont. block exists. read it in and start over.*/
        /*Read in cont. block*/
        //TODO: fix catch statement.
        try{pm->readDiskBlock(currentIndex.blknum, buff);}
        catch(...){cerr << "Error TagTree::readIn" << endl;}
        currentIndex.offset = 0;
      }
    }
  }
  while(currentIndex != _lastEntry);
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

/******************************************************************************/
FileInfo::FileInfo(string fileName,BlkNumType blknum):TreeObject(fileName, blknum)
{}

FileInfo::~FileInfo(){}

unordered_map<string, BlkNumType>* FileInfo::getMap()
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

void FileInfo::del(PartitionManager* pm)
{}

void FileInfo::deleteContBlocks(PartitionManager* pm, BlkNumType blknum)
{}

/******************************************************************************/


