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

class TreeObject
{
protected:
  queue<Modification*> _modifications;
  string _name;/*This TreeObject's name*/
  BlkNumType _blockNumber; /*The Blocknumber of the Super block associated with this TreeObject on disk*/
  unordered_map<TreeObject*, Index> _indeces;/*The location(s) of this TreeObject's superblock entry(s) on disk*/
  Index _lastEntry; /*The Index of the last entry of this TreeObject's data on disk*/
  BlkNumType _startBlock; /*The blocknumber of the start of this TreeObject's data on disk*/
  PartitionManager* _myPartitionManager;
  unordered_map<string, TreeObject*> _myTree;
public:
/*Constructors*/
  virtual ~TreeObject();
  TreeObject(string name, BlkNumType blknum, PartitionManager* pm);
  
/*Accessor Functions*/
  string getName();
  BlkNumType getBlockNumber();
  Index getIndex(TreeObject* obj);
  Index getLastEntry();
  BlkNumType getStartBlock();
  size_t size();
  unordered_map<string, TreeObject*>::iterator begin();
  unordered_map<string, TreeObject*>::iterator end();
  TreeObject* find(string name);
  

/*Modifier Functions*/
  void setName(string name);
  void addIndex(TreeObject* obj, Index index);
  void setLastEntry(Index index);
  virtual void insert(string name, TreeObject* ptr);
  void erase(string name);
  void erase(unordered_map<string, TreeObject*>::iterator item);
  virtual void insertAddition(TreeObject* add);
  virtual void insertDeletion(TreeObject* del);

/*Disk Functions*/
  virtual void writeOut() = 0;
  virtual void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree) = 0;
  virtual void del() = 0;
    /*This will completely remove the TreeObject's presence on disk.*/
    
  void incrementAllocate(Index* index);
  void incrementFollow(Index* index);
  
/*Helper Functions*/
  virtual void deleteContBlocks(BlkNumType blknum);
  /* deleteContBlocks will take a blknum and free it. it will follow the chain 
   * of continuation blocks and free all of them too*/
};

class FileInfo : public TreeObject
{
private:
  Attributes _myAttributes;
  Finode _myFinode;
public:
  FileInfo(string filename, BlkNumType blknum, PartitionManager* pm);
  ~FileInfo();
  string mangle();
  string mangle(vector<string>& tags);
  
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void insert(string name, TreeObject* ptr);
  void del();
  void deleteContBlocks(BlkNumType blknum);
  void insertAddition(TreeObject* add);
  void insertDeletion(TreeObject* del);
    /*This will delete all the blocks with file data starting from any level*/
};

class TagTree : public TreeObject
{
public:
  TagTree(string tagName, BlkNumType blknum, PartitionManager* pm);
  ~TagTree();
  
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
};

class RootTree : public TreeObject
{
public:
  RootTree(PartitionManager* pm);
  ~RootTree();
  
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
  
};

#endif