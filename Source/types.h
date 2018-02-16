/* types.h
 * Arboreal
 * December 2017
 */
#include<utility>
#include<string>
#include"Arboreal_Exceptions.h"


#ifndef TYPES_H
#define TYPES_H

#define MAXOPENFILES 1000
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