/*Trees.h
 * Arboreal
 * November 2017
 */

#include "types.h"


#include<unordered_map>
#include<string>
#include"partitionmanager.h"
using namespace std;

#ifndef TREES_H
#define TREES_H
struct index
{
  BlkNumType blknum;
  unsigned int offset;
};
bool operator ==(Index& lhs, Index& rhs);
bool operator !=(Index& lhs, Index& rhs);

class TreeObject
{
protected:
  TreeObject(string name, BlkNumType blknum);
  string _name;
  BlkNumType _blockNumber;
public:
  virtual ~TreeObject();
  virtual void writeOut(PartitionManager* pm) = 0;
  virtual void readIn(PartitionManager* pm) = 0;
  virtual void deleteContBlocks(PartitionManager* pm, BlkNumType blknum) = 0;
  virtual void del(PartitionManager* pm) = 0;
  string getName();
  BlkNumType getBlockNumber();
protected:
  
};

class FileInfo : public TreeObject
{
private:
  unordered_map<string, BlkNumType> _tags;
  Index _index;
public:
  FileInfo(string filename, BlkNumType blknum);
  ~FileInfo();
  unordered_map<string, BlkNumType>* getMap();
  Index* getIndex();
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void del(PartitionManager* pm);
  void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
};


class TagTree : public TreeObject
{
private:
  unordered_map<string, FileInfo*> _tree;
  vector<FileInfo*> _additions;
  unordered_multimap<BlkNumType, FileInfo*> _deletions;
  Index _lastEntry; //points at the last entry
  BlkNumType _startBlock;
public:
  TagTree(string tagName, BlkNumType blknum);
  ~TagTree();
  unordered_map<string, FileInfo*>* getMap();
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  void del(PartitionManager* pm);
  /*del() will remove the tag tree's presence on disk. That includes removing all 
   * continuation blocks and returning them to the disk.*/
};

class RootTree : public TreeObject
{
private:
  unordered_map<string, TagTree*> _tree;
public:
  RootTree();
  ~RootTree();
  unordered_map<string, TagTree*>* getMap();
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  void del(PartitionManager* pm);
  
};

#endif