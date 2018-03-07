
// #include<unordered_map>
#include "../types.h"

#ifndef DISK_H
#define DISK_H


class Disk {
private:

  BlkNumType _diskSize;
  size_t _blockSize;
  BlkNumType _blockCount;
  char *_diskLocation;
public:
  /*!
   * @param numblocks the number of blocks on the Disk
   * @param blocksize the block size for Disk blocks
   * @param location the location of the Disk
   */
  Disk(BlkNumType numblocks, size_t blockSize, char *location);
  ~Disk();
  
  /*!
   * Reads a block from the Disk. 
  * @param blknum the blocknumber to be read
  * @param blkdata the buffer to put the read data. must be large enough to contain an entire block of data
  */
  void readDiskBlock(BlkNumType blknum, char *blkdata);
  
  /*!
   * Writes a block to the Disk. 
   * @param blknum the blocknumber to be written
   * @param blkdata the buffer to write the data from. It Will write an entire block size of data.
   */
  void writeDiskBlock(BlkNumType blknum, char *blkdata);
  
  /*!
   * @returns the blocksize of the Disk
  */
  size_t getBlockSize();
  
  /*!
   * @returns the number of blocks on the entire Disk
  */
  int getBlockCount();
};

#endif
