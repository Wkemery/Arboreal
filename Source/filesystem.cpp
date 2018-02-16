/*filesystem.cpp
 * Arboreal
 * October, 12, 2017
 */

#include "filesystem.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <unordered_map>
#include<vector>
#include<string>
using namespace std;
bool EncryptionFlag = false;

FileOpen::FileOpen(FileInfo* file, char mode, PartitionManager* pm): _file(file), _mode(mode), _myPartitionManager(pm)
{
  _seek = 0;
  _EOF = false;
}

FileInfo* FileOpen::getFile(){return _file;}
size_t FileOpen::getSeek(){return _seek;}
char FileOpen::getMode(){return _mode;}
bool FileOpen::getEOF(){return _EOF;}
void FileOpen::gotoLastByte()
{
  _seek = _file->getFileSize();
}

void FileOpen::incrementSeek(size_t bytes, bool write)
{
  if(bytes >= _file->getFileSize() - _seek)
  {
    if(write)
    {
      _seek += bytes;
      _file->updateFileSize(_seek);
    }
    else
    {
      setEOF();
    }
  }
  else
  {
    _seek += bytes;
  }
}

Index FileOpen::byteToIndex(short offset)
{
  if(offset != 0 && offset != 1)
  {
    throw arboreal_logic_error("Provided an offset other than 0 or 1", "FileOpen::byteToIndex()");
  }
  size_t seek = _seek + offset;
  unsigned int entriesPerBlock = _myPartitionManager->getBlockSize() / sizeof(BlkNumType);
  size_t blockIndex = seek / _myPartitionManager->getBlockSize();
  Index ret;
  vector<int> remainders;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  /*Set the return offset*/
  ret.offset = seek % _myPartitionManager->getBlockSize();
  
  if(blockIndex < 12)
  {
    if(_file->getFinode().directBlocks[blockIndex] == 0)
    { 
      /*Space not yet allocated*/
      return Index{0,0};
    }
    ret.blknum = _file->getFinode().directBlocks[blockIndex];
  }
  else
  {
    int levelCount = 0;
    blockIndex -= 12;
    while(blockIndex >= entriesPerBlock)
    {
      blockIndex /= entriesPerBlock;
      remainders.push_back(blockIndex % entriesPerBlock);
      levelCount++;
    }
    switch(levelCount)
    {
      case 0:
      {        
        /*Read in Level 1 indirect block*/
        if(_file->getFinode().level1Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        if(_file->getFinode().level1Indirect == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(_file->getFinode().level1Indirect, buff);
  
        /*blockIndex is the level 1 offest*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (blockIndex * sizeof(BlkNumType)), sizeof(BlkNumType));
        ret.blknum = blknum;
        
        break; 
      }
      case 1:
      {
        /*Read in Level 2 indirect block*/
        if(_file->getFinode().level2Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        if(_file->getFinode().level2Indirect == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(_file->getFinode().level2Indirect, buff);
        
        /*blockIndex - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        if(l1blknum == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(l1blknum, buff);
        
        /*remainders[0] is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (remainders[0] * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        ret.blknum = blknum;
        
        break;
      }
      case 2:
      {        
        /*Read in Level 3 indirect block*/
        if(_file->getFinode().level1Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        
        if(_file->getFinode().level3Indirect == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(_file->getFinode().level3Indirect, buff);
        
        /*blockIndex - 1 is the level 3 offset*/
        BlkNumType l2blknum;
        memcpy(&l2blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in level 2 indirect block*/
        if(l2blknum == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(l2blknum, buff);
        
        /*remainders[1] - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((remainders[1] - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        if(l1blknum == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(l1blknum, buff);
        
        /*remainders[0] is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (remainders[0] * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        ret.blknum = blknum;
        
        break;
      }
      default:
      {
        throw arboreal_logic_error("Invalid level value", "FileOpen::byteToIndex");
      }
    }
  }
  if(buff != 0) delete buff; buff = 0;
  return ret;
}

Index FileOpen::incrementIndex()
{
  /*Assuming this function is only called when the seek is at a multiple of blocksize*/
  int check = (_seek) % _myPartitionManager->getBlockSize();
  if(check != 0)
  {
    throw arboreal_logic_error("Called incrementIndex when seekptr was not at end of a block", "FileOpen::incrementIndex");
  }
  if(_seek != _file->getFileSize())
  {
    /*Assuming this function is only called when the seek is at the last byte of the file*/
    throw arboreal_logic_error("Called incrementIndex when seek pointer was not pointing to last byte of file", "FileOpen::incrementIndex");
  }
  
  Index ret{0,0};
  size_t seek = _file->getFileSize();
  size_t entriesPerBlock = _myPartitionManager->getBlockSize() / sizeof(BlkNumType);
  size_t blockIndex = seek / _myPartitionManager->getBlockSize();
  
  size_t directOffset = 12;
  size_t level1Offset = entriesPerBlock;
  size_t level2Offset = (entriesPerBlock) * level1Offset;
  size_t level3Offset = (entriesPerBlock) * level2Offset;
  
  if (blockIndex < directOffset)
  {
    /*Need to allocate the next direct block*/
    int nextDirectBlockIndex = seek / _myPartitionManager->getBlockSize();
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->addDirectBlock(blknum, nextDirectBlockIndex);
  }
  else if(blockIndex == directOffset)
  {
    /*Need to allocate the level 1 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->addIndirectBlock(blknum, 1);
    
  }
  else if(blockIndex == directOffset + level1Offset)
  {
    /*Need to allocate the Level 2 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->addIndirectBlock(blknum, 2);
  }
  else if(blockIndex == directOffset + level2Offset)
  {
    /*Need to allocate level 3 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->addIndirectBlock(blknum, 3);      
  }
  else if(blockIndex == directOffset + level3Offset)
  {
    throw file_error("Maximum File Size Reached!", "FileOpen::incrementIndex");
  }
  
  int levelCount = 0;
  blockIndex -= 12;
  size_t levelFind = blockIndex;
  while(levelFind >= entriesPerBlock)
  {
    levelFind /= entriesPerBlock;
    levelCount++;
  }
  
  switch(levelCount)
  {
    case 0:
    {
      /*We're in level 1*/
      /*blockIndex is the level 1 relativeBlock*/
      ret.blknum = levelInc(blockIndex, _file->getFinode().level1Indirect, 1);
      break;
    }
    case 1:
    {
      /*We're in level 2*/
      /*blockIndex - (level1Offset + directOffset) is the level 2 relativeBlock*/
      size_t relativeBlock = blockIndex - (level1Offset + directOffset);
      ret.blknum = levelInc(relativeBlock, _file->getFinode().level2Indirect, 2);
      break;
    }
    case 2:
    {
      /*We're in level 3*/
      /*blockIndex - (level2Offset + directOffset) is the level 3 relativeBlock*/
      size_t relativeBlock = blockIndex - (level2Offset + directOffset);
      ret.blknum = levelInc(relativeBlock, _file->getFinode().level3Indirect, 3);
      break;
    }
    default:
    {
      throw arboreal_logic_error("Invalid level Count", "FileOpen::incrementIndex()");
    }
  }
  return ret;
}

BlkNumType FileOpen::levelInc(size_t relativeBlock, BlkNumType ledgerBlock, short level)
{
  int blockSize = _myPartitionManager->getBlockSize();
  char* buff = new char[blockSize];
  
  _myPartitionManager->readDiskBlock(ledgerBlock, buff);
  
  switch(level)
  {
    case 1:
    {
      BlkNumType dataBlock = _myPartitionManager->getFreeDiskBlock();
      memcpy(buff + (relativeBlock * sizeof(BlkNumType)), &dataBlock, sizeof(BlkNumType));
      _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      
      return dataBlock;
    }
    case 2:
    {
      BlkNumType level1Block;
      if(relativeBlock % blockSize == 0)
      {
        level1Block = _myPartitionManager->getFreeDiskBlock();;
        memcpy(buff + (relativeBlock * sizeof(BlkNumType) / blockSize), &level1Block, sizeof(BlkNumType));
        _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      }
      memcpy(&level1Block, buff + (relativeBlock * sizeof(BlkNumType) / blockSize), sizeof(BlkNumType));
      return levelInc(relativeBlock % blockSize, level1Block, level - 1);
    }
    case 3:
    {
      BlkNumType level2Block;
      if(relativeBlock % (blockSize^2) == 0)
      {
        level2Block = _myPartitionManager->getFreeDiskBlock();;
        memcpy(buff + (relativeBlock * sizeof(BlkNumType) / (blockSize^2)), &level2Block, sizeof(BlkNumType));
        _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      }
      memcpy(&level2Block, buff + (relativeBlock * sizeof(BlkNumType) / (blockSize^2)), sizeof(BlkNumType));
      return levelInc(relativeBlock % (blockSize^2), level2Block, level - 1);
    }
    default:
    {
      throw arboreal_logic_error("Invalid level", "FileOpen::levelInc");
    }
  }
  return 0;
}

void FileOpen::setEOF()
{
  _seek = -1;
  _EOF = true;
}

void FileOpen::resetSeek()
{
  _seek = 0;
  _EOF = false;
}

/***********************************************************************/

FileSystem::FileSystem(DiskManager *dm, string fileSystemName)
{
  _FSName = fileSystemName;
  
  /* set partition manager for my partition */
  _myPartitionManager = new PartitionManager(dm, fileSystemName);
  
  /*Read in the root tree*/
  _RootTree = new RootTree(_myPartitionManager);
  _RootTree->readIn(&_allFiles, _RootTree);
  
  /*Read in every tag Tree*/
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    it->second->readIn(&_allFiles, _RootTree);
  }
}

FileSystem::~FileSystem()
{
  delete _myPartitionManager;
}

vector<FileInfo*>* FileSystem::tagSearch(vector<string> tags)
{
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  if(tags.size() == 0)
  {
    throw tag_error("No tags specified to search for", "FileSystem::tagSearch");
  }
  else if(tags.size() == 1)
  {
    /*find the tag in root tree*/
    TreeObject* tagTree = _RootTree->find(tags[0]);
    
    /*List files in tag tree pointed to by root tree*/
    
    for(auto fileIt = tagTree->begin(); fileIt != tagTree->end(); fileIt++)//Complexity: number of files in answer
    {
      ret->push_back((FileInfo*)fileIt->second);
    }
    
  }
  else
  {    
    //   * Use size field in root node of tag tree to find smallest tree among the tags you want to search
    /*create vector of tagtrees that we want to search*/
    vector<TreeObject*> searchTrees;
    for(size_t i = 0; i < tags.size(); i++)
    {
      TreeObject* tagTree = _RootTree->find(tags[i]);
      if(tagTree == 0)
      {
        cerr << tags[i] + " excluded from search : Tag Does not exist" << endl;
      }
      else
      {
        searchTrees.push_back(tagTree);
      }
    }
    
    TreeObject* smallest = searchTrees[0];
    for(size_t i = 1; i < searchTrees.size(); i++)//Complexity: number of tags specified
    {
      if(searchTrees[i]->size() < smallest->size())
      {
        smallest = searchTrees[i];
      }
    }
    
    /*Search the smallest tree:*/
    for(auto fileIt = smallest->begin(); fileIt != smallest->end(); fileIt++)
    {
      /* elimnate all nodes with tag count < the number of tags you are searching for*/
      if(fileIt->second->size() >= tags.size())
      {
        /* could be a matching file*/
        bool match = true;
        /* search remainng Tags for exact tag match*/
        for(size_t i = 0; i < tags.size(); i++)
        {
          if(fileIt->second->find(tags[i]) == 0)
          {
            match = false;
            break;
          }
        }
        if(match)
        {
          /*Push back the matching file*/
          ret->push_back((FileInfo*)fileIt->second);
        }
      }
    }
  }
  
  /* return vector of the found file(s)*/
  return ret;
}

vector<FileInfo*>* FileSystem::fileSearch(string name)
{
  /*We're going to use the _allFiles variable to find the files*/
  
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  
  auto files = _allFiles.equal_range(name);
  
  for (auto it = files.first; it != files.second; it++) {
    ret->push_back(it->second);
  }
  
  return ret;
}

void FileSystem::createTag(string tagName)
{
  /*Get a block from disk to store tag tree super block*/
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  /* initialize tree in main memory */
  TagTree* newTree = new TagTree(tagName, newblknum, _myPartitionManager);
  
  /* add TagTree to root tree */
  _RootTree->insert(tagName, newTree);
  
  /*Note Root Tree was modified*/
  insertModification(_RootTree);
  
  /*Write out newly created TagTree. it will only write out the TagTree superblock*/
  newTree->writeOut();
}

void FileSystem::deleteTag(string tagName)
{
  /*Find tagTree*/
  TreeObject* tagTree = _RootTree->find(tagName);
  if(tagTree == 0)
  {
    throw tag_error(tagName + " Does Not Exist", "FileSystem::deleteTag");
  }
    
  /* CANNOT delete tag if tag tree Size > 0 */
  if(tagTree->size() > 0)
  {
    throw tag_error(tagName + " cannot be deleted: Tag has files associated with it", "FileSystem::deleteTag");
  }
  
  /*Delete tagTree on disk*/
  tagTree->del();
  /*Delete TagTree from Root Tree*/
  _RootTree->erase(tagName);
  /*Note Root Tree was modified*/
  insertModification(_RootTree);
  
}

void FileSystem::mergeTags(string tag1, string tag2)
{
  
  //   - create new tag tree if needed
  //   - Move all Nodes in largest tag tree to new (assuming new tree was created otherwise add to existsing tree) Tree
  //   * delete refrences to old tags in Fionde as you go
  //   - Move Nodes of second Tag Tree:
  //   * check that node is not already in destination tree
  //   * if Yes: Skip
  //   * if NO: Add
  //   * Repete
  
}

void FileSystem::tagFile(FileInfo* file, vector<string>& tags)
{
  /*Validate the tagging of this file first*/
  if(tags.size() == 0) {throw tag_error("No tags specified","FileSystem::tagFile");}
  if(file == 0) {throw file_error ("File Does not Exist", "FileSystem::tagFile");}
  
  vector<string> tagSet;
  TreeObject* tagTree = 0;
  
  for(size_t i = 0; i < tags.size(); i++)
  {
    /*Only add the tags that exist to the tagSet*/
    tagTree = _RootTree->find(tags[i]);
    if(tagTree == 0)
    {
      cerr << tags[i] << " Does Not Exist: Not added to file tag set" << endl;
    }
    else
    {
      tagSet.push_back(tags[i]);
    }
  }
  
  if(tagSet.size() == 0)
  {
    throw tag_error("No valid tags specified: file not tagged", "FileSystem::tagFile");
  }
  
  TreeObject* fileCheck =  tagTree->find(file->mangle(tagSet));
  if(fileCheck != 0)
  {
    throw file_error(file->getName() + " with the specified tags already exists", "FileSystem::tagFile");
  }
  
  /*For every tag in tagSet*/
  for(string tag : tagSet)
  {
    /*find tagTree*/
    tagTree = _RootTree->find(tag);
    
    /*Add Tag to Finode */
    file->insert(tag, tagTree);
    
    /*Add Finode to tagTree*/
    tagTree->insert(file->mangle(tagSet), file);
    
    /*Note TagTree was modified*/
    insertModification(tagTree);
  
  }
  
  /*Remove default tag, it exists in this file*/
  if(file->find("default") != 0)
  {
    file->erase("default");
  }
  
  file->writeOut();
}

void FileSystem::untagFile(FileInfo* file, vector<string>& tags)
{
  for(string tag : tags)
  {
    if(tag == "default")
    {
      cerr << tag + " cannot be removed from " << file->getName() << endl;
    }
    
    /*find tagTree*/
    TreeObject* tagTree = _RootTree->find(tag);
    
    /*If tag does not exist print error and continue*/
    if(tagTree == 0)
    {
      cerr << tag + " cannot be removed from " << file->getName() << " : Tag Does not exist" << endl;
    }
    
    /*Remove tag from Finode*/
    file->erase(tag);
    
    /*Remove Finode from tagTree*/
    tagTree->erase(file->getName());
    
    /*Note Tag Tree was modified*/
    insertModification(tagTree);
  }
  
  /*if removed all Tags from file, add default tag*/
  if(file->size() == 0)
  {
    TreeObject* defaultTree = _RootTree->find("default");
    file->insert("default", defaultTree);
  }
  
  /* write updated Finode to disk*/
  file->writeOut();
}

void FileSystem::renameTag(string originalTagName, string newTagName)
{
  /*Rename the tagTree*/
  TreeObject* tagTree = _RootTree->find(originalTagName);
  tagTree->setName(newTagName);
  
  /*Change tagName in rootTree*/
  _RootTree->erase(originalTagName);
  _RootTree->insert(newTagName, tagTree);
  
  /*Change tagName in every FileInfo object of the tagTree*/
  for(auto fileIt = tagTree->begin(); fileIt != tagTree->end(); fileIt++)
  {
    fileIt->second->erase(originalTagName);
    fileIt->second->insert(newTagName, tagTree);
  }
}

FileInfo* FileSystem::createFile(string filename, vector<string>& tags)
{
  /*Get a block from disk to store Finode*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  FileInfo* newFile = new FileInfo(filename, newblknum, _myPartitionManager);
  /* If no tag was specified then add file to "default" tag tree
   * File remains in default tag tree until a non-default tag is associated with file*/
  if(tags.size() == 0)
  {
    vector<string> temp;
    temp.push_back("default");
    
    tagFile(newFile, temp);
  }
  else
  {
    tagFile(newFile, tags);
  }
  
  newFile->writeOut();
  _allFiles.insert(pair<string, FileInfo*>(filename, newFile));
  
  return newFile;
}

void FileSystem::deleteFile(FileInfo* file)
{
  /*Assuming FileInfo* passed from calling code is valid*/
  vector<string> tags;
  for(auto tagIt = file->begin(); tagIt != file->end(); tagIt++)
  {
    tags.push_back(tagIt->first);
  }
  /*Dissasociate all tags from the file*/
  untagFile(file, tags);
  
  /*Delete file from disk*/
  file->del();
}

int FileSystem::openFile(vector<string>& filePath, char mode)
{
  int fileDesc = -1;
    
  /*Find the file*/
  FileInfo* file = pathToFile(filePath);
  
  /*Create a FileOpen object corresponding to this file descriptor*/
  FileOpen* openFile = new FileOpen(file, mode, _myPartitionManager);
  
  /*Add the FileOpen* to the fileOpen table*/
  
  if(_fileOpenTable.size() <= MAXOPENFILES)
  {
    _fileOpenTable.push_back(openFile);
    fileDesc = _fileOpenTable.size() - 1;
  }
  else
  {
   /*Search for an open space*/
   for(size_t i = 0; i < _fileOpenTable.size(); i++)
    {
      if (_fileOpenTable[i] == 0)
      {
        fileDesc = i;
        _fileOpenTable[i] = openFile;
      }
    }
    if(fileDesc == -1)
    {
      /*Never found an open space. Reached maximum number of open files*/
      throw file_error("Reached maximum number of open Files", "FileSystem::openFile()");
    }
  }
  
  /*Return the index of the FileOpen* in the fileopen table*/
  return fileDesc;
}

void FileSystem::closeFile(unsigned int fileDesc)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  
  /*Close the File*/
  delete _fileOpenTable[fileDesc]; _fileOpenTable[fileDesc] = 0;
}

size_t FileSystem::readFile(unsigned int fileDesc, char* data, size_t len)
{
  /*Start reading at seek pointer*/
  if(fileDesc > _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid File descriptor", "FileSystem::writeFile");
  }
  bool setEOF = false;//whether we will read past the end of the file
  FileOpen* openFile = _fileOpenTable[fileDesc];
  Index currentIndex;
  size_t dataOffset = 0;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  /*If seek pointer is past the end of the file, throw error*/
  if(openFile->getEOF())
  {
    throw file_error("Attempt to read past EOF", "FileSystem::readFile()");
  }
  
  /*Get the index we need to start reading from*/
  currentIndex = openFile->byteToIndex(0);
  if(currentIndex.blknum == 0)
  {
    throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::readFile()");
  }
  
  /*Set len so we don't actually read past the end of valid data*/
  if(len > (openFile->getFile()->getFileSize() - openFile->getSeek()))
  {
    len = openFile->getFile()->getFileSize(); 
    setEOF = true;
  }
  
  /*Read data remaining in current block*/
  if(len <= _myPartitionManager->getBlockSize() - currentIndex.offset)
  {
    /*Read len bytes*/
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data, buff, len);
    openFile->incrementSeek(len);
    if(setEOF)
    {
      openFile->setEOF();
    }
    return len;
  }
  
  /*Read as many full blocks as we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    /*Read a full block*/
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data + dataOffset, buff, _myPartitionManager->getBlockSize());
    
    openFile->incrementSeek(_myPartitionManager->getBlockSize());
    dataOffset+= _myPartitionManager->getBlockSize(); len-= _myPartitionManager->getBlockSize();
    
    currentIndex = openFile->byteToIndex(0);
    if(currentIndex.blknum == 0)
    {
      throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::readFile()");
    }
  }
  
  /*Read any leftover bytes*/
  if(len > 0)
  {
    /*Read len bytes*/
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data + dataOffset, buff, len);
    openFile->incrementSeek(len);
    dataOffset+= _myPartitionManager->getBlockSize(); len-= _myPartitionManager->getBlockSize();
  }
  
  if(setEOF)
  {
    openFile->setEOF();
  }
  return dataOffset;
}

size_t FileSystem::writeFile(unsigned int fileDesc, const char* data, size_t len)
{
  /*Start writing at one past seek pointer*/
  
  if(fileDesc > _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid File descriptor", "FileSystem::writeFile");
  }
  
  FileOpen* openFile = _fileOpenTable[fileDesc];
  Index currentIndex;
  size_t dataOffset = 0;
  char* buff = new char[_myPartitionManager->getBlockSize()];

  /*If we are past the end of the File, go to the last byte to start writing.*/
  if(openFile->getEOF())
  {
    throw file_error("Attempt to write past EOF", "FileSystem::write()");
  }
  
  /*Get the index we need to start writing to, 1 past the seek pointer*/
  currentIndex = openFile->byteToIndex(1);
  if(currentIndex.blknum == 0)
  {
    /*There is not space allocated yet, increment Index*/
    currentIndex = openFile->incrementIndex();
  }
  
  /*Fill up current Block, must preserve data already there*/
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
  size_t bytesToWrite = _myPartitionManager->getBlockSize() - currentIndex.offset - 1;
  
  if(len <= bytesToWrite)
  {
    memcpy(buff + currentIndex.offset, data, len);
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(len, true); openFile->getFile()->setEdit();
    return len;
  }
  
  /*Write out all the remaining full blocks we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    /*Write a full block of data*/
    memcpy(buff, data + dataOffset, _myPartitionManager->getBlockSize());
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(_myPartitionManager->getBlockSize(), true); openFile->getFile()->setEdit();
    len -= _myPartitionManager->getBlockSize(); dataOffset += _myPartitionManager->getBlockSize(); 
    
    /*get Current Index*/
    if(len > 0)
    {
      currentIndex = openFile->byteToIndex(1);
      if(currentIndex.blknum == 0)
      {
        /*There is not space allocated yet, increment Index*/
        currentIndex = openFile->incrementIndex();
      }
    }
  }
  
  if(len > 0)
  {
    /*More, but not a full block of data to write*/
    memset(buff, 0, _myPartitionManager->getBlockSize());
    memcpy(buff, data + dataOffset, len);
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(len, true); openFile->getFile()->setEdit();
  }
  
  return dataOffset;
}

size_t FileSystem::appendFile(unsigned int fileDesc, const char* data, size_t len)
{
  /*Start writing at last byte of file*/
  
  if(fileDesc > _fileOpenTable.size())
  {
    throw file_error("Invalid File descriptor", "FileSystem::writeFile");
  }
  
  FileOpen* openFile = _fileOpenTable[fileDesc];
  openFile->gotoLastByte();
  return writeFile(fileDesc, data, len);
}

void FileSystem::seekFileAbsolute(unsigned int fileDesc, size_t offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  openFile->resetSeek();
  openFile->incrementSeek(offset);
}

void FileSystem::seekFileRelative(unsigned int fileDesc, size_t offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  openFile->incrementSeek(offset);
}

Attributes* FileSystem::getAttributes(vector<string>& filePath)
{
  /*Find the file*/
  FileInfo* file = pathToFile(filePath);
  
  return file->getAttributes();
}

void FileSystem::setPermissions(vector<string>& filePath, char* perms)
{
  /*Find the file*/
  FileInfo* file = pathToFile(filePath);
  
  file->setPermissions(perms);
}

void FileSystem::renameFile(vector<string>& originalFilePath, string newFileName)
{
  
  /*Rename the file*/
  FileInfo* file = pathToFile(originalFilePath);
  file->setName(newFileName);
  
  /*Change fileName in every TagTree object associated with the file*/
  for(auto tagIt = file->begin(); tagIt != file->end(); tagIt++)
  {
    tagIt->second->erase(file->getName());
    tagIt->second->insert(newFileName, file);
  }
}

void FileSystem::writeChanges()
{
  for(auto it = _modifiedObjects.begin(); it != _modifiedObjects.end(); it++)
  {
    it->first->writeOut();
  }
}

/* Start Helper Functions */

void FileSystem::insertModification(TreeObject* object)
{
  _modifiedObjects.insert(pair<TreeObject*, int>(object, 0));
}

FileInfo* FileSystem::pathToFile(vector<string>& fullPath)
{  
  if(fullPath.size() == 0)
  {
    throw arboreal_logic_error("path must at least have a file name", "FileSystem::pathToFile");
  }
  
  vector<string> path;
  string filename = fullPath.at(fullPath.size() - 1);
  
  if(fullPath.size() == 1)
  {
    /*Search default tag tree*/
    path.push_back("default");
  }
  else
  {
    /*Search first tag tree for mangled name*/
    path = fullPath;
    path.pop_back();
    filename = fullPath.at(fullPath.size() - 1);
  }
  
  TreeObject* tagTree = _RootTree->find(path[0]);
  if(tagTree == 0)
  {
    throw tag_error(path[0] + "Does not Exist", "FileSystem::pathToFile");
  }
  
  FileInfo* temp = new FileInfo(filename, 0, _myPartitionManager);
  TreeObject* file = tagTree->find(temp->mangle(path));
  if(file == 0)
  {
    throw file_error("File Does not exist", "FileSystem::pathToFile");
  }
  
  delete temp;
  return (FileInfo*)file;
}

/*End Helper Functions*/
void FileSystem::printRoot()
{
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    cout << "Key: " << it->first << " Value: " << it->second->getBlockNumber() << endl;
  }
}

void FileSystem::printTags()
{
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    cout << "TagName: " << it->first << " \tBlockNumber: " << it->second->getBlockNumber() << endl;
    
    for(auto it2 = it->second->begin(); it2 != it->second->end(); it2++)
    {
      cout << "\t FilePath: " << ((FileInfo*)(it2->second))->mangle() << " \tBlockNumber: " << it2->second->getBlockNumber() << endl;
    }
  }
}

void FileSystem::printFiles()
{
  for(auto it = _allFiles.begin(); it != _allFiles.end(); it++)
  {
    cout << "\t FilePath: " << it->second->mangle() << endl;//" \t\tBlockNumber: " << it->second->getBlockNumber() << endl;
  }
}

int FileSystem::getFileNameSize()
{
  return _myPartitionManager->getFileNameSize();
}


