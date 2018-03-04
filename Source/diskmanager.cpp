#include "diskmanager.h"
#include <iostream>
#include<algorithm>
#include <string.h>
#include <cstdlib>
#include <stdio.h>
using namespace std;

bool operator==(const DiskPartition* lhs, const DiskPartition& rhs)
{
  return lhs->partitionName == rhs.partitionName;
}

DiskManager::DiskManager(Disk *d)
{
  myDisk = d;
  int offset = 0;
  char* buff = new char[getBlockSize()];
  memset(buff, 0, getBlockSize());
  /* Read superblock from disk*/
  myDisk->readDiskBlock(0, buff);

  /*successfully read superblock, read in partition info*/
  /*Layout -
    * number of partitions - int - 4 bytes
    * repeat: 
    *  partition name - 16 bytes
    *  partitions size in blocks - BlkNumType
    *  partition start pos - BlkNumType
    *  filenameSize limit - int
    */
  int numPartitions = 0;
  memcpy(&numPartitions, buff, sizeof(int));
  offset+= sizeof(int);
  //TODO: add functionality for more paritions than can fit on a single block, or set cap on paritions
  for(int i = 0; i < numPartitions; i++)
  {
    //TODO: check these values as your copying them to validate them.
    DiskPartition* temp = new DiskPartition{"", 0, 0};
    temp->partitionName.assign(buff + offset, 16);
    temp->partitionName = temp->partitionName.substr(0, temp->partitionName.find_first_of('\0'));
    offset+= 16;
    
    memcpy( &temp->partitionSize,buff + offset, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    memcpy(&temp->partitionBlkStart, buff + offset, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    memcpy(&temp->fileNameSize, buff + offset, sizeof(int));
    offset+= sizeof(int);
    
    _myPartitions.push_back(temp);
        
  }
  delete[] buff;
}

DiskManager::~DiskManager()
{
  for( auto part : _myPartitions)
  {
    if(part != 0)
    {
      delete part;
      part = 0;
    }
  }
}

void DiskManager::readDiskBlock(string partitionName, BlkNumType blknum, char *blkdata)
{
  /* find partition index in diskP */
  BlkNumType index = 0;
  index = findPart(partitionName)->partitionBlkStart;
  
  /* translate relative block number to absolute block number*/
  BlkNumType absBlockNum = index + blknum;
  
  /* read block data from block using disk read command*/
  myDisk->readDiskBlock(absBlockNum, blkdata);
}

void DiskManager::writeDiskBlock(string partitionName, BlkNumType blknum, char *blkdata)
{
  /* find partition index in diskP */
  BlkNumType index = 0;
  index = findPart(partitionName)->partitionBlkStart;
  
  /* translate relative block number to absolute block number */
  BlkNumType absBlockNum = index + blknum;
  
  /* write blkdata to block number using disk write command*/
  myDisk->writeDiskBlock(absBlockNum, blkdata); 
}

BlkNumType DiskManager::getPartitionSize(string partitionName)
{
  BlkNumType size = 0;
  size = findPart(partitionName)->partitionSize;
  return size;
}

DiskPartition* DiskManager::findPart(string partitionName)
{
  struct DiskPartition temp{partitionName, 0};
  auto it = find(_myPartitions.begin(), _myPartitions.end(), temp);
  if(it == _myPartitions.end())
  {
    throw tag_error("Partition Does Not Exist", "DiskManager::findPart");
  }
  return *it;
}

size_t DiskManager::getBlockSize()
{return myDisk->getBlockSize();}