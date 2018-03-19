///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Disk.cpp
//  Disk Source File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "Disk.h"

Disk::Disk(BlkNumType numblocks, size_t blockSize, char *location)
{
  _blockCount = numblocks;
  _diskSize = numblocks * blockSize;
  _blockSize = blockSize;
  _diskLocation = strdup(location);
}

Disk::~Disk()
{
  free(_diskLocation);
}

void Disk::readDiskBlock(BlkNumType blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= _blockCount)) throw invalid_arg("Blocknumber is out bounds", "Disk::readDiskBlock");
  ifstream f(_diskLocation, ios::binary | ios::in);
  if (!f) throw disk_error("Unable to open Disk", "Disk::readDiskBlock");
  f.seekg(blknum * _blockSize);
  if(f.bad()) throw disk_error("Disk Seek Error", "Disk::readDiskBlock");
  
  f.read(blkdata, _blockSize);
  if(f.bad()) throw disk_error("Disk Read Error", "Disk::readDiskBlock");
  
  f.close();
}

void Disk::writeDiskBlock(BlkNumType blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= _blockCount)) throw invalid_arg("Blocknumber is out bounds", "Disk::writeDiskBlock");
  ofstream f(_diskLocation, ios::binary | ios::in | ios::out );
  if (!f) throw disk_error("Unable to open Disk", "Disk::writeDiskBlock");
  f.seekp(blknum * _blockSize);
  if(f.bad()) throw disk_error("Disk Seek Error", "Disk::writeDiskBlock");
  f.write(blkdata, _blockSize);
  if(f.bad()) throw disk_error("Disk Write Error", "Disk::writeDiskBlock");
  f.close();
}


size_t Disk::getBlockSize() {return (_blockSize);}
int Disk::getBlockCount() {return (_blockCount);}