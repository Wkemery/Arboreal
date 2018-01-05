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
    //TODO: this is probably gonna print twice
    cerr << "Partition" << _partitionName << " has run out of space!" << endl;
//     throw disk_error("Partition Full", "PartitionManager::PartitionManager");
  }
  
  _freeBlockStart = blknum;
  
  memcpy(&blknum, buff + offset, sizeof(BlkNumType));
  if(blknum == 0)
  {
    //TODO: same thing as above.
    /*No freelist, disk must be full*/
    cerr << "Partition" << _partitionName << " has run out of space!" << endl;
//     throw disk_error("Partition Full", "PartitionManager::PartitionManager");
  }
  
  _freeBlockEnd = blknum;
  offset+= sizeof(BlkNumType);
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
    cerr << "WARNING! Last free block allocated" << endl;
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
}


void PartitionManager::readDiskBlock(BlkNumType blknum, char *blkdata)
{
  myDM->readDiskBlock(_partitionName, blknum, blkdata);
}

void PartitionManager::writeDiskBlock(BlkNumType blknum, char *blkdata)
{
  myDM->writeDiskBlock(_partitionName, blknum, blkdata);
}

int PartitionManager::getBlockSize()
{
  return myDM->getBlockSize();
}

int PartitionManager::getFileNameSize()
{
  return _fileNameSize;
}  


string PartitionManager::getPartitionName()
{
  return _partitionName;
}

