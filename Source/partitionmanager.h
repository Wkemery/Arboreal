#include "disk.h"
#include "diskmanager.h"
#include "bitvector.h"
typedef unsigned long long BlkNumType;
#ifndef PARTMANAGER_H
#define PARTMANAGER_H
class PartitionManager {
  DiskManager *myDM;
  BitVector *dmBV;
  
  //TODO: modify functions to throw
  //TODO: have getFreeDiskBlock return a BlkNumType
  public:
    char myPartitionName;
    BlkNumType myPartitionSize;
    PartitionManager(DiskManager *dm, char partitionname, int partitionsize);
    ~PartitionManager();
    int readDiskBlock(BlkNumType blknum, char *blkdata);
    int writeDiskBlock(BlkNumType blknum, char *blkdata);
    int getBlockSize();
    BlkNumType getFreeDiskBlock();
    int returnDiskBlock(BlkNumType blknum);
};
#endif
