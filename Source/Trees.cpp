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

void FileInfo::writeOut(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
  //filename - 64 bytes
  // fidentifier - probably 8 bytes
  //file size
  
}

void FileInfo::readIn(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
}

void FileInfo::del()
{}

/******************************************************************************/

TagTree::TagTree(BlkNumType blocknum): _blockNumber(blocknum)
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  //for every entry in the _tree 
    // write out key to buffer. pad to 64 bytes 
    //write out the blocknumber of the fidentifier to buffer
  
  //write out buffer to my _blockNumber
}

void TagTree::readIn(PartitionManager* pm)
{
  //TODO:stub
  //TODO: implement block continuation
  
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