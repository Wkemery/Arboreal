#include "disk.h"
#include "diskmanager.h"
#include "types.h"
#ifndef PARTMANAGER_H
#define PARTMANAGER_H
class PartitionManager {
private:
  string _partitionName;
  BlkNumType _partitionSize;//in blocks
  BlkNumType _freeBlockStart;
  BlkNumType _freeBlockEnd;
  BlkNumType _partitionBlkStart;
  int _fileNameSize;
  DiskManager *myDM;
public:
  PartitionManager(DiskManager *dm, string partitionName);
  ~PartitionManager();
  void readDiskBlock(BlkNumType blknum, char *blkdata);
  void writeDiskBlock(BlkNumType blknum, char *blkdata);
  size_t getBlockSize();
  BlkNumType getFreeDiskBlock();
  void returnDiskBlock(BlkNumType blknum);
  /* Throws: invalid_arg */
  string getPartitionName();
  int getFileNameSize();
  bool operator==(const PartitionManager& rhs);
  
    
};
#endif
