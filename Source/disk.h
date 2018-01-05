#include "types.h"
#include "Arboreal_Exceptions.h"

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
    void readDiskBlock(BlkNumType blknum, char *blkdata);
    /*
     * Throws: invalid_arg, disk_error
     */
    void writeDiskBlock(BlkNumType blknum, char *blkdata);
    /*
     * Throws: invalid_arg, disk_error
     */
    int getBlockSize() {return (blkSize);};
    int getBlockCount() {return (blkCount);};
};

#endif
