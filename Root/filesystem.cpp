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
#include <vector>
#include <string>
bool EncryptionFlag = false;

FileOpen::FileOpen(FileInfo* file, char mode): _file(file), _mode(mode)
{
  _seek = 0;
  _EOF = false;
}

FileInfo* FileOpen::getFile(){return _file;}
long unsigned int FileOpen::getSeek(){return _seek;}
char FileOpen::getMode(){return _mode;}
bool FileOpen::getEOF(){return _EOF;}

void FileOpen::incrementSeek(long unsigned int bytes)
{
  if(bytes >= _file->getFileSize() - _seek)
  {
    setEOF();
  }
  else
  {
    _seek += bytes;
  }
}

Index FileOpen::byteToIndex(PartitionManager* pm)
{
  int entriesPerBlock = pm->getBlockSize() / sizeof(BlkNumType);
  long unsigned int blockIndex = _seek / pm->getBlockSize(); //This will most likely have a remainder. Don't care yet.
  Index ret;
  std::vector<int> remainders;
  
  ret.offset = _seek % pm->getBlockSize();
  
  if(blockIndex < 12)
  {
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
        char* buff = new char[pm->getBlockSize()];
        
        /*Read in Level 1 indirect block*/
        if(_file->getFinode().level1Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        pm->readDiskBlock(_file->getFinode().level1Indirect, buff);
  
        /*blockIndex is the level 1 offest*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (blockIndex * sizeof(BlkNumType)), sizeof(BlkNumType));
        ret.blknum = blknum;
        break; 
      }
      case 1:
      {
        char* buff = new char[pm->getBlockSize()];
        
        /*Read in Level 2 indirect block*/
        if(_file->getFinode().level2Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        pm->readDiskBlock(_file->getFinode().level3Indirect, buff);
        
        /*blockIndex - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        pm->readDiskBlock(l1blknum, buff);
        
        /*remainders[0] is the level 1 offset*/
        BlkNumType blknum;
        memcpy(&blknum, buff + (remainders[0] * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        ret.blknum = blknum;
        
        break;
      }
      case 2:
      {
        char* buff = new char[pm->getBlockSize()];
        
        /*Read in Level 3 indirect block*/
        if(_file->getFinode().level1Indirect == 0)
        {
          throw arboreal_logic_error("Level 1 indirect block does not exist", "FileOpen::byteToIndex");
        }
        
        pm->readDiskBlock(_file->getFinode().level3Indirect, buff);
        
        /*blockIndex - 1 is the level 3 offset*/
        BlkNumType l2blknum;
        memcpy(&l2blknum, buff + ((blockIndex - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in level 2 indirect block*/
        pm->readDiskBlock(l2blknum, buff);
        
        /*remainders[1] - 1 is the level 2 offset*/
        BlkNumType l1blknum;
        memcpy(&l1blknum, buff + ((remainders[1] - 1) * sizeof(BlkNumType)), sizeof(BlkNumType));
        
        /*Read in Level 1 indirect block*/
        pm->readDiskBlock(l1blknum, buff);
        
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
  return ret;
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

FileSystem::FileSystem(DiskManager *dm, std::string fileSystemName)
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

std::vector<FileInfo*>* FileSystem::tagSearch(std::vector<std::string> tags)
{
  std::vector<FileInfo*>* ret = new std::vector<FileInfo*>;
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
    /*create std::vector of tagtrees that we want to search*/
    std::vector<TreeObject*> searchTrees;
    for(size_t i = 0; i < tags.size(); i++)
    {
      TreeObject* tagTree = _RootTree->find(tags[i]);
      if(tagTree == 0)
      {
        std::cerr << tags[i] + " excluded from search : Tag Does not exist" << std::endl;
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
  
  /* return std::vector of the found file(s)*/
  return ret;
}

std::vector<FileInfo*>* FileSystem::fileSearch(std::string name)
{
  /*We're going to use the _allFiles variable to find the files*/
  
  std::vector<FileInfo*>* ret = new std::vector<FileInfo*>;
  
  auto files = _allFiles.equal_range(name);
  
  for (auto it = files.first; it != files.second; it++) {
    ret->push_back(it->second);
  }
  
  return ret;
}

void FileSystem::createTag(std::string tagName)
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

void FileSystem::deleteTag(std::string tagName)
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

void FileSystem::mergeTags(std::string tag1, std::string tag2)
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

void FileSystem::tagFile(FileInfo* file, std::vector<std::string>& tags)
{
  /*Validate the tagging of this file first*/
  if(tags.size() == 0) {throw tag_error("No tags specified","FileSystem::tagFile");}
  if(file == 0) {throw file_error ("File Does not Exist", "FileSystem::tagFile");}
  
  std::vector<std::string> tagSet;
  TreeObject* tagTree = 0;
  
  for(size_t i = 0; i < tags.size(); i++)
  {
    /*Only add the tags that exist to the tagSet*/
    tagTree = _RootTree->find(tags[i]);
    if(tagTree == 0)
    {
      std::cerr << tags[i] << " Does Not Exist: Not added to file tag set" << std::endl;
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
  for(std::string tag : tagSet)
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
}

void FileSystem::untagFile(FileInfo* file, std::vector<std::string>& tags)
{
  for(std::string tag : tags)
  {
    if(tag == "default")
    {
      std::cerr << tag + " cannot be removed from " << file->getName() << std::endl;
    }
    
    /*find tagTree*/
    TreeObject* tagTree = _RootTree->find(tag);
    
    /*If tag does not exist print error and continue*/
    if(tagTree == 0)
    {
      std::cerr << tag + " cannot be removed from " << file->getName() << " : Tag Does not exist" << std::endl;
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

void FileSystem::renameTag(std::string originalTagName, std::string newTagName)
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

FileInfo* FileSystem::createFile(std::string filename, std::vector<std::string>& tags)
{
  /*Get a block from disk to store Finode*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  FileInfo* newFile = new FileInfo(filename, newblknum, _myPartitionManager);
  /* If no tag was specified then add file to "default" tag tree
   * File remains in default tag tree until a non-default tag is associated with file*/
  if(tags.size() == 0)
  {
    std::vector<std::string> temp;
    temp.push_back("default");
    
    tagFile(newFile, temp);
  }
  else
  {
    tagFile(newFile, tags);
  }
  
  newFile->writeOut();
  _allFiles.insert(pair<std::string, FileInfo*>(filename, newFile));
  
  return newFile;
}

void FileSystem::deleteFile(FileInfo* file)
{
  /*Assuming FileInfo* passed from calling code is valid*/
  std::vector<std::string> tags;
  for(auto tagIt = file->begin(); tagIt != file->end(); tagIt++)
  {
    tags.push_back(tagIt->first);
  }
  /*Dissasociate all tags from the file*/
  untagFile(file, tags);
  
  /*Delete file from disk*/
  file->del();
}

int FileSystem::openFile(std::vector<std::string>& filePath, char mode)
{
  /*Find the file*/
  FileInfo* file = pathToFile(filePath);
  
  /*Create a FileOpen object corresponding to this file descriptor*/
  FileOpen* openFile = new FileOpen(file, mode);
  
  /*Add the FileOpen* to the fileOpen table*/
  _fileOpenTable.push_back(openFile);
  
  /*Return the index of the FileOpen* in the fileopen table*/
  return _fileOpenTable.size() - 1;
}

void FileSystem::closeFile(unsigned int fileDesc)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  
  /*Close the File*/
  _fileOpenTable.erase(_fileOpenTable.begin() + fileDesc);
}

long unsigned int FileSystem::readFile(unsigned int fileDesc, char* data, long unsigned int len)
{return 0;}

long unsigned int FileSystem::writeFile(unsigned int fileDesc, const char* data, long unsigned int len)
{return 0;}

long unsigned int FileSystem::appendFile(unsigned int fileDesc, const char* data, long unsigned int len)
{return 0;}

void FileSystem::seekFileAbsolute(unsigned int fileDesc, long unsigned int offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  openFile->resetSeek();
  openFile->incrementSeek(offset);
}

void FileSystem::seekFileRelative(unsigned int fileDesc, long unsigned int offset)
{
  if(fileDesc >= _fileOpenTable.size())
  {
    throw file_error("Invalid file descriptor", "FileSystem::closeFile");
  }
  FileOpen* openFile = _fileOpenTable.at(fileDesc);
  
  openFile->incrementSeek(offset);
}

Attributes* FileSystem::getAttributes(std::vector<std::string>& filePath)
{return 0;}

void FileSystem::setAttributes(std::vector<std::string>& filePath, Attributes* atts)
{return;}

void FileSystem::renameFile(std::vector<std::string>& originalFilePath, std::string newFileName)
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

FileInfo* FileSystem::pathToFile(std::vector<std::string>& fullPath)
{  
  if(fullPath.size() == 0)
  {
    throw arboreal_logic_error("path must at least have a file name", "FileSystem::pathToFile");
  }
  
  std::vector<std::string> path;
  std::string filename = fullPath.at(fullPath.size() - 1);
  
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
    cout << "Key: " << it->first << " Value: " << it->second->getBlockNumber() << std::endl;
  }
}

void FileSystem::printTags()
{
  for(auto it = _RootTree->begin(); it != _RootTree->end(); it++)
  {
    cout << "TagName: " << it->first << " \tBlockNumber: " << it->second->getBlockNumber() << std::endl;
    
    for(auto it2 = it->second->begin(); it2 != it->second->end(); it2++)
    {
      cout << "\t FilePath: " << ((FileInfo*)(it2->second))->mangle() << " \tBlockNumber: " << it2->second->getBlockNumber() << std::endl;
    }
  }
}

void FileSystem::printFiles()
{
  for(auto it = _allFiles.begin(); it != _allFiles.end(); it++)
  {
    cout << "\t FilePath: " << it->second->mangle() << std::endl;//" \t\tBlockNumber: " << it->second->getBlockNumber() << std::endl;
  }
}

int FileSystem::getFileNameSize()
{
  return _myPartitionManager->getFileNameSize();
}


