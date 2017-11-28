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
{
  //TODO:stub
}

void FileInfo::readIn()
{
  //TODO:stub
}

void FileInfo::del()
{}

/******************************************************************************/

TagTree::TagTree(BlkNumType blocknum): _blockNumber(blocknum)
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut()
{
  //TODO:stub
}

void TagTree::readIn()
{
  //TODO:stub
}

void TagTree::zeroDisk()
{}

/******************************************************************************/


bool operator==(const pair<string, unsigned int>& lhs, const pair<string, unsigned int>& rhs)
{
  return (lhs.first == rhs.first);
}

size_t PairHash::operator()(const pair<string, unsigned int>& k) const
{
  return hash<string>()(k.first);
}