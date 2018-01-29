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

FileSystem::FileSystem(DiskManager *dm, string fileSystemName)
{
  _FSName = fileSystemName;
  
  /* set partition manager for my partition */
  _myPartitionManager = new PartitionManager(dm, fileSystemName);
  
  /*Read in the root tree*/
  _RootTree = new RootTree(_myPartitionManager);
  _RootTree->readIn(&_allFiles);
  
  /*Read in every tag Tree*/
  for(auto it = _RootTree->getMap()->begin(); it != _RootTree->getMap()->end(); it++)
  {
    it->second->readIn(&_allFiles);
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
    unordered_map<string, FileInfo*>* treeptr = it->second->getMap();
    
    for(auto fileIt = tagTree->begin(); fileIt != tagTree->end(); fileIt++)//Complexity: number of files in answer
    {
      ret->push_back(fileIt->second);
    }
    
  }
  else
  {    
    //   * Use size field in root node of tag tree to find smallest tree among the tags you want to search
    /*create vector of tagtrees that we want to search*/
    vector<TagTree*> searchTrees;
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
    
    TagTree* smallest = searchTrees[0];
    for(size_t i = 1; i < searchTrees.size(); i++)//Complexity: number of tags specified
    {
      if(searchTrees[i]->getMap()->size() < smallest->getMap()->size())
      {
        smallest = searchTrees[i];
      }
    }
    
    /*Search the smallest tree:*/
    for(auto fileIt = smallest->begin(); fileIt != smallest->end(); fileIt++)//Complexity: size of smallest specified tag tree * # of tags specified (worst case: size of smallest specified tag tree*# of tags specified* number of tags associated with file in question)
    {
      //   # elimnate all nodes with tag count < the number of tags you are searching for
      if(it->second->getMap()->size() >= tags.size())
      {
        //could be a matching file
        bool match = true;
        //   # search remainng files for exact tag match O(1) average case, worst case O(n)
        for(unsigned int i = 0; i < tags.size(); i++)//Complexity: # of tags specified(worst case: # of tags specified* number of tags associated with file in question)
        {
          if(it->second->getMap()->find(tags[i]) == it->second->getMap()->end()) //complexity: avg. O(1), worst number of tags associated with file in question
          {
            match = false;
            break;
          }
        }
        if(match)
        {
          ret->push_back(it->second);
        }
      }
    }
  }
  
  //   # return (list) the found file(s)
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
  /*Get a block from disk to store tag tree*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();//Complexity: not our fucking problem, but linear with # of blocks on partition
  
  //   - initialize tree in main memory
  TagTree* newTree = new TagTree(tagName, newblknum, _myPartitionManager);
  //   - add respective node to root tree (write TagTree block num to Node as well as TagTree memory address to Node)
  
  auto ret = _RootTree->getMap()->insert(pair<string, TagTree*>(tagName, newTree));//Complexity: avg. 1, worst # of tags in filesystem
  if(!ret.second)
  {
    throw tag_error (tagName + " is not unique", "FileSystem::createTag");
  }
  
  /*Keep track of addition to RootTree*/
  _RootTree->insertAddition(newTree);
  /*Note Root Tree was modified*/
  insertModification(_RootTree);
  
  /*Write out newly created TagTree. it will only write out the TagTree superblock*/
  newTree->writeOut(); //Complexity: size of tag tree, this case 0;
}

void FileSystem::deleteTag(string tagName)
{
  /*find tagTree*/
  auto tagTreeIt = _RootTree->getMap()->find(tagName);//Complexity: avg 1, worst # tags on system
  if(tagTreeIt == _RootTree->getMap()->end())
  {
    throw tag_error(tagName + " Does Not Exist", "FileSystem::deleteTag");
  }
  
  TagTree* tagTree = tagTreeIt->second;
  unordered_map<string, FileInfo*>* tagTreeMap = tagTree->getMap();
  
  /* CANNOT delete tag if tag tree Size > 0 */
  if(tagTreeMap->size() > 0)
  {
    throw tag_error(tagName + " cannot be deleted: Tag has files associated with it", "FileSystem::deleteTag");
  }
  
  /*Delete tagTree on disk*/
  tagTree->del();
  /*Delete Node from Root Tree*/
  _RootTree->insertDeletion(tagTree);
  _RootTree->getMap()->erase(tagTreeIt);
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
  //for all tags in the vector
  /*Validate the tagging of this file first*/
  if(tags.size() == 0) {throw tag_error("No tags specified","FileSystem::tagFile");}
  if(file == 0) {throw file_error ("File Does not Exist", "FileSystem::tagFile");}
  
  bool found = true;
  auto tagTreeptr = _RootTree->getMap()->find(tags[0]);
  if(tagTreeptr == _RootTree->getMap()->end())
  {
    cerr << tags[0] << " Does Not Exist: Not added to file tag set" << endl;
    found = false;
  }
  
  size_t i = 1;
  while((!found) && (i < tags.size()))
  {
    tagTreeptr = _RootTree->getMap()->find(tags[i]);
    if(tagTreeptr == _RootTree->getMap()->end())
    {
      cerr << tags[i] << " Does Not Exist: Not added to file tag set" << endl;
      found = false;
    }
    else
    {
      found = true;
    }
    i++;
  }
  
  if(found == false)
  {
    return;
  }
  
  auto ret = tagTreeptr->second->getMap()->find(file->mangle(tags));
  if(ret != tagTreeptr->second->getMap()->end())
  {
    throw file_error(file->getName() + " with the specified tags already exists", "FileSystem::tagFile");
  }
  
  for(auto t : tags)//Complexity: avg:number of tags specified , worst: number of tags specified * size of root tree
  {
    /*find tagTree*/
    auto it = _RootTree->getMap()->find(t);//Complexity: avg: 1, worst: size of root tree
    
    if(it == _RootTree->getMap()->end()) 
    {
      cerr << t << " Does Not Exist: Not added to file tag set" << endl;
    }
    else
    {
      unordered_map<string, FileInfo*>* treeptr = it->second->getMap();
      
      /*Add Tag to Finode */
      auto ret = file->getMap()->insert(pair<string, BlkNumType>(t, it->second->getBlockNumber()));//Complexity: Complexity: avg: 1, worst: avg number of tags associated iwth file
      if(!ret.second)
      {
        cerr << "File already tagged with " << t << " : Only Tagged once" << endl;
      }
      
      //   - Create and Add new Node to TagTree
      auto ret2 = treeptr->insert(pair<string, FileInfo*>(file->mangle(tags), file));//Complexity: Complexity: avg: 1, worst: size of tag tree
      if(!ret2.second)
      {
        throw arboreal_logic_error(file->getName() + " with the specified tags already exists \n THIS SHOULD HAVE BEEN TRIGGERED EARLIER", "FileSystem::tagFile");
        //NOTE: this is a duplicate error check, but we check above to save a little time and not create a new file inode unnecesarily. 
        //NOTE: That is also why this is a logic error
      }
      
      /*Keep track of addition*/
      it->second->insertAddition(file);
      /*Note a TagTree was modified*/
      insertModification(it->second);
    }
  }
  
  // (write updated Finode to disk)
  file->writeOut();
  
}

