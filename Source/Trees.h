/*Trees.h
 * Arboreal
 * November 2017
 */

#include<unordered_map>
#include<string>
#include"hash.hpp"
using namespace std;
typedef unsigned long long BlkNumType;

#ifndef TREES_H
#define TREES_H

// bool operator==(const pair<string, unsigned int>& lhs, const pair<string, unsigned int>& rhs);

// class PairHash
// {
// public:
//   size_t operator()(const pair<int, int> &k) const;
// };


class FileInfo
{
private:
  string _fileName;
  BlkNumType _fidentifier;
  unordered_map<string, BlkNumType> _tags;
  unordered_map<pair<string, unsigned int>, BlkNumType, boost::hash<pair<int, int>>> test;
public:
  FileInfo(string filename, BlkNumType fidentifier);
  BlkNumType getFidentifier();
  string getFilename();
  unordered_map<string, BlkNumType>* getTags();
  void writeOut();
  void readIn();
  void del();
};


class TagTree
{
private:
  unordered_map<string, FileInfo*> _tree;
  BlkNumType _blockNumber;
public:
  TagTree(BlkNumType blocknum);
  unordered_map<string, FileInfo*>* getTree();
  BlkNumType getBlockNum();
  void writeOut();
  void readIn();
  void zeroDisk();
};


#endif