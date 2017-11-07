/*Trees.cpp
 * Arboreal
 * November 2017
 */


#include "Trees.h"
#include<string>
#include<unordered_map>
using namespace std;

FileInfo::FileInfo(string filename, BlkNumType fidentifier):_fileName(filename),_fidentifier(fidentifier) 
{}


BlkNumType FileInfo::getFidentifier(){ return _fidentifier;}

string FileInfo::getFilename() {return _fileName;}

unordered_map<string, BlkNumType>* FileInfo::getTags()
{
  return &_tags;
}

void FileInfo::writeOut()
{}

void FileInfo::readIn()
{}

void FileInfo::del()
{}

/******************************************************************************/

TagTree::TagTree(BlkNumType blocknum): _blockNumber(blocknum)
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut()
{}

void TagTree::readIn()
{}

void TagTree::zeroDisk()
{}

/******************************************************************************/


bool operator==(const pair<string, unsigned int>& lhs, const pair<string, unsigned int>& rhs)
{
  return (lhs->first == rhs->first);
}

PairHash::size_t operator()(const pair<int, int> &k) const{
  return k.first * 100 + k.second;
}