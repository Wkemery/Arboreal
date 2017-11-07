/*Trees.cpp
 * Arboreal
 * November 2017
 */


#include "Trees.h"
#include<string>
#include<unordered_map>
using namespace std;

FileInfo::FileInfo(){}

FileInfo::~FileInfo(){}

BlkNumType FileInfo::getFidentifier(){ return _fidentifier;}

string FileInfo::getFilename() {return _fileName;}

void FileInfo::delTag(string filename)
{}

void FileInfo::writeOut()
{}

void FileInfo::readIn()
{}

void FileInfo::addTag(string tagName, int blocknum)
{}

unordered_map<string, BlkNumType>* FileInfo::getTags()
{
  return &_tags;
}

/******************************************************************************/

TagTree::TagTree(){}

TagTree::TagTree(int blocknum): _blockNumber(blocknum)
{}

TagTree::~TagTree()
{}

unordered_map<string, FileInfo*>* TagTree::getTree() {return &_tree;}

BlkNumType TagTree::getBlockNum() {return _blockNumber;}

void TagTree::writeOut()
{}

void TagTree::readIn()
{}

void TagTree::zeroDisk()
{}

