/*Trees.h
 * Arboreal
 * November 2017
 */

#include<unordered_map>
#include<string>
#include"partitionmanager.h"
using namespace std;
typedef unsigned long long BlkNumType;
typedef pair<string, unsigned int> TagTuple;
#ifndef TREES_H
#define TREES_H

bool operator==(const pair<string, unsigned int>& lhs, const pair<string, unsigned int>& rhs);
class PairHash
{
public:
  size_t operator()(const pair<string, unsigned int>& k) const;
};

class FileInfo
{
private:
  string _fileName;
  BlkNumType _fidentifier;
  unordered_map<string, BlkNumType> _tags;
public:
  FileInfo(string filename, BlkNumType blknum);
  BlkNumType getFidentifier();
  string getFilename();
  unordered_map<string, BlkNumType>* getTags();
  
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void del();
};


class TagTree
{
private:
  unordered_map<string, FileInfo*> _tree;
  BlkNumType _blockNumber;
public:
  TagTree(BlkNumType blknum);
  unordered_map<string, FileInfo*>* getTree();
  BlkNumType getBlockNum();
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void zeroDisk();
};


#endif