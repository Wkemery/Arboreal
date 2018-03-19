/* types.h
 * Arboreal
 * December 2017
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <cstring>
#include <queue>
#include <vector>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <time.h>
#include"../../../SharedHeaders/Arboreal_Exceptions.h"


#ifndef TYPES_H
#define TYPES_H

#define MAXopen_fileS 1000
extern bool DEBUG;

typedef unsigned long BlkNumType;
typedef struct index Index;
typedef struct rootSuperBlock RootSuperBlock;
typedef struct tagTreeSuperBlock TagTreeSuperBlock;
typedef struct finode Finode;
typedef struct file_attributes FileAttributes;

struct index
{
  BlkNumType blknum;
  size_t offset;
};

struct rootSuperBlock 
{
  size_t size;
  Index lastEntry;
  BlkNumType startBlock;
};

struct tagTreeSuperBlock
{
  size_t size;
  Index lastEntry;
  BlkNumType startBlock;
};

struct file_attributes
{
  time_t creationTime;
  time_t lastAccess;
  time_t lastEdit;
  size_t size;
  char permissions[12];
  int owner;
};

struct finode
{
  BlkNumType attributes;
  BlkNumType directBlocks[12];
  BlkNumType level1Indirect;
  BlkNumType level2Indirect;
  BlkNumType level3Indirect;
};

class TreeObject;
class FileInfo;
class TagTree;
class RootTree;
class Attributes;
class Modification;
class Addition;
class Deletion;

#endif