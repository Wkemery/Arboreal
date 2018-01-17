/* types.h
 * Arboreal
 * December 2017
 */

#include<utility>
#include<string>
#include"Arboreal_Exceptions.h"

#ifndef TYPES_H
typedef unsigned long BlkNumType;
typedef std::pair<std::string, unsigned int> TagTuple;
typedef struct index Index;
typedef struct rootSuperBlock RootSuperBlock;
typedef struct tagTreeSuperBlock TagTreeSuperBlock;
typedef struct finode Finode;

struct index
{
  BlkNumType blknum;
  long unsigned int offset;
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

#define TYPES_H

#endif