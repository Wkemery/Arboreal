#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "disk.h"

using namespace std;

Disk::Disk(BlkNumType numblocks, size_t blksz, char *fname)
{
  blkCount = numblocks;
  diskSize = numblocks * blksz;
  blkSize = blksz;
  diskFilename = strdup(fname);
}

Disk::~Disk()
{
  delete diskFilename;
}

void Disk::readDiskBlock(BlkNumType blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= blkCount)) throw invalid_arg("Blocknumber is out bounds", "Disk::readDiskBlock");
  ifstream f(diskFilename, ios::binary | ios::in);
  if (!f) throw disk_error("Unable to open Disk", "Disk::readDiskBlock");
  f.seekg(blknum * blkSize);
  if(f.bad()) throw disk_error("Disk Seek Error", "Disk::readDiskBlock");
  
  f.read(blkdata, blkSize);
  if(f.bad()) throw disk_error("Disk Read Error", "Disk::readDiskBlock");
  
  f.close();
}

void Disk::writeDiskBlock(BlkNumType blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= blkCount)) throw invalid_arg("Blocknumber is out bounds", "Disk::writeDiskBlock");
  ofstream f(diskFilename, ios::binary | ios::in | ios::out );
  if (!f) throw disk_error("Unable to open Disk", "Disk::writeDiskBlock");
  f.seekp(blknum * blkSize);
  if(f.bad()) throw disk_error("Disk Seek Error", "Disk::writeDiskBlock");
  f.write(blkdata, blkSize);
  if(f.bad()) throw disk_error("Disk Write Error", "Disk::writeDiskBlock");
  f.close();
}
