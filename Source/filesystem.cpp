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
  if(_file->getFileSize() > 0)
  {
    _seek = 1;
  }
}

FileInfo* FileOpen::getFile(){return _file;}
size_t FileOpen::getSeek(){return _seek;}
char FileOpen::getMode(){return _mode;}
bool FileOpen::getEOF(){return _EOF;}
void FileOpen::gotoPastLastByte()
{
  resetSeek();
  _seek = _file->getFileSize() + 1;
}

void FileOpen::incrementSeek(size_t bytes, bool write)
{
  
  if(bytes > ((_file->getFileSize() + 1) - _seek))
  {
    if(write)
    {
      size_t newSize = 0;
      if(_seek > _file->getFileSize()) newSize = bytes + _seek - 1;
      else newSize = bytes + _seek;
      if(_seek == 0)
      {
        _seek++;
      }
      _seek += bytes;
      if(_file->getFileSize() < newSize)
      {
        _file->updateFileSize(newSize);
      }
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

void FileOpen::decrementSeek(size_t bytes)
{
  if(bytes >= _seek)
  {
    setEOF();
  }
  else
  {
    _seek -= bytes;
  }
}

Index FileOpen::byteToIndex(short offset)
{
  refresh();
  if(offset != 0 && offset != 1)
  {
    throw arboreal_logic_error("Provided an offset other than 0 or 1", "FileOpen::byteToIndex()");
  }
  if(getEOF())
  {
    throw arboreal_logic_error("Cannot get index when past EOF", "FileOPen::byteToIndex()");
  }
  
  if(_seek == 0)
  {
    /*Empty file. return {0,0}*/
    return Index{0,0};
  }
  
  size_t seek = _seek + offset;
  unsigned int entriesPerBlock = _myPartitionManager->getBlockSize() / sizeof(BlkNumType);
  size_t blockIndex = seek / _myPartitionManager->getBlockSize();
  Index ret;
  vector<int> remainders;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  /*Set the return offset*/
  ret.offset = (seek - 1) % _myPartitionManager->getBlockSize();
  
  if(blockIndex < 12)
  {
    if(_file->getFinode().directBlocks[blockIndex] == 0)
    { 
      /*Space not yet allocated*/
      return Index{0,0};
    }
    ret.blknum = _file->getFinode().directBlocks[blockIndex];
    return ret;
  }
  else
  {
    int levelCount = 0;
    blockIndex -= 12;
    while(blockIndex >= entriesPerBlock)
    {
      remainders.push_back(blockIndex % entriesPerBlock);
      blockIndex /= entriesPerBlock;
      levelCount++;
    }
    switch(levelCount)
    {
      case 0:
      {        
        /*Read in Level 1 indirect block*/
        if(_file->getFinode().level1Indirect == 0)
        { 
          /*Space not yet allocated*/
          return Index{0,0};
        }
        _myPartitionManager->readDiskBlock(_file->getFinode().level1Indirect, buff);
  
        /*blockIndex is the level 1 offset*/
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
  refresh();
  
  /*Assuming this function is only called when the seek pointing to the end of a block*/
  int check = (_seek - 1) % _myPartitionManager->getBlockSize();
  if(check != 0 && (_seek != 0))
  {
    throw arboreal_logic_error("Called incrementIndex when seekptr was not at end of a block", "FileOpen::incrementIndex");
  }
  
  if(getEOF() || ((_seek != _file->getFileSize() + 1) && (_seek != 0)))
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
    return Index{blknum, 0};
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
  if(blockIndex <= 12)
  {
    blockIndex = 0;
  }
  else
  {
    blockIndex -= 12;
  }
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
      size_t relativeBlock = blockIndex - (level1Offset);
      ret.blknum = levelInc(relativeBlock, _file->getFinode().level2Indirect, 2);
      break;
    }
    case 2:
    {
      /*We're in level 3*/
      /*blockIndex - (level2Offset + directOffset) is the level 3 relativeBlock*/
      size_t relativeBlock = blockIndex - (level2Offset);
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
  int entriesPerBlock = blockSize / sizeof(BlkNumType);
  char* buff = new char[blockSize];
  memset(buff, 0, blockSize);
  char* zeroBuff = new char[blockSize];
  memset(zeroBuff, 0, blockSize);
  
  _myPartitionManager->readDiskBlock(ledgerBlock, buff);
  
  switch(level)
  {
    case 1:
    {
      BlkNumType dataBlock = _myPartitionManager->getFreeDiskBlock();
      memcpy(buff + (relativeBlock * sizeof(BlkNumType)), &dataBlock, sizeof(BlkNumType));
      _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      delete buff; delete zeroBuff;
      return dataBlock;
      
    }
    case 2:
    {
      BlkNumType level1Block;
      if(relativeBlock % entriesPerBlock == 0)
      {
        level1Block = _myPartitionManager->getFreeDiskBlock();
        _myPartitionManager->writeDiskBlock(level1Block, zeroBuff);
        memcpy(buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), &level1Block, sizeof(BlkNumType));
        _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      }
      memcpy(&level1Block, buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), sizeof(BlkNumType));
      delete buff; delete zeroBuff;
      return levelInc(relativeBlock % entriesPerBlock, level1Block, level - 1);
    }
    case 3:
    {
      BlkNumType level2Block;
      if(relativeBlock % (entriesPerBlock^2) == 0)
      {
        level2Block = _myPartitionManager->getFreeDiskBlock();
        _myPartitionManager->writeDiskBlock(level2Block, zeroBuff);
        
        memcpy(buff + ((relativeBlock/entriesPerBlock^2) * sizeof(BlkNumType)), &level2Block, sizeof(BlkNumType));
        _myPartitionManager->writeDiskBlock(ledgerBlock, buff);
      }
      memcpy(&level2Block, buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), sizeof(BlkNumType));
      delete buff; delete zeroBuff;
      return levelInc(relativeBlock % (entriesPerBlock^2), level2Block, level - 1);
    }
    default:
    {
      throw arboreal_logic_error("Invalid level", "FileOpen::levelInc");
    }
  }
  delete buff;
  return 0;
}

void FileOpen::setEOF()
{
  _seek = 0;
  _EOF = true;
}

void FileOpen::resetSeek()
{
  if(_file->getFileSize() > 0)
  {
    _seek = 1;
  }
  else
  {
    _seek = 0;
  }
  _EOF = false;
}

void FileOpen::refresh()
{
  if(_file->getFileSize() > 0 && _seek == 0)
  {
    _seek = 1;
  }
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

vector<FileInfo*>* FileSystem::tagSearch(unordered_set<string>& tags)
{
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  if(tags.size() == 0)
  {
    throw tag_error("No tags specified to search for", "FileSystem::tagSearch");
  }
  else if(tags.size() == 1)
  {
    /*find the tag in root tree*/
    TreeObject* tagTree = _RootTree->find(*(tags.begin()));
    
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
    for(string tag : tags)
    {
      TreeObject* tagTree = _RootTree->find(tag);
      if(tagTree == 0)
      {
        cerr << tag + " excluded from search : Tag Does not exist" << endl;
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
        for(string tag : tags)
        {
          if(fileIt->second->find(tag) == 0)
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

void FileSystem::tagFile(FileInfo* file, unordered_set<string> tags)
{
  /*Validate the tagging of this file first*/
  unordered_set<string> tagsToAdd = tags;
  
  if(tagsToAdd.size() == 0) {throw tag_error("No tags specified","FileSystem::tagFile");}
  if(file == 0) {throw file_error ("File Does not Exist", "FileSystem::tagFile");}
  
  unordered_set<string> wholeTagSet = file->getTags();
  
  TreeObject* tagTree = 0;
  
  for(string tag : tags)
  {
    /*Remove tags that do not exist*/
    tagTree = _RootTree->find(tag);
    if(tagTree == 0)
    {
      cerr << tag << " Does Not Exist: Not added to file tag set" << endl;
      tagsToAdd.erase(tag);
    }
    else
    {
      wholeTagSet.insert(tag);
    }
  }
  
  if(tagsToAdd.size() == 0)
  {
    throw tag_error("No valid tags specified: file not Changed", "FileSystem::tagFile");
  }
  

  /*Validate the new file*/
  TreeObject* fileCheck = 0;
  
  tagTree = _RootTree->find(*(wholeTagSet.begin()));
  if(tagTree == 0)
  {
    throw arboreal_logic_error(*wholeTagSet.begin() + " does not exist", "FileSystem::untagFile");
  }
  fileCheck =  tagTree->find(file->mangle(wholeTagSet));
  
  if(fileCheck != 0)
  {
    throw file_error(file->get_name() + " with the specified tags already exists", "FileSystem::tagFile");
  }
  
  /*Remove tags already that the file already has*/
  unordered_set<string> fileTagSet = file->getTags();
  for(string tag : fileTagSet)
  {
    wholeTagSet.erase(tag);
  }
  
  if(wholeTagSet.size() == 0)
  {
    cerr << "File was not tagged with anything new" << endl;
    return;
  }
  

  
  /*For every tag in newTags*/
  for(string tag : wholeTagSet)
  {
    /*find tagTree*/
    tagTree = _RootTree->find(tag);
    
    /*Add Tag to Finode */
    file->insert(tag, tagTree);
    
    /*Add Finode to tagTree*/
    tagTree->insert(file->mangle(wholeTagSet), file);
    
    /*Note TagTree was modified*/
    insertModification(tagTree);
  
  }
  
  /*Remove default tag, if it exists in this file */
  if(file->find("default") != 0)
  {
    file->erase("default");
  }
  
  file->writeOut();
}

void FileSystem::tagFile(vector<string>& filePath, unordered_set<string> tagsToAdd)
{
  FileInfo* file = pathToFile(filePath);
  tagFile(file, tagsToAdd);
}

void FileSystem::untagFile(vector<string>& filePath, unordered_set<string> tagsToRemove)
{
  FileInfo* file = pathToFile(filePath);
  untagFile(file, tagsToRemove);
}

void FileSystem::untagFile(FileInfo* file, unordered_set<string> tags, bool deleting)
{
  if(file == 0)
  {
    throw arboreal_logic_error("Invalid FileInfo*", "FileSystem::untagFile()");
  }
  
  string originalFileName = file->mangle();
  unordered_set<string> tagsToRemove = tags;
  unordered_set<string> currentTagSet = file->getTags();
  
  TreeObject* tagTree = 0;
  
  for(string tag : tags)
  {
    /*Remove tags that do not exist and that are not part of the file's current tag set*/
    tagTree = _RootTree->find(tag);
    if(tagTree == 0)
    {
      cerr << tag << " Does Not Exist: Tag cannot be removed" << endl;
      tagsToRemove.erase(tag);
    }
    else if(currentTagSet.find(tag) == currentTagSet.end())
    {
      tagsToRemove.erase(tag);
    }
  }
  
  unordered_set<string> potentialTagSet = file->getTags();
  for(string tag : tagsToRemove)
  {
    potentialTagSet.erase(tag);
  }
  
  /*Validate the new file*/
  TreeObject* fileCheck = 0;
  if(potentialTagSet.size() == 0)
  {
    if(!deleting)
    {
      /*TODO: Search default tree for file*/
    }
  }
  else
  {
    tagTree = _RootTree->find(*(potentialTagSet.begin()));
    if(tagTree == 0)
    {
      throw arboreal_logic_error(*potentialTagSet.begin() + " does not exist", "FileSystem::untagFile");
    }
    fileCheck =  tagTree->find(file->mangle(potentialTagSet));
  }

  if(fileCheck != 0)
  {
    throw file_error(file->get_name() + " with the specified tags already exists", "FileSystem::tagFile");
  }
  
  for(string tag : tagsToRemove)
  {
    if(tag == "default")
    {
      cerr << tag + " cannot be removed from " << file->get_name() << endl;
    }
    
    /*find tagTree*/
    TreeObject* tagTree = _RootTree->find(tag);
    
    /*Remove Finode from tagTree*/
    tagTree->erase(originalFileName);
    
    /*Remove tag from Finode*/
    file->erase(tag);
    
    /*Note Tag Tree was modified*/
    insertModification(tagTree);
  }
  
  /*if removed all Tags from file, add default tag*/
  if(file->size() == 0 && !deleting)
  {
    TreeObject* defaultTree = _RootTree->find("default");
    file->insert("default", defaultTree);
    vector<string> tagSet; tagSet.push_back("default");
    defaultTree->insert(file->mangle(tagSet), file);
    insertModification(defaultTree);
  }
  
  /* write updated Finode to disk*/
  file->writeOut();
}

void FileSystem::renameTag(string originalTagName, string newTagName)
{
  /*Cannot rename tag to something that already exists*/
  TreeObject* tagTreeCheck = _RootTree->find(newTagName);
  if(tagTreeCheck != 0)
  {
    throw tag_error("new Tag name must not already exist!", " FileSystem::renameTag()");
  }
  
  /*Rename the tagTree*/
  TreeObject* tagTree = _RootTree->find(originalTagName);
  tagTree->set_name(newTagName);
  
  /*Note TagTree was modified*/
  insertModification(tagTree);
  
  /*Change tagName in rootTree*/
  _RootTree->erase(originalTagName);
  _RootTree->insert(newTagName, tagTree);
  
  /*Note RootTree was modified*/
  insertModification(_RootTree);
  
  
  /*Change tagName in every FileInfo object of the tagTree*/
  for(auto fileIt = tagTree->begin(); fileIt != tagTree->end(); fileIt++)
  {
    fileIt->second->erase(originalTagName);
    fileIt->second->insert(newTagName, tagTree);
    fileIt->second->writeOut();
  }
}

FileInfo* FileSystem::createFile(string filename, unordered_set<string>& tags)
{
  /*Get a block from disk to store Finode*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  FileInfo* newFile = new FileInfo(filename, newblknum, _myPartitionManager);
  /* If no tag was specified then add file to "default" tag tree
   * File remains in default tag tree until a non-default tag is associated with file*/
  if(tags.size() == 0)
  {
    unordered_set<string> temp;
    temp.insert("default");
    
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
  string originalFileName = file->mangle();
  if(file == 0)
  {
    throw arboreal_logic_error("Invalid FileInfo*", "FileSystem::deleteFile()");
  }
  
  unordered_set<string> tags = file->getTags();
  
  /*Dissasociate all tags from the file*/
  untagFile(file, tags, true);

  /*Delete file from disk*/
  file->del();
  
  /*Delete file from _allFiles*/
  auto ret = _allFiles.equal_range(file->get_name());
  for(auto fileIt = ret.first; fileIt != ret.second; fileIt++)
  {
    if(fileIt->second == file)
    {
      _allFiles.erase(fileIt);
      break;
    }
  }
}

void FileSystem::deleteFile(vector<string>& filePath)
{
  FileInfo* file = pathToFile(filePath);
  deleteFile(file);
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
  if(fileDesc >= _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  
  /*Close the File*/
  delete _fileOpenTable[fileDesc]; _fileOpenTable[fileDesc] = 0;
}

size_t FileSystem::readFile(unsigned int fileDesc, char* data, size_t len)
{
  if(len == 0)
  {
    return 0;
  }
  
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
  
  if(openFile->getMode() != 'r' && openFile->getMode() != 'x')
  {
    throw file_error("File not opened with read permissions", "FileSystem::writeFile");
  }
  
  /*If seek pointer is past the end of the file, throw error*/
  if(openFile->getEOF())
  {
    throw file_error("Attempt to read past EOF", "FileSystem::readFile()");
  }
  
  /*Get the index we need to start reading from*/
  currentIndex = openFile->byteToIndex(0);
  if(currentIndex.blknum == 0)
  {
//     throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::readFile()");
    openFile->setEOF();
    return 0;
  }
  
  /*Set len so we don't actually read past the end of valid data*/
  if(len > (openFile->getFile()->getFileSize() - (openFile->getSeek() - 1)))
  {
    len = openFile->getFile()->getFileSize() - openFile->getSeek(); 
    setEOF = true;
  }
  
  /*Read data remaining in current block*/
  size_t bytesToRead = _myPartitionManager->getBlockSize() - currentIndex.offset;

  
  if(len <= bytesToRead)
  {
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data, buff + currentIndex.offset, len);
    openFile->incrementSeek(len);
    if(setEOF)
    {
      openFile->setEOF();
    }
    return len;
  }
  else
  {
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data, buff + currentIndex.offset, bytesToRead);
    openFile->incrementSeek(bytesToRead);
  }

  len-= bytesToRead; dataOffset+= bytesToRead;
  
  /*Read as many full blocks as we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    currentIndex = openFile->byteToIndex(0);
    if(currentIndex.blknum == 0)
    {
      throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::readFile()");
    }
    
    /*Read a full block*/
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data + dataOffset, buff + currentIndex.offset, _myPartitionManager->getBlockSize());
    
    openFile->incrementSeek(_myPartitionManager->getBlockSize());
    dataOffset+= _myPartitionManager->getBlockSize(); len-= _myPartitionManager->getBlockSize();
  }
  
  /*Read any leftover bytes*/
  if(len > 0)
  {
    currentIndex = openFile->byteToIndex(0);
    if(currentIndex.blknum == 0)
    {
      throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::readFile()");
    }
    
    /*Read len bytes*/
    _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    memcpy(data + dataOffset, buff + currentIndex.offset, len);
    openFile->incrementSeek(len);
    dataOffset+= len;
  }
  
  if(setEOF)
  {
    openFile->setEOF();
  }
  delete buff;
  return dataOffset;
}

size_t FileSystem::writeFile(unsigned int fileDesc, const char* data, size_t len)
{
  /*Start writing at seek pointer*/
  
  if(fileDesc > _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid File descriptor", "FileSystem::writeFile");
  }
  
  FileOpen* openFile = _fileOpenTable[fileDesc];
  Index currentIndex;
  size_t dataOffset = 0;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  if(openFile->getMode() != 'w' && openFile->getMode() != 'x')
  {
    throw file_error("File not opened with write permissions", "FileSystem::writeFile");
  }

  /*If we are past the end of the File, go to the last byte to start writing.*/
  if(openFile->getEOF())
  {
    throw file_error("Attempt to write past EOF", "FileSystem::write()");
  }
  
  /*Get the index we need to start writing to, 1 past the seek pointer*/
  currentIndex = openFile->byteToIndex(0);
  if(currentIndex.blknum == 0)
  {
    /*There is not space allocated yet, increment Index*/
    currentIndex = openFile->incrementIndex();
  }
  
  /*Fill up current Block, must preserve data already there*/
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
  size_t bytesToWrite = _myPartitionManager->getBlockSize() - currentIndex.offset;
  
  if(len <= bytesToWrite)
  {
    memcpy(buff + currentIndex.offset, data, len);
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(len, true); openFile->getFile()->setEdit();
    return len;
  }
  else
  {
    memcpy(buff + currentIndex.offset, data, bytesToWrite);
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(bytesToWrite, true); openFile->getFile()->setEdit();
  }

  len-= bytesToWrite;
  dataOffset+= bytesToWrite;
  /*Write out all the remaining full blocks we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    /*get Current Index*/
    if(len > 0)
    {
      currentIndex = openFile->byteToIndex(0);
      if(currentIndex.blknum == 0)
      {
        /*There is not space allocated yet, increment Index*/
        currentIndex = openFile->incrementIndex();
      }
    }
    
    /*Write a full block of data*/
    memcpy(buff, data + dataOffset, _myPartitionManager->getBlockSize());
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(_myPartitionManager->getBlockSize(), true); openFile->getFile()->setEdit();
    len -= _myPartitionManager->getBlockSize(); dataOffset += _myPartitionManager->getBlockSize(); 
    
  }
  
  if(len > 0)
  {
    /*More, but not a full block of data to write*/
    currentIndex = openFile->byteToIndex(0);
    if(currentIndex.blknum == 0)
    {
      /*There is not space allocated yet, increment Index*/
      currentIndex = openFile->incrementIndex();
    }
    
    memset(buff, 0, _myPartitionManager->getBlockSize());
    memcpy(buff + currentIndex.offset, data + dataOffset, len);
    _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
    openFile->incrementSeek(len, true); openFile->getFile()->setEdit();
    dataOffset += len; 
  }
  
  delete buff;
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
  openFile->gotoPastLastByte();
  return writeFile(fileDesc, data, len);
}

void FileSystem::seekFileAbsolute(unsigned int fileDesc, size_t offset)
{
  /*NOTE: to the outside world seek is 0 indexed.*/
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::seekFileAbsolute");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  openFile->resetSeek();
  openFile->incrementSeek(offset);
}

void FileSystem::seekFileRelative(unsigned int fileDesc, long int offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::seekFileRelative");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  if(offset < 0)
  {
    openFile->decrementSeek(-offset);
  }
  else if(offset > 0)
  {
    openFile->incrementSeek(offset);
  }
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
  file->set_name(newFileName);
  
  /*Change fileName in every TagTree object associated with the file*/
  for(auto tagIt = file->begin(); tagIt != file->end(); tagIt++)
  {
    tagIt->second->erase(file->get_name());
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
    cout << "Key: " << it->first << " Value: " << it->second->get_block_number() << endl;
  }
}

void FileSystem::printTags()
{
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    cout << "TagName: " << it->first << " \tBlockNumber: " << it->second->get_block_number() << endl;
    
    for(auto it2 = it->second->begin(); it2 != it->second->end(); it2++)
    {
      cout << "\t FilePath: " << ((FileInfo*)(it2->second))->mangle() << " \tBlockNumber: " << it2->second->get_block_number() << endl;
    }
  }
}

void FileSystem::printFiles()
{
  for(auto it = _allFiles.begin(); it != _allFiles.end(); it++)
  {
    cout << "\t FilePath: " << it->second->mangle() << endl;//" \t\tBlockNumber: " << it->second->get_block_number() << endl;
  }
}

int FileSystem::getFileNameSize()
{
  return _myPartitionManager->getFileNameSize();
}


