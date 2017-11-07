/*Trees.h
 * Arboreal
 * November 2017
 */

#include<unordered_map>
#include<string>
using namespace std;

#ifndef TREES_H
#define TREES_H

#define NOTUNIQUE 5000

typedef unsigned long long BlkNumType;

class FileInfo
{
private:
  BlkNumType _fidentifier;
  string _fileName;
  unordered_map<string, BlkNumType> _tags;
public:
  FileInfo();
  ~FileInfo();
  BlkNumType getFidentifier();
  string getFilename();
  void delTag(string tagName);
  void writeOut();
  void readIn();
  void addTag(string tagName, int blocknum);
  unordered_map<string, BlkNumType>* getTags();
};

class TagTree
{
private:
  unordered_map<string, FileInfo*> _tree;
  BlkNumType _blockNumber;
public:
  TagTree();
  ~TagTree();
  TagTree(int blocknum);
  unordered_map<string, FileInfo*>* getTree();
  BlkNumType getBlockNum();
  void writeOut();
  void readIn();
  void zeroDisk();
};








#endif