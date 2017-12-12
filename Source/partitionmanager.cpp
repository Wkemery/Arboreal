#include "partitionmanager.h"
#include <iostream>
#include <string.h>
using namespace std;



PartitionManager::PartitionManager(DiskManager *dm, string partitionName)
{
  myDM = dm;
  _partitionName = partitionName;
  _partitionSize = dm->findPart(partitionName)->partitionSize;
  _partitionBlkStart = dm->findPart(partitionName)->partitionBlkStart;

  char* buff = new char[getBlockSize()];
  //TODO: fix catch
  //relative block 0 is where the next free ptr will be stored.
  try {readDiskBlock(0, buff);}
  catch(...){cerr << "Error part.cpp constructor" << endl;}
  
  int offset = 32;//byte 32 is where the start of free list is
  BlkNumType blknum;
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  offset+= sizeof(BlkNumType);
  if(blknum == 0)
  {
    //TODO: some sort of error here.
    cerr << "Error part.cpp constructor2" << endl;
  }
  
  _freeBlockStart = blknum;
  
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  if(blknum == 0)
  {
    //TODO: some sort of error here.
    cerr << "Error part.cpp constructor3" << endl;
  }
  
  _freeBlockEnd = blknum;
}

PartitionManager::~PartitionManager()
{}

BlkNumType PartitionManager::getFreeDiskBlock()
{
  //TODO: what is freeblock start and end are the same?
  char* buff = new char[getBlockSize()];
  BlkNumType ret = _freeBlockStart;
  try {readDiskBlock(_freeBlockStart, buff);}
  catch(...){cerr << "Error part.cpp getFreeDiskBlock" << endl;}
  
  int offset = sizeof(BlkNumType);//second position is where the next free block is
  BlkNumType blknum;
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  
  if(blknum == 0)
  {
    //TODO: some sort of error here.
    cerr << "Error part.cpp getFreeDiskBlock2" << endl;
  }
  
  _freeBlockStart = blknum;
  
  /*Write out freeblock*/
  try {readDiskBlock(0, buff);}
  catch(...){cerr << "Error part.cpp getFreeDiskBlock3" << endl;}
  
  offset = 32;//byte 32 is where the start of free list is
  memcpy(buff + offset, &_freeBlockStart, sizeof(BlkNumType));
  
  /*Write out buff*/
  try {writeDiskBlock(0, buff);}
  catch(...){cerr << "Error part.cpp getFreeDiskBlock4" << endl;}
  return ret;
}

void PartitionManager::returnDiskBlock(BlkNumType blknum)
{
  char* buff = new char[getBlockSize()];
  int offset = sizeof(BlkNumType);
  /* Prevent deallocating of block 0*/
  
  if (blknum == 0 || blknum > _partitionSize)
  {
    cerr << "Error part.cpp returnDiskBlock" << endl;
  }

  /*Add blknum to end of free list */
  /*Read in last free block, modify position 2, the next block*/
  try {readDiskBlock(_freeBlockEnd, buff);}
  catch(...){cerr << "Error part.cpp returnDiskBlock2" << endl;}
  
  memcpy(buff + offset, &blknum, sizeof(BlkNumType));
  
  try {writeDiskBlock(_freeBlockEnd, buff);}
  catch(...){cerr << "Error part.cpp returnDiskBlock3" << endl;}
  
  //TODO: always zeroing block, acutally less work.
  /*prepare block for returning*/
  memset(buff, 0, getBlockSize());
  memcpy(&_freeBlockEnd, buff, sizeof(BlkNumType));
  _freeBlockEnd = blknum;
  
  /*Write out new blockend*/
  try {writeDiskBlock(_freeBlockEnd, buff);}
  catch(...){cerr << "Error part.cpp returnDiskBlock4" << endl;}
  
}


void PartitionManager::readDiskBlock(BlkNumType blknum, char *blkdata)
{
  try{myDM->readDiskBlock(_partitionName, blknum, blkdata);}
  catch(...)
  {
    cerr << "PartitionManager::readDiskBlock" << endl;
  }
}

void PartitionManager::writeDiskBlock(BlkNumType blknum, char *blkdata)
{
  try{myDM->writeDiskBlock(_partitionName, blknum, blkdata);}
  catch(...)
  {
    cerr << "PartitionManager::readDiskBlock" << endl;
  }
}

int PartitionManager::getBlockSize()
{
  return myDM->getBlockSize();
}

string PartitionManager::getPartitionName()
{
  return _partitionName;
}

