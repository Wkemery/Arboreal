
// #include<unordered_map>
#include "types.h"

#ifndef DISK_H
#define DISK_H


class Disk {
  public:

  BlkNumType diskSize;
  size_t blkSize;
  BlkNumType blkCount;
  char *diskFilename;

  Disk(BlkNumType numblocks, size_t blksz, char *fname);
    ~Disk();
    void readDiskBlock(BlkNumType blknum, char *blkdata);
    void writeDiskBlock(BlkNumType blknum, char *blkdata);
    size_t getBlockSize() {return (blkSize);};
    int getBlockCount() {return (blkCount);};
};

#endif