void FileSystem::untagFile(FileInfo* file, vector<string> tags)
{
  //for all tags in the vector
  for(auto t : tags)
  {
    //   - find tagTree
    
    auto it = _RootTree->getMap()->find(t);//Complexity: avg: 1, worst: size of root tree
    //   - If tag does not exist print error and continue
    if(it == _RootTree->getMap()->end())
    {
      cerr << t + " cannot be removed from " << file->getName() << " : Tag Does not exist" << endl;
    }
    unordered_map<string, FileInfo*>* treeptr = it->second->getMap();
    
    /*Keep track of deletion*/
    it->second->insertDeletion(treeptr->find(file->getName())->second);
    
    //   - Remove Node from TagTree
    treeptr->erase(file->getName()); //Complexity: Complexity: avg: 1, worst: size of tag tree
    /*Note a Tag Tree was modified*/
    insertModification(it->second);
    //   - Delete tag from FileInode
    file->getMap()->erase(t);//Complexity: Complexity: avg: 1, worst: avg number of tags associated iwth file
    
  }
  
  // (write updated Finode to disk)
  file->writeOut();
}


FileInfo* FileSystem::createFile(string filename, vector<string>& tags)
{
  /*Get a block from disk to store FInode*/
  
  BlkNumType newblknum = 0;
  newblknum = _myPartitionManager->getFreeDiskBlock();
  
  FileInfo* newFile = new FileInfo(filename, newblknum, _myPartitionManager);
  //   - If tag not given then add file to "default" tag tree
  //   * File remains in default tag tree until a non-default tag is associated with file
  
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
  //   NOTE: File will be referenced by Finode block number(fidentifier)
  //   - check to make sure file exists, you have a FileInfo*, so file must exist
  vector<string> tags;
  auto fileTags = file->getMap();
  for(auto it = fileTags->begin(); it != fileTags->end(); it++)//Complexity: number of tags associated with file
  {
    tags.push_back(it->first);
  }
  //   - dissasociate all tags from the file (Call untagFile())
  untagFile(file, tags);
  
  //   - Free all data blocks
  //   - Free Finode block on disk
  file->del();
  
  delete file;
  file = 0;
}

int FileSystem::openFile(char *filename, int fnameLen, char mode, int lockId)
{
  return 0;
}

int FileSystem::closeFile(int fileDesc)
{return 0;}

int FileSystem::readFile(int fileDesc, char *data, int len)
{return 0;}

int FileSystem::writeFile(int fileDesc, const char *data, int len)
{return 0;}

int FileSystem::appendFile(int fileDesc, char *data, int len)
{return 0;}

int FileSystem::seekFile(int fileDesc, int offset, int flag)
{return 0;}

int FileSystem::renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2)
{return 0;}

int FileSystem::getAttributes(char *filename, int fnameLen, char* buffer, int flag)
{return 0;}

int FileSystem::setAttributes(char *filename, int fnameLen, char* buffer, int flag)
{return 0;}

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

/*End Helper Functions*/
void FileSystem::printRoot()
{
  for(auto it = _RootTree->getMap()->begin(); it != _RootTree->getMap()->end(); it++)
  {
    cout << "Key: " << it->first << " Value: " << it->second->getBlockNumber() << endl;
  }
}

void FileSystem::printTags()
{
  for(auto it = _RootTree->getMap()->begin(); it != _RootTree->getMap()->end(); it++)
  {
    cout << "TagName: " << it->first << " \tBlockNumber: " << it->second->getBlockNumber() << endl;
    
    for(auto it2 = it->second->getMap()->begin(); it2 != it->second->getMap()->end(); it2++)
    {
      cout << "\t FilePath: " << it2->second->mangle() << " \tBlockNumber: " << it2->second->getBlockNumber() << endl;
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

bool operator==(const FileOpen& lhs, const FileOpen& rhs)
{
  if((lhs.finodeblk == rhs.finodeblk)) return true;
  else return false;
}



