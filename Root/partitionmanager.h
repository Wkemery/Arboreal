

#include "disk.h"
#include "diskmanager.h"
#include "types.h"


#ifndef PARTMANAGER_H
#define PARTMANAGER_H



class PartitionManager {
private:
  std::string _partitionName;
  BlkNumType _partitionSize;//in blocks
  BlkNumType _freeBlockStart;
  BlkNumType _freeBlockEnd;
  BlkNumType _partitionBlkStart;
  int _fileNameSize;
  DiskManager *myDM;
public:
  PartitionManager(DiskManager *dm, std::string partitionName);
  ~PartitionManager();
  void readDiskBlock(BlkNumType blknum, char *blkdata);
  void writeDiskBlock(BlkNumType blknum, char *blkdata);
  int getBlockSize();
  BlkNumType getFreeDiskBlock();
  void returnDiskBlock(BlkNumType blknum);
  /* Throws: invalid_arg */
  std::string getPartitionName();
  int getFileNameSize();
  bool operator==(const PartitionManager& rhs);
  
    
};
#endif
