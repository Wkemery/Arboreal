/*Trees.h
 * Arboreal
 * November 2017
 */

#include "types.h"


#include<unordered_map>
#include<string>
#include"partitionmanager.h"
#include<queue>
#include<map>
#include<algorithm>
using namespace std;

#ifndef TREES_H
#define TREES_H
class FileInfo;
class TreeObject;

struct index
{
  BlkNumType blknum;
  unsigned int offset;
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

bool operator ==(Index& lhs, Index& rhs);
bool operator !=(Index& lhs, Index& rhs);

class Attributes
{
public:
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void del(PartitionManager* pm);
};


class Modification
{
protected:
  TreeObject* _mod;
  TreeObject* _parent;
  Modification(TreeObject* obj, TreeObject* parent);
public:
  virtual void writeOut(PartitionManager* pm) = 0;
};

class Addition : public Modification
{
public:
  Addition(TreeObject* obj, TreeObject* parent);
  void writeOut (PartitionManager* pm);
};

class Deletion : public Modification
{
public:
  Deletion(TreeObject* obj, TreeObject* parent);
  void writeOut (PartitionManager* pm);
};

//TODO: deleteContBlocks may need to be overriden in derived classes
class TreeObject
{
protected:
  queue<Modification*> _modifications;
  TreeObject(string name, BlkNumType blknum);
  string _name;
  BlkNumType _blockNumber;
  Index _index;
  Index _lastEntry; //points at the last entry
  BlkNumType _startBlock;
  unordered_map<TreeObject*, bool> _readable;
public:
  virtual ~TreeObject();
  virtual void writeOut(PartitionManager* pm) = 0;
  virtual void readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles) = 0;
  virtual void del(PartitionManager* pm) = 0;
  bool isRead(TreeObject*);
  void setRead(TreeObject*);
  string getName();
  Index getIndex();
  void setIndex(Index index);
  void setLastEntry(Index index);
  Index getLastEntry();
  BlkNumType getStartBlock();
  BlkNumType getBlockNumber();
  void incrementAllocate(Index* index, PartitionManager* pm);
  void incrementFollow(Index* index, PartitionManager* pm);
  void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  /* deleteContBlocks will take a blknum and free it. it will follow the chain 
   * of continuation blocks and free all of them too*/
protected:
  
};

class FileInfo : public TreeObject
{
private:
  map<string, BlkNumType> _tags;
  Attributes _myAttributes;
  Finode _myFinode;
public:
  FileInfo(string filename, BlkNumType blknum);
  ~FileInfo();
  map<string, BlkNumType>* getMap();
  string mangle();
  string mangle(vector<string>& tags);
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles);
  void del(PartitionManager* pm);
//   void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);

};


class TagTree : public TreeObject
{
private:
  unordered_map<string, FileInfo*> _tree;
//   queue<FileInfo*> _additions;
//   unordered_multimap<BlkNumType, FileInfo*> _deletions;
public:
  TagTree(string tagName, BlkNumType blknum);
  ~TagTree();
  unordered_map<string, FileInfo*>* getMap();
  void insertAddition(FileInfo* file);
  void insertDeletion(FileInfo* file);
  void writeOutAdds(PartitionManager* pm);
  void writeOutDels(PartitionManager* pm);
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles);
//   void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  void del(PartitionManager* pm);
  /*del() will remove the tag tree's presence on disk. That includes removing all 
   * continuation blocks and returning them to the disk.*/
};

class RootTree : public TreeObject
{
private:
  unordered_map<string, TagTree*> _tree;
//   queue<TagTree*> _additions;
//   unordered_multimap<BlkNumType, TagTree*> _deletions;
public:
  RootTree();
  ~RootTree();
  unordered_map<string, TagTree*>* getMap();
  void insertAddition(TagTree* tag);
  void insertDeletion(TagTree* tag);
//   void writeOutAdds(PartitionManager* pm);
  void writeOutDels(PartitionManager* pm);
  /*Function Overrides*/
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles);
//   void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  void del(PartitionManager* pm);//TODO: maybe unnecesary?
  
};

#endif