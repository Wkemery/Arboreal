


#include<unordered_map>
#include "types.h"

#ifndef DISK_H
#define DISK_H

class Disk {
  public:

  BlkNumType diskSize;
  int blkSize;
  BlkNumType blkCount;
  char *diskFilename;

  Disk(BlkNumType numblocks, int blksz, char *fname);
    ~Disk();
    void readDiskBlock(BlkNumType blknum, char *blkdata);
    void writeDiskBlock(BlkNumType blknum, char *blkdata);
    int getBlockSize() {return (blkSize);};
    int getBlockCount() {return (blkCount);};
};

#endif
