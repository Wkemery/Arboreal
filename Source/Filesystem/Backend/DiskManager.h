///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DiskManager.h
//  DiskManager Header File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "Disk.h"
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
  Disk *_myDisk;
  vector<DiskPartition*> _myPartitions;
  
public:
  /*!
   * @param d Pointer to the Disk this will manage
   */
  DiskManager(Disk *d);
  ~DiskManager();
  
  /** @name Accessor Functions
   */
  ///@{ 
  
  /*!
   * Reads a block from the Disk. 
   * @param partitionName the name of the partition to write the block to
   * @param blknum the blocknumber to be read
   * @param blkdata the buffer to put the read data. must be large enough to contain an entire block of data
   * @sa PartitionManger::readDiskBlock() ParitionManager::readDiskBlock()
   */
  void readDiskBlock(string partitionName, BlkNumType blknum, char *blkdata);
  
  /*!
   * @returns the blocksize of the Disk
   */
  size_t getBlockSize();
  
  /*!
   * @param partitionName the name of the partition
   * @returns the size of a partition in blocks
   */
  BlkNumType getPartitionSize(string partitionName);
  
  /*!
    * @param partitionName the name of the partition
    * @returns the size of a partition in blocks
    */
  DiskPartition* findPart(string partitionName);
    
  ///@}
  
  /** @name Modifier Functions
    */
  ///@{ 
  
  
  /*!
    * Writes a block to the Disk. 
    * @param partitionName the name of the partition to write the block to
    * @param blknum the blocknumber to be written
    * @param blkdata the buffer to write the data from. It Will write an entire block size of data.
    * @sa PartitionManger::writeDiskBlock() ParitionManager::writeDiskBlock()
    */
  void writeDiskBlock(string partitionName, BlkNumType blknum, char *blkdata);
  
  
  ///@}
  
};

#endif

