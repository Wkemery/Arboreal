#include "disk.h"
#include "diskmanager.h"
#include "bitvector.h"

#ifndef PARTMANAGER_H
#define PARTMANAGER_H
class PartitionManager {
  DiskManager *myDM;
  BitVector *dmBV;
  
  public:
    char myPartitionName;
    int myPartitionSize;
    PartitionManager(DiskManager *dm, char partitionname, int partitionsize);
    ~PartitionManager();
    int readDiskBlock(int blknum, char *blkdata);
    int writeDiskBlock(int blknum, char *blkdata);
    int getBlockSize();
    int getFreeDiskBlock();
    int returnDiskBlock(int blknum);
};
#endif
