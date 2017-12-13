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
  
//   void insert(string tagName, BlkNumType blknum);
//   void erase(string tagName);
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void del(PartitionManager* pm);
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
//   void insert(string fileName, FileInfo* fileInfo);
//   void erase(string fileName);
  void writeOut(PartitionManager* pm);
  void readIn(PartitionManager* pm);
  void deleteContBlocks(PartitionManager* pm, BlkNumType blknum);
  void del(PartitionManager* pm);
  /*del() will remove the tag tree's presence on disk. That includes removing all 
   * continuation blocks and returning them to the disk.*/
};

#endif