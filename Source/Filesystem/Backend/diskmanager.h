#include "disk.h"
#include "../types.h"
#include<vector>

using std::vector;
#ifndef DISKMANAGER_H
#define DISKMANAGER_H
;

struct DiskPartition
{
  string partitionName;
  BlkNumType partitionSize;
  BlkNumType partitionBlkStart;
  int fileNameSize;
  
};

bool operator==(const DiskPartition* lhs, const DiskPartition& rhs);


class DiskManager {
private:
  Disk *myDisk;
  vector<DiskPartition*> _myPartitions;
  
  public:
    DiskManager(Disk *d);
    ~DiskManager();
    void readDiskBlock(string partitionName, BlkNumType blknum, char *blkdata);
    void writeDiskBlock(string partitionName, BlkNumType blknum, char *blkdata);
    size_t getBlockSize();
    BlkNumType getPartitionSize(string partitionName);
    DiskPartition* findPart(string partitionName);
    
};

#endif

