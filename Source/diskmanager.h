#include "disk.h"

#ifndef DISKMANAGER_H
#define DISKMANAGER_H
using namespace std;

class DiskPartition {
  public:
    char partitionName;
    int partitionSize;
};

class DiskManager {
  Disk *myDisk;
  int partCount;
  DiskPartition *diskP;

  /* declare other private members here */

  int findPart(char partitionname);
    /* findPart searches diskP for the DiskPartition object with a partition name matching the parameter partitionname. 
     * It return the index of the object if found. If not found, it returns -1. */
  int blockOffset(int index, int blknum);
    /* blockOffset takes the index of the desired partition and the block number relative to that partition. It converts
     * the relative block number to its corresponding absolute block number on the disk. It returns the absolute block 
     * number and returns -1 if the absolute block number is out of bounds for that partition. */
  
  public:
    DiskManager(Disk *d, int partCount, DiskPartition *dp);
    ~DiskManager();
    int readDiskBlock(char partitionname, int blknum, char *blkdata);
    int writeDiskBlock(char partitionname, int blknum, char *blkdata);
    int getBlockSize() {return myDisk->getBlockSize();};
    int getPartitionSize(char partitionname);
};

extern void intToChar(int pos, int num, char * buff);
extern int charToInt(int pos, char * buff);  

#endif

