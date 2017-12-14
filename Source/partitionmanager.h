#include "disk.h"
#include "diskmanager.h"
#include "types.h"
#ifndef PARTMANAGER_H
#define PARTMANAGER_H
class PartitionManager {
  //TODO: modify functions to throw
private:
  string _partitionName;
  BlkNumType _partitionSize;//in blocks
  BlkNumType _freeBlockStart;
  BlkNumType _freeBlockEnd;
  BlkNumType _partitionBlkStart;
  DiskManager *myDM;
public:
  PartitionManager(DiskManager *dm, string partitionName);
  ~PartitionManager();
  void readDiskBlock(BlkNumType blknum, char *blkdata);
  void writeDiskBlock(BlkNumType blknum, char *blkdata);
  int getBlockSize();
  BlkNumType getFreeDiskBlock();
  void returnDiskBlock(BlkNumType blknum);
  string getPartitionName();
  int getFileNameSize();
  bool operator==(const PartitionManager& rhs);
  
    
};
#endif
