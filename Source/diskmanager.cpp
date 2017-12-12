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
  /* Read superblock from disk*/
  try{myDisk->readDiskBlock(0, buff);}
  catch(...)
  {
    //TODO: error
    cerr << "DiskManager::DiskManager" << endl;
  }

  /*successfully read superblock, read in partition info*/
  /*Layout -
    * number of partitions - int - 4 bytes
    * repeat: 
    *  partition name - 16 bytes
    *  partitions size in blocks - BlkNumType
    *  partition start pos - BlkNumType
    */
  int numPartitions = 0;
  memcpy(buff, &numPartitions, sizeof(int));
  offset+= sizeof(int);
  //TODO: add functionality for more paritions than can fit on a single block, or set cap on paritions
  for(int i = 0; i < numPartitions; i++)
  {
    //TODO: check these values as your copying them to validate them.
    DiskPartition* temp = new DiskPartition{"", 0, 0};
    temp->partitionName.assign(buff + offset, 16);
    offset+= 16;
    
    memcpy(buff + offset, &temp->partitionSize, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    memcpy(buff + offset, &temp->partitionBlkStart, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    _myPartitions.push_back(temp);
    
  }
}

DiskManager::~DiskManager()
{}

void DiskManager::readDiskBlock(string partitionName, BlkNumType blknum, char *blkdata)
{
  /* find partition index in diskP */
  BlkNumType index = 0;
  try{index = findPart(partitionName)->partitionSize;}
  //TODO: fix catch
  catch(...)
  {
    cerr<< "error DiskManager::writeDiskBlock" << endl;
  }
  
  /* translate relative block number to absolute block number*/
  BlkNumType absBlockNum = index + blknum;
  
  /* read block data from block using disk read command*/
  try{myDisk->readDiskBlock(absBlockNum, blkdata);}
  //TODO: fix catch
  catch(...)
  {
    cerr<< "error DiskManager::writeDiskBlock2" << endl;
  }
  
}

void DiskManager::writeDiskBlock(string partitionName, BlkNumType blknum, char *blkdata)
{
  /* find partition index in diskP */
  BlkNumType index = 0;
  try{index = findPart(partitionName)->partitionSize;}
  //TODO: fix catch
  catch(...)
  {
    cerr<< "error DiskManager::writeDiskBlock" << endl;
  }
  
  /* translate relative block number to absolute block number */
  BlkNumType absBlockNum = index + blknum;
  
  /* write blkdata to block number using disk write command*/
  try{myDisk->writeDiskBlock(absBlockNum, blkdata);}
  catch(...)
  {
    cerr << "error DiskManager::writeDiskBlock2" << endl;
  }  
}

BlkNumType DiskManager::getPartitionSize(string partitionName)
{
  BlkNumType size = 0;
  try{size = findPart(partitionName)->partitionSize;}
  catch(...)
  {
    cerr << "error DiskManager::getPartitionSize" << endl;
  }
  return size;
}

DiskPartition* DiskManager::findPart(string partitionName)
{
  struct DiskPartition temp{partitionName, 0};
  auto it = find(_myPartitions.begin(), _myPartitions.end(), temp);
  if(it == _myPartitions.end())
  {
    //throw error
    cerr << "DiskManager::findPart" << endl;
  }
  return *it;
}

int DiskManager::getBlockSize()
{return myDisk->getBlockSize();}