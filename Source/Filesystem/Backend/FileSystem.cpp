///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileSystem.cpp
//  FileSystem Source File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FileSystem.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
using std::cerr;
using std::string;
using std::string;
using std::unordered_map;
using std::vector;
using std::unordered_set;
using std::endl;
using std::cout;
using std::pair;

bool EncryptionFlag = false;

FileOpen::FileOpen(FileInfo* file, char mode, PartitionManager* pm): _file(file), _mode(mode), _myPartitionManager(pm)
{
  _seek = 0;
  _EOF = false;
  if(_file->get_file_size() > 0)
  {
    _seek = 1;
  }
}

FileInfo* FileOpen::getFile(){return _file;}
size_t FileOpen::getSeek(){return _seek;}
char FileOpen::getMode(){return _mode;}
bool FileOpen::get_EOF(){return _EOF;}
void FileOpen::go_past_last_byte()
{
  reset_seek();
  _seek = _file->get_file_size() + 1;
}

void FileOpen::increment_seek(size_t bytes, bool write)
{
  
  if(bytes > ((_file->get_file_size() + 1) - _seek))
  {
    if(write)
    {
      size_t newSize = 0;
      if(_seek > _file->get_file_size()) newSize = bytes + _seek - 1;
      else newSize = bytes + _seek;
      if(_seek == 0)
      {
        _seek++;
      }
      _seek += bytes;
      if(_file->get_file_size() < newSize)
      {
        _file->update_file_size(newSize);
      }
    }
    else
    {
      set_EOF();
    }
  }
  else
  {
    _seek += bytes;
  }
}

void FileOpen::decrement_seek(size_t bytes)
{
  if(bytes >= _seek)
  {
    set_EOF();
  }
  else
  {
    _seek -= bytes;
  }
}

