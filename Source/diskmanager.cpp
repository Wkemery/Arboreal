#include "diskmanager.h"
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>
using namespace std;

DiskManager::DiskManager(Disk *d, int partcount, DiskPartition *dp)
{
  myDisk = d;
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));
  
  /* Read superblock from disk*/
  int ret = myDisk->readDiskBlock(0, buffer); 
  
  if(ret == -1)
  {
    /* Could not read from disk, initialize disk */
    int r = myDisk->initDisk();
    if(r == -1 ) exit(1);
    
    /* Set private vars*/
    partCount = partcount;
    diskP = new DiskPartition[partCount];
    for(int i = 0; i < partCount; i++)
    {
      diskP[i].partitionName = dp[i].partitionName;
      diskP[i].partitionSize = dp[i].partitionSize;      
    }
    
    /* Disk is initialized, write out partition info*/
    
    /* Write out partcount (first 4 bytes)*/
    intToChar(0, partCount, buffer); 
    int bufferPos = 4;
    
    /* Write out partition information, name(1 byte), part size(4 bytes)*/
    for(int i = 0; i < partCount; i++)
    {
      buffer[bufferPos] = diskP[i].partitionName;
      bufferPos++;
      intToChar(bufferPos, diskP[i].partitionSize, buffer);
      bufferPos+=4;
    }
    
    /* Now write out buffer to superblock*/
    ret = myDisk->writeDiskBlock(0, buffer);
    if(ret == -1)
    {
      cerr << "Disk unexpectedly moved or deleted" << endl;
      //TODO:Note could try to recreate disk here in the future...
      exit(1);
    }
    else if(ret == -2)
    {
      /* Something is really messed up, if block 0 was out of bounds*/
      cerr << "Unknown Error!" << endl;
      exit(1);
    }
  }
  else if(ret == 0)
  {
    /*successfully read superblock, read in partition info, ignore driver partition info*/
    /* first 4 bytes is number of partitions. next byte is partition name then next 4 bytes is size (0 padded) */
    partCount = charToInt(0, buffer);
    diskP = new DiskPartition[partCount];
    
    /* Read in partition information*/
    int bufferPos = 4;
    for(int i = 0; i < partCount; i++)
    {
      diskP[i].partitionName = buffer[bufferPos];
      bufferPos++;
      diskP[i].partitionSize = charToInt(bufferPos, buffer);
      bufferPos+=4;
    }
    
  }
  else
  {
    /* Something is really messed up, if reading from block 0 was out of bounds*/
    cerr << "Unknown Error!" << endl;
    exit(1);
  }
}

DiskManager::~DiskManager()
{
  delete diskP;
}

/*
 *   returns: 
 *   0, if the block is successfully read;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds; (same as disk)
 *  -3 if partition doesn't exist
 */
int DiskManager::readDiskBlock(char partitionname, int blknum, char *blkdata)
{
  /* find partition index in diskP*/
  int index = findPart(partitionname);
  if(index == -1) return -1; //if partition doesn't exist
  
  /* translate relative block number to absolute block number*/
  int absBlockNum = blockOffset(index, blknum);
  if(absBlockNum == -1) return -2;
  
  /* read block data from block using disk read command*/
  int ret = myDisk->readDiskBlock(absBlockNum, blkdata);
  return ret;
  
}

/*
 *   returns: 
 *   0, if the block is successfully written;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds;  (same as disk)
 *  -3 if partition doesn't exist
 */
int DiskManager::writeDiskBlock(char partitionname, int blknum, char *blkdata)
{
  /* find partition index in diskP */
  int index = findPart(partitionname);
  if(index == -1) return -3; //if partition doesn't exist
  
  /* translate relative block number to absolute block number */
  int absBlockNum = blockOffset(index, blknum);
  if(absBlockNum == -1) return -2;
  
  /* write blkdata to block number using disk write command*/
  int ret = myDisk->writeDiskBlock(absBlockNum, blkdata);
  return ret;
  
}

/*
 * return size of partition
 * -1 if partition doesn't exist.
 */
int DiskManager::getPartitionSize(char partitionname)
{
  int index = findPart(partitionname);
  if(index == -1) return -1;
  return diskP[index].partitionSize;
  
}

int DiskManager::findPart(char partitionname)
{
  for(int i = 0; i < partCount; i++)
  {
    if(diskP[i].partitionName == partitionname) return i;
  }
  
  return -1;
}

int DiskManager::blockOffset(int index, int blknum)
{
  if((blknum >= diskP[index].partitionSize) || (blknum < 0)) return -1;
  
  int startBlock = 1;
  for(int i = 0; i < index ; i++)
  {
    startBlock+=diskP[i].partitionSize;
  }
  int absBlockNum = startBlock + blknum;
  return absBlockNum;
}

int charToInt(int pos, char * buff) 
{
  char temp[5];
  memset(temp, 0, sizeof(temp));
  memcpy(temp, buff+pos, sizeof(char) * 4);
  return atoi(temp);
}

void intToChar(int pos, int num, char * buff) 
{
  char temp[5];
  memset(temp , 0, sizeof(temp));
  snprintf(temp , 5, "%04d", num);
  memcpy(buff+pos, temp, sizeof(char) * 4);
}