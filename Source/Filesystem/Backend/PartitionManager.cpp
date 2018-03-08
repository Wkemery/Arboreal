///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PartitionManager.cpp
//  PartitionManager Source File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "PartitionManager.h"
#include <iostream>
#include <string.h>

using std::cerr;
using std::string;
using std::endl;



PartitionManager::PartitionManager(DiskManager *dm, string partitionName)
{
  _myDM = dm;
  _partitionName = partitionName;
  _partitionSize = dm->findPart(partitionName)->partitionSize;
  _partitionBlkStart = dm->findPart(partitionName)->partitionBlkStart;
  _fileNameSize = dm->findPart(partitionName)->fileNameSize;

  char* buff = new char[getBlockSize()];
  //relative block 0 is where the next free ptr will be stored.
  
  readDiskBlock(0, buff);
  
  int offset = 0;//byte 0 is where the start of free list is
  BlkNumType blknum;
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  offset+= sizeof(BlkNumType);
  if(blknum == 0)
  {
    /*No freelist, disk must be full*/
    delete[] buff;
    throw disk_error("Partition is Full!", "PartitionManager::PartitionManager()");
  }
  
  _freeBlockStart = blknum;
  
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  if(blknum == 0)
  {
    /*No freelist, disk must be full*/
    delete[] buff;
    throw disk_error("Partition Full", "PartitionManager::PartitionManager");
  }
  
  _freeBlockEnd = blknum;
  offset+= sizeof(BlkNumType);
  delete[] buff;
}

PartitionManager::~PartitionManager()
{}

BlkNumType PartitionManager::getFreeDiskBlock()
{
  //TODO: what if freeblock start and end are the same? well then this must be the last free block
  char* buff = new char[getBlockSize()];
  BlkNumType ret = _freeBlockStart;
  readDiskBlock(_freeBlockStart, buff);
  
  int offset = sizeof(BlkNumType);//second position is where the next free block is
  BlkNumType blknum;
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  
  if(blknum == 0)
  {
    /*There is no next free block, this must be the last one*/
    if(DEBUG) cerr << "WARNING! Last free block on Partition has been allocated" << endl;
  }
  
  _freeBlockStart = blknum;
  
  /*Write out freeblock*/
  readDiskBlock(0, buff);
  
  memcpy(buff, &_freeBlockStart, sizeof(BlkNumType));
  
  /*Write out block 0*/
  writeDiskBlock(0, buff);
  
  /*update/write out new _freeBlockStart*/
  readDiskBlock(_freeBlockStart, buff);
  
  /*Set previous to 0*/
  memset(buff, 0, sizeof(BlkNumType));
  writeDiskBlock(_freeBlockStart, buff);
  
  if(ret == 0)
  {
    delete[] buff;
    throw disk_error("Partition is Full!", "PartitionManager::getFreeDiskBlock()");
  }
  
  delete[] buff;
  return ret;
  
}

void PartitionManager::returnDiskBlock(BlkNumType blknum)
{
  char* buff = new char[getBlockSize()];
  int offset = sizeof(BlkNumType);
  /* Prevent deallocating of block 0*/
  
  if (blknum == 0 || blknum > _partitionSize)
  {
    throw invalid_arg("Blocknumber out of bounds", "PartitionManager::returnDiskBlock");
  }

  /*Add blknum to end of free list */
  /*Read in last free block, modify position 2, the next block*/
  readDiskBlock(_freeBlockEnd, buff);
  
  memcpy(buff + offset, &blknum, sizeof(BlkNumType));
  
  writeDiskBlock(_freeBlockEnd, buff);
  
  /*NOTE: always zeroing block, acutally less work.*/
  /*prepare block for returning*/
  memset(buff, 0, getBlockSize());
  memcpy(buff, &_freeBlockEnd ,sizeof(BlkNumType));
  _freeBlockEnd = blknum;
 
  /*Write out new blockend*/
  writeDiskBlock(_freeBlockEnd, buff);
  
  /*Update superblock with new end of free list*/
  readDiskBlock(0, buff);
  
  memcpy(buff + offset, &blknum, sizeof(BlkNumType));
  
  writeDiskBlock(0, buff);
  delete[] buff;
}


void PartitionManager::readDiskBlock(BlkNumType blknum, char *blkdata)
{
  _myDM->readDiskBlock(_partitionName, blknum, blkdata);
}

void PartitionManager::writeDiskBlock(BlkNumType blknum, char *blkdata)
{
  _myDM->writeDiskBlock(_partitionName, blknum, blkdata);
}

size_t PartitionManager::getBlockSize()
{
  return _myDM->getBlockSize();
}

int PartitionManager::get_file_name_size()
{
  return _fileNameSize;
}  


string PartitionManager::getPartitionName()
{
  return _partitionName;
}