Index FileOpen::byte_to_index(short offset)
{
  refresh();
  if(offset != 0 && offset != 1)
  {
    throw arboreal_logic_error("Provided an offset other than 0 or 1", "FileOpen::byte_to_index()");
  }
  if(get_EOF())
  {
    throw arboreal_logic_error("Cannot get index when past EOF", "FileOPen::byte_to_index()");
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
    if(_file->get_finode().directBlocks[blockIndex] == 0)
    { 
      /*Space not yet allocated*/
      delete[] buff;
      return Index{0,0};
    }
    ret.blknum = _file->get_finode().directBlocks[blockIndex];
    delete[] buff;
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
        if(_file->get_finode().level1Indirect == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(_file->get_finode().level1Indirect, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
  
        /*blockIndex is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (blockIndex * sizeof(BlkNumType)), sizeof(BlkNumType));
        ret.blknum = blknum;
        
        break; 
      }
      case 1:
      {
        /*Read in Level 2 indirect block*/
        if(_file->get_finode().level2Indirect == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(_file->get_finode().level2Indirect, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
        
        /*blockIndex - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        if(l1blknum == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(l1blknum, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
        
        /*remainders[0] is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (remainders[0] * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        ret.blknum = blknum;
        
        break;
      }
      case 2:
      {        
        /*Read in Level 3 indirect block*/
        
        if(_file->get_finode().level3Indirect == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(_file->get_finode().level3Indirect, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
        
        /*blockIndex - 1 is the level 3 offset*/
        BlkNumType l2blknum;
        memcpy(&l2blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in level 2 indirect block*/
        if(l2blknum == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(l2blknum, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
        
        /*remainders[1] - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((remainders[1] - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        if(l1blknum == 0)
        { 
          /*Space not yet allocated*/
          delete[] buff;
          return Index{0,0};
        }
        try{_myPartitionManager->readDiskBlock(l1blknum, buff);}
        catch(arboreal_exception& e) { delete[] buff; throw; }
        
        /*remainders[0] is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (remainders[0] * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        ret.blknum = blknum;
        
        break;
      }
      default:
      {
        delete[] buff;
        throw arboreal_logic_error("Invalid level value", "FileOpen::byte_to_index");
      }
    }
  }
  if(buff != 0) delete[] buff; buff = 0;
  return ret;
}

Index FileOpen::increment_index()
{
  refresh();
  
  /*Assuming this function is only called when the seek pointing to the end of a block*/
  int check = (_seek - 1) % _myPartitionManager->getBlockSize();
  if(check != 0 && (_seek != 0))
  {
    throw arboreal_logic_error("Called increment_index when seekptr was not at end of a block", "FileOpen::increment_index");
  }
  
  if(get_EOF() || ((_seek != _file->get_file_size() + 1) && (_seek != 0)))
  {
    /*Assuming this function is only called when the seek is at the last byte of the file*/
    throw arboreal_logic_error("Called increment_index when seek pointer was not pointing to last byte of file", "FileOpen::increment_index");
  }
  
  Index ret{0,0};
  size_t seek = _file->get_file_size();
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
    _file->add_direct_block(blknum, nextDirectBlockIndex);
    return Index{blknum, 0};
  }
  else if(blockIndex == directOffset)
  {
    /*Need to allocate the level 1 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->add_indirect_block(blknum, 1);
    
  }
  else if(blockIndex == directOffset + level1Offset)
  {
    /*Need to allocate the Level 2 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->add_indirect_block(blknum, 2);
  }
  else if(blockIndex == directOffset + level2Offset)
  {
    /*Need to allocate level 3 indirect block*/
    BlkNumType blknum = _myPartitionManager->getFreeDiskBlock();
    _file->add_indirect_block(blknum, 3);      
  }
  else if(blockIndex == directOffset + level3Offset)
  {
    throw file_error("Maximum File Size Reached!", "FileOpen::increment_index");
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
      ret.blknum = levelInc(blockIndex, _file->get_finode().level1Indirect, 1);
      break;
    }
    case 1:
    {
      /*We're in level 2*/
      /*blockIndex - (level1Offset + directOffset) is the level 2 relativeBlock*/
      size_t relativeBlock = blockIndex - (level1Offset);
      ret.blknum = levelInc(relativeBlock, _file->get_finode().level2Indirect, 2);
      break;
    }
    case 2:
    {
      /*We're in level 3*/
      /*blockIndex - (level2Offset + directOffset) is the level 3 relativeBlock*/
      size_t relativeBlock = blockIndex - (level2Offset);
      ret.blknum = levelInc(relativeBlock, _file->get_finode().level3Indirect, 3);
      break;
    }
    default:
    {
      throw arboreal_logic_error("Invalid level Count", "FileOpen::increment_index()");
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
  
  try{ _myPartitionManager->readDiskBlock(ledgerBlock, buff); }
  catch(arboreal_exception& e) { delete[] buff; throw; }
  
  switch(level)
  {
    case 1:
    {
      BlkNumType dataBlock = _myPartitionManager->getFreeDiskBlock();
      memcpy(buff + (relativeBlock * sizeof(BlkNumType)), &dataBlock, sizeof(BlkNumType));
      try{_myPartitionManager->writeDiskBlock(ledgerBlock, buff);}
      catch(arboreal_exception& e) { delete[] buff; delete zeroBuff; throw; }
      delete[] buff; delete[] zeroBuff;
      return dataBlock;
      
    }
    case 2:
    {
      BlkNumType level1Block;
      if(relativeBlock % entriesPerBlock == 0)
      {
        level1Block = _myPartitionManager->getFreeDiskBlock();
        try{_myPartitionManager->writeDiskBlock(level1Block, zeroBuff);}
        catch(arboreal_exception& e) { delete[] buff; delete zeroBuff; throw; }
        memcpy(buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), &level1Block, sizeof(BlkNumType));
        try{_myPartitionManager->writeDiskBlock(ledgerBlock, buff);}
        catch(arboreal_exception& e) { delete[] buff; delete zeroBuff; throw; }
      }
      memcpy(&level1Block, buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), sizeof(BlkNumType));
      delete[] buff; delete[] zeroBuff;
      return levelInc(relativeBlock % entriesPerBlock, level1Block, level - 1);
    }
    case 3:
    {
      BlkNumType level2Block;
      if(relativeBlock % (entriesPerBlock^2) == 0)
      {
        level2Block = _myPartitionManager->getFreeDiskBlock();
        try{_myPartitionManager->writeDiskBlock(level2Block, zeroBuff);}
        catch(arboreal_exception& e) { delete[] buff; delete zeroBuff; throw; }
        
        memcpy(buff + ((relativeBlock/entriesPerBlock^2) * sizeof(BlkNumType)), &level2Block, sizeof(BlkNumType));
        try{_myPartitionManager->writeDiskBlock(ledgerBlock, buff);}
        catch(arboreal_exception& e) { delete[] buff; delete zeroBuff; throw; }
      }
      memcpy(&level2Block, buff + ((relativeBlock/entriesPerBlock) * sizeof(BlkNumType)), sizeof(BlkNumType));
      delete[] buff; delete[] zeroBuff;
      return levelInc(relativeBlock % (entriesPerBlock^2), level2Block, level - 1);
    }
    default:
    {
      delete[] buff; delete[] zeroBuff;
      throw arboreal_logic_error("Invalid level", "FileOpen::levelInc");
    }
  }
  delete[] buff; delete[] zeroBuff;
  return 0;
}

void FileOpen::set_EOF()
{
  _seek = 0;
  _EOF = true;
}

void FileOpen::reset_seek()
{
  if(_file->get_file_size() > 0)
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
  if(_file->get_file_size() > 0 && _seek == 0)
  {
    _seek = 1;
  }
}

/***********************************************************************/

FileSystem::FileSystem(DiskManager *dm, string partitionName)
{
  _FSName = partitionName;
  
  /* set partition manager for my partition */
  _myPartitionManager = new PartitionManager(dm, partitionName);
  
  /*Read in the root tree*/
  _RootTree = new RootTree(_myPartitionManager);
  _RootTree->read_in(&_allFiles, _RootTree);
  
  /*Read in every tag Tree*/
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    it->second->read_in(&_allFiles, _RootTree);
  }
}

FileSystem::~FileSystem()
{
  delete _myPartitionManager;
  delete _RootTree;
  for(auto file : _allFiles)
  {
    delete file.second;
  }
  
  for(auto objIt = objsToDelete.begin(); objIt != objsToDelete.end(); objIt++)
  {
    delete *objIt;
  }
  objsToDelete.clear();
  
  for(auto fileIt = _fileOpenTable.begin(); fileIt != _fileOpenTable.end(); fileIt++)
  {
    if(*fileIt != 0 )
    {
      delete *fileIt; 
      *fileIt = 0;
    }
  }
  _fileOpenTable.clear();
}

vector<FileInfo*>* FileSystem::tag_search(unordered_set<string>& tags)
{
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  if(tags.size() == 0)
  {
    throw tag_error("No tags specified to search for", "FileSystem::tag_search");
  }
  else if(tags.size() == 1)
  {
    /*find the tag in root tree*/
    TreeObject* tagTree = _RootTree->find(*(tags.begin()));
    
    if(tagTree == 0)
    {
        throw tag_error("Tag " + *(tags.begin()) + " Does not exist", "FileSystem::tag_search()");
    }
    
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
        throw tag_error("Tag " + tag + " Does not exist", "FileSystem::tag_search()");
      }
      else
      {
        searchTrees.push_back(tagTree);
      }
    }
    
    if(searchTrees.size() == 0)
    {
        return ret;
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

vector<FileInfo*>* FileSystem::file_search(string name)
{
  /*We're going to use the _allFiles variable to find the files*/
  
  vector<FileInfo*>* ret = new vector<FileInfo*>;
  
  auto files = _allFiles.equal_range(name);
  
  for (auto it = files.first; it != files.second; it++) 
  {
    ret->push_back(it->second);
  }
  if(ret->size() == 0)
  {
    throw arboreal_exception("File Not Found","[FileSystem.cpp::fileSearch()]: ");
  }
  return ret;
}

void FileSystem::create_tag(string tagName)
{
  /*Get a block from disk to store tag tree super block*/
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  /* initialize tree in main memory */
  TagTree* newTag = new TagTree(tagName, newblknum, _myPartitionManager);
  try
  {
    /* add TagTree to root tree */
    _RootTree->insert(tagName, newTag);
    
    /*Note Root Tree was modified*/
    insert_modification(_RootTree);
    
    /*Write out newly created TagTree. it will only write out the TagTree superblock*/
    newTag->write_out();
  }
  catch(arboreal_exception& e)
  {
    delete newTag;
    throw;
  }

}

void FileSystem::delete_tag(string tagName)
{
  /*Find tagTree*/
  TreeObject* tagTree = _RootTree->find(tagName);
  if(tagTree == 0)
  {
    throw tag_error(tagName + " Does Not Exist", "FileSystem::delete_tag");
  }
    
  /* CANNOT delete tag if tag tree Size > 0 */
  if(tagTree->size() > 0)
  {
    throw tag_error(tagName + " cannot be deleted: Tag has files associated with it", "FileSystem::delete_tag");
  }
  
  /*Delete tagTree on disk*/
  tagTree->del();
  /*Delete TagTree from Root Tree*/
  _RootTree->erase(tagName);
  /*Note Root Tree was modified*/
  insert_modification(_RootTree);
  
  objsToDelete.push_back(tagTree);
}

void FileSystem::merge_tags(string tag1, string tag2)
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

void FileSystem::tag_file(FileInfo* file, unordered_set<string> tags)
{
  /*Validate the tagging of this file first*/
  unordered_set<string> tagsToAdd = tags;
  
  if(tagsToAdd.size() == 0) {throw tag_error("No tags specified","FileSystem::tag_file");}
  if(file == 0) {throw file_error ("File Does not Exist", "FileSystem::tag_file");}
  
  unordered_set<string> wholeTagSet = file->get_tags();
  
  TreeObject* tagTree = 0;
  
  for(string tag : tags)
  {
    /*Disallow tagging with default*/
    if(tag == "default")
    {
      throw tag_error("Cannot tag a file with the default tag", "FileSystem::tag_file");
    }
    /*Remove tags that do not exist*/
    tagTree = _RootTree->find(tag);
    if(tagTree == 0)
    {
      throw tag_error (" Does Not Exist: Not added to file tag set", "FileSystem::tag_file");
      tagsToAdd.erase(tag);
    }
    else
    {
      wholeTagSet.insert(tag);
    }
  }
  
  if(tagsToAdd.size() == 0)
  {
    throw tag_error("No valid tags specified: file not Changed", "FileSystem::tag_file");
  }
  

  /*Validate the new file*/
  TreeObject* fileCheck = 0;
  
  tagTree = _RootTree->find(*(wholeTagSet.begin()));
  if(tagTree == 0)
  {
    throw arboreal_logic_error(*wholeTagSet.begin() + " does not exist", "FileSystem::untag_file");
  }
  fileCheck =  tagTree->find(file->mangle(wholeTagSet));
  
  if(fileCheck != 0)
  {
    throw file_error(file->get_name() + " with the specified tags already exists", "FileSystem::tag_file");
  }
  
  /*Remove tags already that the file already has*/
  unordered_set<string> fileTagSet = file->get_tags();
  for(string tag : fileTagSet)
  {
    wholeTagSet.erase(tag);
  }
  
  if(wholeTagSet.size() == 0)
  {
    throw tag_error("File was not tagged with anything new", "FileSystem::tag_file");
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
    insert_modification(tagTree);
  
  }
  
  /*Remove default tag, if it exists in this file */
  if(file->find("default") != 0)
  {
    file->erase("default");
  }
  
  file->write_out();
}

void FileSystem::tag_file(vector<string>& filePath, unordered_set<string> tagsToAdd)
{
  FileInfo* file = path_to_file(filePath);
  tag_file(file, tagsToAdd);
}

void FileSystem::untag_file(vector<string>& filePath, unordered_set<string> tagsToRemove)
{
  FileInfo* file = path_to_file(filePath);
  untag_file(file, tagsToRemove);
}

void FileSystem::untag_file(FileInfo* file, unordered_set<string> tags, bool deleting)
{
  if(file == 0)
  {
    throw arboreal_logic_error("Invalid FileInfo*", "FileSystem::untag_file()");
  }
  
  string originalFileName = file->mangle();
  unordered_set<string> tagsToRemove = tags;
  unordered_set<string> currentTagSet = file->get_tags();
  
  TreeObject* tagTree = 0;
  
  for(string tag : tags)
  {
    /*Remove tags that do not exist and that are not part of the file's current tag set*/
    tagTree = _RootTree->find(tag);
    if(tagTree == 0)
    {
      throw tag_error (" Does Not Exist: Tag cannot be removed", "FileSystem::untag_file()");
      tagsToRemove.erase(tag);
    }
    else if(currentTagSet.find(tag) == currentTagSet.end())
    {
      tagsToRemove.erase(tag);
    }
  }
  
  unordered_set<string> potentialTagSet = file->get_tags();
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
      TreeObject* defaultTree = _RootTree->find("default");
      if(defaultTree == 0)
      {
        throw arboreal_logic_error("Default Tree not Found!", "FileSystem::untag_file()");
      }
      
      fileCheck = defaultTree->find(file->mangle());
    }
  }
  else
  {
    tagTree = _RootTree->find(*(potentialTagSet.begin()));
    if(tagTree == 0)
    {
      throw arboreal_logic_error(*potentialTagSet.begin() + " does not exist", "FileSystem::untag_file()");
    }
    fileCheck =  tagTree->find(file->mangle(potentialTagSet));
  }

  if(fileCheck != 0)
  {
    throw file_error(file->get_name() + " with the specified tags already exists", "FileSystem::untag_file()");
  }
  
  for(string tag : tagsToRemove)
  {
    if(tag == "default")
    {
      throw (tag + " cannot be removed from " + file->get_name(), "FileSystem::untag_file()");
    }
    
    /*find tagTree*/
    TreeObject* tagTree = _RootTree->find(tag);
    
    /*Remove Finode from tagTree*/
    tagTree->erase(originalFileName);
    
    /*Remove tag from Finode*/
    file->erase(tag);
    
    /*Note Tag Tree was modified*/
    insert_modification(tagTree);
  }
  
  /*if removed all Tags from file, add default tag*/
  if(file->size() == 0 && !deleting)
  {
    TreeObject* defaultTree = _RootTree->find("default");
    file->insert("default", defaultTree);
    vector<string> tagSet; tagSet.push_back("default");
    defaultTree->insert(file->mangle(tagSet), file);
    insert_modification(defaultTree);
  }
  
  /* write updated Finode to disk*/
  file->write_out();
}

void FileSystem::rename_tag(string originalTagName, string newTagName)
{
  /*Cannot rename tag to something that already exists*/
  TreeObject* tagTreeCheck = _RootTree->find(newTagName);
  if(tagTreeCheck != 0)
  {
    throw tag_error("new Tag name must not already exist!", " FileSystem::rename_tag()");
  }
  
  /*Rename the tagTree*/
  TreeObject* tagTree = _RootTree->find(originalTagName);
  tagTree->set_name(newTagName);
  
  /*Note TagTree was modified*/
  insert_modification(tagTree);
  
  /*Change tagName in rootTree*/
  _RootTree->erase(originalTagName);
  _RootTree->insert(newTagName, tagTree);
  
  /*Note RootTree was modified*/
  insert_modification(_RootTree);
  
  
  /*Change tagName in every FileInfo object of the tagTree*/
  for(auto fileIt = tagTree->begin(); fileIt != tagTree->end(); fileIt++)
  {
    fileIt->second->erase(originalTagName);
    fileIt->second->insert(newTagName, tagTree);
    fileIt->second->write_out();
  }
}

FileInfo* FileSystem::create_file(string filename, unordered_set<string>& tags)
{
  /*Get a block from disk to store Finode*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  FileInfo* newFile = new FileInfo(filename, newblknum, _myPartitionManager);
  /* If no tag was specified then add file to "default" tag tree
   * File remains in default tag tree until a non-default tag is associated with file*/
  
  try
  {
    if(tags.size() == 0)
    {
      unordered_set<string> temp;
      temp.insert("default");
      
      tag_file(newFile, temp);
    }
    else
    {
      tag_file(newFile, tags);
    }
    
    newFile->write_out();
    _allFiles.insert(pair<string, FileInfo*>(filename, newFile));
  }
  catch(arboreal_exception& e)
  {
    delete newFile;
    throw;
  }

  return newFile;
}

void FileSystem::delete_file(FileInfo* file)
{
  string originalFileName = file->mangle();
  if(file == 0)
  {
    throw arboreal_logic_error("Invalid FileInfo*", "FileSystem::delete_file()");
  }
  
  unordered_set<string> tags = file->get_tags();
  
  /*Dissasociate all tags from the file*/
  untag_file(file, tags, true);

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
  objsToDelete.push_back(file);
}

void FileSystem::delete_file(vector<string>& filePath)
{
  FileInfo* file = path_to_file(filePath);
  delete_file(file);
}

int FileSystem::open_file(vector<string>& filePath, char mode)
{
  int fileDesc = -1;
    
  /*Find the file*/
  FileInfo* file = path_to_file(filePath);
  
  /*Create a FileOpen object corresponding to this file descriptor*/
  FileOpen* open_file = new FileOpen(file, mode, _myPartitionManager);
  
  /*Add the FileOpen* to the fileOpen table*/
  
  if(_fileOpenTable.size() <= MAXopen_fileS)
  {
    _fileOpenTable.push_back(open_file);
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
        _fileOpenTable[i] = open_file;
      }
    }
    if(fileDesc == -1)
    {
      /*Never found an open space. Reached maximum number of open files*/
      throw file_error("Reached maximum number of open Files", "FileSystem::open_file()");
    }
  }
  
  /*Return the index of the FileOpen* in the fileopen table*/
  return fileDesc;
}

void FileSystem::close_file(unsigned int fileDesc)
{
  if(fileDesc >= _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid file descriptor", "FileSystem::close_file");
  }
  
  /*Close the File*/
  delete _fileOpenTable[fileDesc]; _fileOpenTable[fileDesc] = 0;
}

size_t FileSystem::read_file(unsigned int fileDesc, char* data, size_t len)
{
  if(len == 0)
  {
    return 0;
  }
  
  /*Start read_ing at seek pointer*/
  if(fileDesc > _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid File descriptor", "FileSystem::write_file");
  }
  bool set_EOF = false;//whether we will read past the end of the file
  FileOpen* open_file = _fileOpenTable[fileDesc];
  Index currentIndex;
  size_t dataOffset = 0;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  if(open_file->getMode() != 'r' && open_file->getMode() != 'x')
  {
    delete[] buff;
    throw file_error("File not opened with read permissions", "FileSystem::write_file");
  }
  
  /*If seek pointer is past the end of the file, throw error*/
  if(open_file->get_EOF())
  {
    delete[] buff;
    throw file_error("Attempt to read past EOF", "FileSystem::read_file()");
  }
  
  /*Get the index we need to start read_ing from*/
  currentIndex = open_file->byte_to_index(0);
  if(currentIndex.blknum == 0)
  {
//     throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::read_file()");
    open_file->set_EOF();
    delete[] buff;
    return 0;
  }
  
  /*Set len so we don't actually read past the end of valid data*/
  if(len > (open_file->getFile()->get_file_size() - (open_file->getSeek() - 1)))
  {
    len = open_file->getFile()->get_file_size() - open_file->getSeek(); 
    set_EOF = true;
  }
  
  /*Read data remaining in current block*/
  size_t bytesToRead = _myPartitionManager->getBlockSize() - currentIndex.offset;

  
  if(len <= bytesToRead)
  {
    try{_myPartitionManager->readDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    memcpy(data, buff + currentIndex.offset, len);
    open_file->increment_seek(len);
    if(set_EOF)
    {
      open_file->set_EOF();
    }
    delete[] buff;
    return len;
  }
  else
  {
    try{_myPartitionManager->readDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    memcpy(data, buff + currentIndex.offset, bytesToRead);
    open_file->increment_seek(bytesToRead);
  }

  len-= bytesToRead; dataOffset+= bytesToRead;
  
  /*Read as many full blocks as we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    currentIndex = open_file->byte_to_index(0);
    if(currentIndex.blknum == 0)
    {
      throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::read_file()");
    }
    
    /*Read a full block*/
    try{_myPartitionManager->readDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    memcpy(data + dataOffset, buff + currentIndex.offset, _myPartitionManager->getBlockSize());
    
    open_file->increment_seek(_myPartitionManager->getBlockSize());
    dataOffset+= _myPartitionManager->getBlockSize(); len-= _myPartitionManager->getBlockSize();
  }
  
  /*Read any leftover bytes*/
  if(len > 0)
  {
    currentIndex = open_file->byte_to_index(0);
    if(currentIndex.blknum == 0)
    {
      throw arboreal_logic_error("A supposedly valid seek returned no valid index", "FileSystem::read_file()");
    }
    
    /*Read len bytes*/
    try{_myPartitionManager->readDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    memcpy(data + dataOffset, buff + currentIndex.offset, len);
    open_file->increment_seek(len);
    dataOffset+= len;
  }
  
  if(set_EOF)
  {
    open_file->set_EOF();
  }
  delete[] buff;
  return dataOffset;
}

size_t FileSystem::write_file(unsigned int fileDesc, const char* data, size_t len)
{
  /*Start writing at seek pointer*/
  
  if(fileDesc > _fileOpenTable.size() || _fileOpenTable[fileDesc] == 0)
  {
    throw file_error("Invalid File descriptor", "FileSystem::write_file");
  }
  
  FileOpen* open_file = _fileOpenTable[fileDesc];
  Index currentIndex;
  size_t dataOffset = 0;
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  if(open_file->getMode() != 'w' && open_file->getMode() != 'x')
  {
    delete[] buff;
    throw file_error("File not opened with write permissions", "FileSystem::write_file");
  }

  /*If we are past the end of the File, go to the last byte to start writing.*/
  if(open_file->get_EOF())
  {
    delete[] buff;
    throw file_error("Attempt to write past EOF", "FileSystem::write()");
  }
  
  /*Get the index we need to start writing to, 1 past the seek pointer*/
  currentIndex = open_file->byte_to_index(0);
  if(currentIndex.blknum == 0)
  {
    /*There is not space allocated yet, increment Index*/
    currentIndex = open_file->increment_index();
  }
  
  /*Fill up current Block, must preserve data already there*/
  try{_myPartitionManager->readDiskBlock(currentIndex.blknum, buff);}
  catch(arboreal_exception& e) { delete[] buff; throw; }
  size_t bytesToWrite = _myPartitionManager->getBlockSize() - currentIndex.offset;
  
  if(len <= bytesToWrite)
  {
    memcpy(buff + currentIndex.offset, data, len);
    try{_myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    open_file->increment_seek(len, true); open_file->getFile()->set_edit();
    delete[] buff;
    return len;
  }
  else
  {
    memcpy(buff + currentIndex.offset, data, bytesToWrite);
    try{_myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    open_file->increment_seek(bytesToWrite, true); open_file->getFile()->set_edit();
  }

  len-= bytesToWrite;
  dataOffset+= bytesToWrite;
  /*Write out all the remaining full blocks we can*/
  while(len >= _myPartitionManager->getBlockSize())
  {
    /*get Current Index*/
    if(len > 0)
    {
      currentIndex = open_file->byte_to_index(0);
      if(currentIndex.blknum == 0)
      {
        /*There is not space allocated yet, increment Index*/
        currentIndex = open_file->increment_index();
      }
    }
    
    /*Write a full block of data*/
    memcpy(buff, data + dataOffset, _myPartitionManager->getBlockSize());
    try{_myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    open_file->increment_seek(_myPartitionManager->getBlockSize(), true); open_file->getFile()->set_edit();
    len -= _myPartitionManager->getBlockSize(); dataOffset += _myPartitionManager->getBlockSize(); 
    
  }
  
  if(len > 0)
  {
    /*More, but not a full block of data to write*/
    currentIndex = open_file->byte_to_index(0);
    if(currentIndex.blknum == 0)
    {
      /*There is not space allocated yet, increment Index*/
      currentIndex = open_file->increment_index();
    }
    
    memset(buff, 0, _myPartitionManager->getBlockSize());
    memcpy(buff + currentIndex.offset, data + dataOffset, len);
    try{_myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);}
    catch(arboreal_exception& e) { delete[] buff; throw; }
    open_file->increment_seek(len, true); open_file->getFile()->set_edit();
    dataOffset += len; 
  }
  
  delete[] buff;
  return dataOffset;
}

size_t FileSystem::append_file(unsigned int fileDesc, const char* data, size_t len)
{
  /*Start writing at last byte of file*/
  
  if(fileDesc > _fileOpenTable.size())
  {
    throw file_error("Invalid File descriptor", "FileSystem::write_file");
  }
  
  FileOpen* open_file = _fileOpenTable[fileDesc];
  open_file->go_past_last_byte();
  return write_file(fileDesc, data, len);
}

void FileSystem::seek_file_absolute(unsigned int fileDesc, size_t offset)
{
  /*NOTE: to the outside world seek is 0 indexed.*/
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::seek_file_absolute");
  }
  FileOpen* open_file = _fileOpenTable.at(fileDesc);
  
  open_file->reset_seek();
  open_file->increment_seek(offset);
}

void FileSystem::seek_file_relative(unsigned int fileDesc, long int offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::seek_file_relative");
  }
  FileOpen* open_file = _fileOpenTable.at(fileDesc);
  
  if(offset < 0)
  {
    open_file->decrement_seek(-offset);
  }
  else if(offset > 0)
  {
    open_file->increment_seek(offset);
  }
}

Attributes* FileSystem::get_attributes(vector<string>& filePath)
{
  /*Find the file*/
  FileInfo* file = path_to_file(filePath);
  
  return file->get_attributes();
}

void FileSystem::set_permissions(vector<string>& filePath, char* perms)
{
  /*Find the file*/
  FileInfo* file = path_to_file(filePath);
  
  file->set_permissions(perms);
}

void FileSystem::rename_file(vector<string>& originalFilePath, string newFileName)
{
  
  /*Rename the file*/
  FileInfo* file = path_to_file(originalFilePath);
  file->set_name(newFileName);
  
  /*Change fileName in every TagTree object associated with the file*/
  for(auto tagIt = file->begin(); tagIt != file->end(); tagIt++)
  {
    tagIt->second->erase(file->get_name());
    tagIt->second->insert(newFileName, file);
  }
}

void FileSystem::write_changes()
{
  for(auto it = _modifiedObjects.begin(); it != _modifiedObjects.end(); it++)
  {
    it->first->write_out();
  }
}

/* Start Helper Functions */

void FileSystem::insert_modification(TreeObject* object)
{
  _modifiedObjects.insert(pair<TreeObject*, int>(object, 0));
}

FileInfo* FileSystem::path_to_file(vector<string>& fullPath)
{  
  if(fullPath.size() == 0)
  {
    throw arboreal_logic_error("path must at least have a file name", "FileSystem::path_to_file");
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
    throw tag_error(path[0] + "Does not Exist", "FileSystem::path_to_file");
  }
  
  FileInfo* temp = new FileInfo(filename, 0, _myPartitionManager);
  TreeObject* file = tagTree->find(temp->mangle(path));
  if(file == 0)
  {
    throw file_error("File Does not exist", "FileSystem::path_to_file");
  }
  
  delete temp;
  return (FileInfo*)file;
}

/*End Helper Functions*/
void FileSystem::print_root()
{
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    cout << "Key: " << it->first << " Value: " << it->second->get_block_number() << endl;
  }
}

void FileSystem::print_tags()
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

void FileSystem::print_files()
{
  for(auto it = _allFiles.begin(); it != _allFiles.end(); it++)
  {
    cout << "\t FilePath: " << it->second->mangle() << endl;//" \t\tBlockNumber: " << it->second->get_block_number() << endl;
  }
}

int FileSystem::get_file_name_size()
{
  return _myPartitionManager->get_file_name_size();
}


