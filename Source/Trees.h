/*Trees.h
 * Arboreal
 * November 2017
 */

#include<unordered_map>
#include<string>

using namespace std;
typedef unsigned long long BlkNumType;

#ifndef TREES_H
#define TREES_H
class FileInfo
{
private:
  string _fileName;
  BlkNumType _fidentifier;
  unordered_map<string, BlkNumType> _tags;
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