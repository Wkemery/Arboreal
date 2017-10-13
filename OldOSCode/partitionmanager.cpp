#include "partitionmanager.h"
#include <iostream>
#include <string.h>
using namespace std;


PartitionManager::PartitionManager(DiskManager *dm, char partitionname, int partitionsize)
{
  myDM = dm;
  myPartitionName = partitionname;
  myPartitionSize = myDM->getPartitionSize(myPartitionName);

  char buffer[64];
  readDiskBlock(0, buffer);
  int i = 0;
  dmBV = new BitVector(myPartitionSize);
  while (i < 64)
  {
    if (buffer[i] != 'c')
    {
      dmBV->setBitVector((unsigned int *) buffer);
      break;
    }
    i++;
  }
  if (i == 64)
  {
    dmBV->setBit(0);
    dmBV->setBit(1);
  }
}

PartitionManager::~PartitionManager()
{
	delete dmBV;
}

/*
 * return blocknum, -1 otherwise
 */
int PartitionManager::getFreeDiskBlock()
{
  for (int i = 2; i < myPartitionSize; i++)
  {
    if (dmBV->testBit(i) == 0)
    {
      char buffer[64];
      /* getBitVector does not get rid of garbage in buffer. need to zero out*/
      memset(buffer, 0, sizeof(buffer));
      dmBV->setBit(i);
      dmBV->getBitVector((unsigned int *)buffer);
      writeDiskBlock(0, buffer);
      return i;
    }
  }
  return -1;
}

/*
 * return 0 for sucess, -1 otherwise
 */
int PartitionManager::returnDiskBlock(int blknum)
{
  /* write the code for deallocating a partition block */
  /* Prevent deallocating of block 0 or 1*/
  if (blknum < 2 || blknum > myPartitionSize)	return -1;
  dmBV->resetBit(blknum);
  char buffer[64];

  for (int i = 0; i < 64; i++)
  {
    buffer[i] = 'c';
  }
  writeDiskBlock(blknum, buffer);
  memset(buffer, 0, sizeof(buffer));
  dmBV->getBitVector((unsigned int *) buffer);
  writeDiskBlock(0, buffer);
  return 0;
}


int PartitionManager::readDiskBlock(int blknum, char *blkdata)
{
  return myDM->readDiskBlock(myPartitionName, blknum, blkdata);
}

int PartitionManager::writeDiskBlock(int blknum, char *blkdata)
{
  return myDM->writeDiskBlock(myPartitionName, blknum, blkdata);
}

int PartitionManager::getBlockSize()
{
  return myDM->getBlockSize();
}
