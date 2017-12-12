#include "types.h"


#ifndef DISK_H
#define DISK_H
using namespace std; 

class Disk {
  public:

  BlkNumType diskSize;
  int blkSize;
  BlkNumType blkCount;
  char *diskFilename;

  Disk(BlkNumType numblocks, int blksz, char *fname);
    ~Disk();
    int initDisk();
    int readDiskBlock(BlkNumType blknum, char *blkdata);
    int writeDiskBlock(BlkNumType blknum, char *blkdata);
    int getBlockSize() {return (blkSize);};
    int getBlockCount() {return (blkCount);};
};

#endif
