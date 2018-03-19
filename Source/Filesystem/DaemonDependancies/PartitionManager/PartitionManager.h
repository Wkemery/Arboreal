///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PartitionManager.h
//  Partition Manager Header File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "../Types/types.h"
#include "../DiskManager/DiskManager.h"
using std::cerr;
using std::string;
using std::endl;

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
  DiskManager *_myDM;
public:
  /*!
   * @param dm the DiskManager associated with this object
   * @param partitionName the name of the partition that this will be managing
   */
  PartitionManager(DiskManager *dm, string partitionName);
  ~PartitionManager();
  
  /** @name Accessor Functions
   */
  ///@{ 
  
  
  /*!
   * Reads a block from the Disk. 
   * @param blknum the blocknumber to be read
   * @param blkdata the buffer to put the read data. must be large enough to contain an entire block of data
   */
  void readDiskBlock(BlkNumType blknum, char *blkdata);

  /*!
   * @returns the blocksize of the Disk
   */
  size_t getBlockSize();
  
  /*!
   * @returns The name of the partition this PartitionManager is associated with
   */
  string getPartitionName();
  
  /*!
   * @returns The maximum file name size for this partition in bytes
   */
  int get_file_name_size();
  
  ///@}
  
  /** @name Modifier Functions
   */
  ///@{ 
  
  
  /*!
   * Writes a block to the Disk. 
   * @param blknum the blocknumber to be written
   * @param blkdata the buffer to write the data from. It Will write an entire block size of data.
   */
  void writeDiskBlock(BlkNumType blknum, char *blkdata);
  
  /*!
   * Allocates a block on disk if there is a free one. The Disk free list is updated accordingly
   * @returns the block number of the newly allocated block
   */
  BlkNumType getFreeDiskBlock();
  
  /*!
   * returns a block to the Disk free list and zeros it out before writing.
   * @param blknum the blocknumber of the block to be freed
   */
  void returnDiskBlock(BlkNumType blknum);
  
  ///@}
};
#endif
