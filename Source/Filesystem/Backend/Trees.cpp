///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Trees.cpp
//  TreeObject Source File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Trees.h"
#include <time.h>
#include <string>
#include <string.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <unordered_set>
using std::unordered_set;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::pair;

#define DEFAULTOWNER 1
#define DEFAULTPERMISSIONS 0

bool operator ==(Index& lhs, Index& rhs)
{
  return ((lhs.blknum == rhs.blknum) && (lhs.offset == rhs.offset));
}

bool operator !=(Index& lhs, Index& rhs)
{
  return !(lhs == rhs);
}
/******************************************************************************/

Modification::Modification(TreeObject* obj, TreeObject* parent)
{
  _mod = obj;
  _parent = parent;
}

Modification::~Modification(){}

/******************************************************************************/

Attributes::Attributes(BlkNumType blknum, PartitionManager* pm): _blockNumber(blknum), _myPartitionManager(pm)
{
  memset(&_atts, 0, sizeof(FileAttributes));
}


void Attributes::write_out()
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize());
  memcpy(buff, &_atts, sizeof(FileAttributes));
  _myPartitionManager->writeDiskBlock(_blockNumber, buff);
  delete[] buff;
}

void Attributes::read_in()
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  _myPartitionManager->readDiskBlock(_blockNumber, buff);
  memcpy(&_atts, buff, sizeof(FileAttributes));
  delete[] buff;
}

void Attributes::del()
{
  _myPartitionManager->returnDiskBlock(_blockNumber);
}

void Attributes::set_creation_time()
{
  _atts.creationTime = time(0);
  write_out();
}
void Attributes::set_owner(int owner)
{
  _atts.owner = owner;
  write_out();
}
void Attributes::set_permissions(char* perms)
{
  if(perms == 0)
  {
    char dperms[12] = {0,0,0,1,1,0,1,0,0,0,0,0};
    memcpy(_atts.permissions, dperms, 12);
  }
  else
  {
    memcpy(_atts.permissions, perms, 12);
  }
  write_out();
  
}
void Attributes::set_access()
{
  _atts.lastAccess = time(0);
  write_out();
}
void Attributes::set_edit()
{
  _atts.lastEdit = time(0);
  write_out();
}
void Attributes::update_size(size_t size)
{
  _atts.size = size;
  write_out();
}

/*Accessor Functions*/
time_t Attributes::get_creation_time(){return _atts.creationTime;}
int Attributes::get_owner(){return _atts.owner;}
char* Attributes::get_permissions(){return _atts.permissions;}
time_t Attributes::get_access(){return _atts.lastAccess;}
time_t Attributes::get_edit(){return _atts.lastEdit;}
size_t Attributes::get_size(){return _atts.size;}
FileAttributes Attributes::get_file_attributes(){return _atts;}

/******************************************************************************/


Addition::Addition(TreeObject* obj, TreeObject* parent):Modification(obj, parent)
{}

Addition::~Addition(){}

void Addition::write_out(PartitionManager* pm)
{  
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  Index nextEntry{0,0};
  
  
  pm->readDiskBlock(_parent->get_last_entry().blknum, buff);
  
  if(_parent->get_last_entry().blknum == 0)
  {
    nextEntry.blknum = _parent->get_start_block();
    nextEntry.offset = 0;
  }
  else
  {
    nextEntry = _parent->get_last_entry();
    _parent->increment_allocate(&nextEntry);
  }
  
  if(_parent->get_last_entry().blknum != nextEntry.blknum)
  {
    pm->readDiskBlock(nextEntry.blknum, buff);
  }
  
  int keySize = _mod->get_name().length();
  strncpy(buff + nextEntry.offset, _mod->get_name().c_str(), keySize);
  memset(buff + nextEntry.offset + keySize, 0, pm->get_file_name_size() - keySize);
  
  /*write out the blocknumber of the tagTree/Finode to buffer*/
  BlkNumType tagBlk = _mod->get_block_number();
  memcpy(buff + nextEntry.offset + pm->get_file_name_size(), &tagBlk, sizeof(BlkNumType));
  
  _parent->set_last_entry(nextEntry);
  
  _mod->add_index(_parent, _parent->get_last_entry());
  
  pm->writeDiskBlock(_parent->get_last_entry().blknum, buff);
  
  delete[] buff;
}
/******************************************************************************/

Deletion::Deletion(TreeObject* obj, TreeObject* parent):Modification(obj, parent)
{}

Deletion::~Deletion(){}

void Deletion::write_out(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  if(_mod->get_index(_parent).blknum == 0)
  {
    if(DEBUG) cerr << "Tag Tree was never written out. Not really an error, just here for debugging" << endl;
    //NOTE: even though it is just here for debugging don't remove the if, just the printout.
  }
  else
  {        
    pm->readDiskBlock(_mod->get_index(_parent).blknum, buff);
      
    /*Zero out name and blocknumber*/
    memset(buff + _mod->get_index(_parent).offset, 0, pm->get_file_name_size() + sizeof(BlkNumType));
    
    /*Write out buff to mod blknum*/
    pm->writeDiskBlock(_mod->get_index(_parent).blknum, buff);
  }
  
  delete[] buff;
}

/******************************************************************************/
TreeObject::TreeObject(string name, BlkNumType blknum, PartitionManager* pm)
:_name(name), _blockNumber(blknum), _myPartitionManager(pm)
{}

TreeObject::~TreeObject(){}

string TreeObject::get_name() const {return _name;}

void TreeObject::set_name(string name){_name = name;}

void TreeObject::add_index(TreeObject* obj, Index index)
{
  Index temp{index.blknum, index.offset};
  _indeces.insert(pair<TreeObject*, Index>(obj, temp));
}

Index TreeObject::get_index(TreeObject* obj) const
{
  auto indexIt = _indeces.find(obj);
  if(indexIt == _indeces.end())
  {
    throw arboreal_logic_error("TreeObject* does not exist in Index map", "TreeObject::get_index");
  }
  return indexIt->second;
}

BlkNumType TreeObject::get_block_number() const {return _blockNumber;}

Index TreeObject::get_last_entry() const {return _lastEntry;}

BlkNumType TreeObject::get_start_block() const {return _startBlock;}

void TreeObject::set_last_entry(Index index){_lastEntry = index;}


void TreeObject::increment_allocate(Index* index)
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  int entrySize = _myPartitionManager->get_file_name_size() + (sizeof(BlkNumType));
  
  if((_myPartitionManager->getBlockSize() - index->offset - entrySize) < (entrySize + sizeof(BlkNumType)))
  {
    BlkNumType newBlock;
    newBlock = _myPartitionManager->getFreeDiskBlock();
    
    /*Read in old block*/
    _myPartitionManager->readDiskBlock(index->blknum, buff);
    
    /*set continuation on old block*/
    memcpy(buff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), &newBlock, sizeof(BlkNumType));
    
    /*Write out old block*/
    _myPartitionManager->writeDiskBlock(index->blknum, buff);
    
    index->blknum = newBlock;
    index->offset = 0;
  }
  else
  {
    index->offset += entrySize;
  }
  delete[] buff;
}

void TreeObject::increment_follow(Index* index)
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  int entrySize = _myPartitionManager->get_file_name_size() + (sizeof(BlkNumType));
  
  if((_myPartitionManager->getBlockSize() - index->offset - entrySize) < entrySize + sizeof(BlkNumType))
  {
    /*Read in index.blknum*/
    _myPartitionManager->readDiskBlock(index->blknum, buff);
    
    /*Read in cont. blocknum */
    BlkNumType contBlkNum;
    memcpy(&contBlkNum, buff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
    
    if(contBlkNum == 0)
    {
      index->blknum = 0;
      index->offset = 0;
    }
    else
    {
      index->blknum = contBlkNum;
      index->offset = 0;
    }
  }
  else
  {
    index->offset += entrySize;
  }
  delete[] buff;
}

void TreeObject::delete_cont_blocks(BlkNumType blknum)
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  /*Read in the block passed*/
  _myPartitionManager->readDiskBlock(blknum, buff);
  
  /*Check for cont block*/
  BlkNumType contBlkNum;
  memcpy(&contBlkNum, buff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
  
  if(contBlkNum != 0)
  {
    delete_cont_blocks(contBlkNum);
  }
  
  _myPartitionManager->returnDiskBlock(blknum);
  
  delete[] buff;
}

void TreeObject::insert_addition(TreeObject* add)
{
  if(add != 0)
  {
    _modifications.push(new Addition(add, this));
  }
}

void TreeObject::insert_deletion(TreeObject* del)
{
  if(del != 0)
  {
    _modifications.push(new Deletion(del, this));
  }
}

size_t TreeObject::size() const {return _myTree.size();}

unordered_map<string, TreeObject*>::iterator TreeObject::begin() {return _myTree.begin();}

unordered_map<string, TreeObject*>::iterator TreeObject::end() {return _myTree.end();}

void TreeObject::insert(string name, TreeObject* obj)
{
  auto ret = _myTree.insert(pair<string, TreeObject*>(name, obj));
  if(!ret.second)
  {
    throw tag_error (name + " is not unique", "TreeObject::insert");
  }
  
  /*Keep track of addition to TreeObject*/
  this->insert_addition(obj);
}

void TreeObject::erase(string name)
{
  auto object = _myTree.find(name);
  if(object == _myTree.end())
  {
    throw arboreal_logic_error(name + " Does not exist in the TagTree " + _name, "TreeObject::erase");
  }
  this->insert_deletion(object->second);
  _myTree.erase(object);
}

TreeObject* TreeObject::find(string name) const
{
  auto it = _myTree.find(name);
  if(it == _myTree.end())
  {
    return 0;
  }
  return it->second;
}


/******************************************************************************/
RootTree::RootTree(PartitionManager* pm):TreeObject("Root", 1, pm)
{
  _lastEntry.blknum = 1;
  _lastEntry.offset = 0;
  _startBlock = 0;
}

RootTree::~RootTree()
{
  for(auto tag = _myTree.begin(); tag != _myTree.end(); tag++)
  {
    delete tag->second;
  }
}

void RootTree::del()
{
  throw arboreal_logic_error("Attempt to delete Root Tree", "RootTree::del");
}

void RootTree::write_out()
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory

  /******************************************************************************/

  /*Write out modifications*/
  size_t queueSize = _modifications.size();
  
  for(size_t i = 0; i < queueSize; i++)
  {
    _modifications.front()->write_out(_myPartitionManager);
    delete (_modifications.front());
    _modifications.pop();
  }
  
  /******************************************************************************/
  
  /*Write out Root super block*/
  
  Index currentIndex{_blockNumber, 0};
  
  /*_blockNumber is the super block for the Root Tree */
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.size());
  currentIndex.offset+=  _name.size() + 1;
  
  size_t treeSize = _myTree.size();
  
  RootSuperBlock rootInfo{treeSize, _lastEntry, _startBlock};
  
  memcpy(buff + currentIndex.offset, &rootInfo, sizeof(RootSuperBlock));
  currentIndex.offset+=  sizeof(RootSuperBlock);
  
  /*Write out RootTree superblock*/
  _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
  
  delete[] buff;
}

void RootTree::read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree)
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  
  /*Read in RootTree superblock*/
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
  
  /*Store values from Root Tree superblock*/
//   _name.assign(buff + currentIndex.offset, 5);
  currentIndex.offset+=  5;
    
  RootSuperBlock rootInfo;
  
  memcpy(&rootInfo, buff + currentIndex.offset, sizeof(RootSuperBlock));
  currentIndex.offset+=  sizeof(RootSuperBlock);
  
  
  _lastEntry.blknum = rootInfo.lastEntry.blknum;
  _lastEntry.offset = rootInfo.lastEntry.offset;
  _startBlock = rootInfo.startBlock;
  /****************************************************************************/
  
  /*Read in the startBlock, first block with entries*/
  if(_startBlock == 0)
  {
    delete[] buff;
    throw arboreal_logic_error("Root tree Start Block zero. Root Tree can never be empty becuase of the default tree", 
                               "RootTree::read_in");
  }
  
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);

  Index EOFIndex{0,0};
  
  while(currentIndex != EOFIndex)
  {
    if(buff[currentIndex.offset] != 0)
    {
      string tagName;
      BlkNumType blknum;
      
      /*Read in tagName*/
      tagName.assign(buff + currentIndex.offset, _myPartitionManager->get_file_name_size());
      tagName = tagName.substr(0, tagName.find_first_of('\0'));
      
      /*Read in blocknumber for that TagTree object*/
      memcpy(&blknum, buff + currentIndex.offset + _myPartitionManager->get_file_name_size() , sizeof(BlkNumType));
      if(blknum == 0)
      {
        delete[] buff;
        throw arboreal_logic_error("blocknumber for a tag tree is zero as read from disk in the root tree", 
                                   "RootTree::read_in");
      }
      
      /*Create TagTree object*/
      TagTree* tagTree = new TagTree(tagName, blknum, _myPartitionManager);
      tagTree->add_index(this, currentIndex);

      
      /*Insert key and value into tagtree in memory*/
      auto it_ret = _myTree.insert(pair<string, TagTree*>(tagName, tagTree));
      if(!it_ret.second)
      {
        delete tagTree;
        throw arboreal_logic_error("Duplicate Tag Tree read in from disk", "RootTree::read_in");
      }
    }
    
    increment_follow(&currentIndex);
    if(currentIndex != EOFIndex)
    {
      _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    }
  }
  
  delete[] buff;
}
/******************************************************************************/

TagTree::TagTree(string tagName, BlkNumType blknum, PartitionManager* pm)
:TreeObject(tagName, blknum, pm)
{
  _lastEntry.blknum = 0;
  _startBlock = 0;
}

TagTree::~TagTree()
{}

void TagTree::write_out()
{  
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  if(_startBlock == 0)
  {
    /*Need to allocate a new block to start appending*/
    BlkNumType newblknum = 0;
    newblknum = _myPartitionManager->getFreeDiskBlock();
    _lastEntry.blknum = 0;
    _lastEntry.offset = 0;
    _startBlock = newblknum;
  }
  /****************************************************************************/
  
  /*Write out modifications*/
  size_t queueSize = _modifications.size();
  
  for(size_t i = 0; i < queueSize; i++)
  {
    _modifications.front()->write_out(_myPartitionManager);
    delete (_modifications.front());
    _modifications.pop();
  }
  
  /****************************************************************************/
  
  Index currentIndex{_blockNumber, 0};
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  /*_blockNumber is the super block for this tagTree*/
  /*Layout - 
   * TagName - fileNameSize - 64 for now
   * Size - size_t - 4 ?
   * Index of end of tag tree on disk - sizeof(Index)
   * startBlocknumber - start of entries on disk - sizeof(BlkNumType)
   */
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.size());
  currentIndex.offset+= _myPartitionManager->get_file_name_size();
  
  TagTreeSuperBlock tagInfo{_myTree.size(), _lastEntry, _startBlock};
  memcpy(buff + currentIndex.offset, &tagInfo, sizeof(TagTreeSuperBlock));
  currentIndex.offset+=  sizeof(TagTreeSuperBlock);
  
  /*Write out TagTree superblock*/
  _myPartitionManager->writeDiskBlock(currentIndex.blknum, buff);
  
  delete[] buff;
}

void TagTree::read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree)
{
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  
  /*Read in tagTree superblock*/
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
  
  /*Store values from superblock*/
  _name.assign(buff + currentIndex.offset, _myPartitionManager->get_file_name_size());
  _name = _name.substr(0, _name.find_first_of('\0'));
  currentIndex.offset+=  _myPartitionManager->get_file_name_size();
  
  TagTreeSuperBlock tagInfo;
  memcpy(&tagInfo, buff + currentIndex.offset, sizeof(TagTreeSuperBlock));
  currentIndex.offset+=  sizeof(TagTreeSuperBlock);
  
  _lastEntry.blknum = tagInfo.lastEntry.blknum;
  _lastEntry.offset = tagInfo.lastEntry.offset;
  _startBlock = tagInfo.startBlock;
  /****************************************************************************/

  /*Read in the startBlock, first block with entries*/
  if(_startBlock == 0)
  {
    /*TagTree is empty*/
    delete[] buff;
    return;
  }
  
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  
  _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
  
  Index EOFIndex{0,0};
  
  while(currentIndex != EOFIndex)
  {
    if(buff[currentIndex.offset] != 0)
    {
      string fileName;
      BlkNumType blknum;
      
      /*Read in fileName*/
      fileName.assign(buff + currentIndex.offset, _myPartitionManager->get_file_name_size());
      fileName = fileName.substr(0, fileName.find_first_of('\0'));
      
      /*Read in blocknumber for that Finode object*/
      memcpy(&blknum, buff + currentIndex.offset + _myPartitionManager->get_file_name_size() , sizeof(BlkNumType));
      if(blknum == 0)
      {
        delete[] buff;
        throw arboreal_logic_error("blocknumber for a finode is zero as read from disk in a tag tree", 
                                   "TagTree::read_in");
      }
      
      /*Create FileInfo object*/
      FileInfo* finode = new FileInfo(fileName, blknum, _myPartitionManager);
      
      /*Read in the finode*/
      finode->read_in(allFiles, rootTree);
      
      bool found = false;
      
      /*Check to see if FileInfo object already existed. if so, set finode to 
       * already existing FileInfo object and delete finode.*/
      auto ret = allFiles->equal_range(fileName);
      for(auto it = ret.first; it != ret.second; it++)
      {
        if(it->second->mangle() == finode->mangle())
        {
          FileInfo* temp = finode;
          finode = it->second;
          delete temp; temp = 0;
          found = true;
          break;
        }
      }
      
      finode->add_index(this, currentIndex);
      
      /*Insert key and value into FileInfo object in memory*/
      auto it_ret = _myTree.insert(pair<string, FileInfo*>(finode->mangle(), finode));
      if(!(it_ret.second))
      {
        delete[] buff;
        throw arboreal_logic_error("Duplicate File read in from Disk", "TagTree:read_in");
      }
      
      /*add to allFiles, if it wasn't already there*/
      if(!found) 
      {
        allFiles->insert(pair<string, FileInfo*>(fileName, finode));
      }
    }
    
    increment_follow(&currentIndex);
    if(currentIndex != EOFIndex)
    {
      _myPartitionManager->readDiskBlock(currentIndex.blknum, buff);
    }
  }
  
  delete[] buff;
}

void TagTree::del()
{
  if(_myTree.size() > 0)
  {
    throw arboreal_logic_error("Attempt to delete nonempty TagTree", "TagTree::del");
  }
  
  char* buff = new char[_myPartitionManager->getBlockSize()];
  
  /*Read in the super block of this tagTree*/
  _myPartitionManager->readDiskBlock(_blockNumber, buff);
  
  TagTreeSuperBlock tagInfo;
  memcpy(&tagInfo, buff + _myPartitionManager->get_file_name_size(), sizeof(TagTreeSuperBlock));
  
  /* This value could be in memory. but it may differ, and we only care about 
   * what is written to disk. plus if we are deleting this tag tree anyway the 
   * values in memory will be handled elsewhere*/
  BlkNumType myStartBlock = tagInfo.startBlock; 
  
  if(myStartBlock != 0)
  {
    delete_cont_blocks(myStartBlock);
  }
  
  _myPartitionManager->returnDiskBlock(_blockNumber);
  
  delete[] buff;
}

/******************************************************************************/
FileInfo::FileInfo(string fileName,BlkNumType blknum, PartitionManager* pm)
:TreeObject(fileName, blknum, pm)
{
  memset(&_myFinode, 0, sizeof(Finode));
  _myAttributes = 0;
}

FileInfo::~FileInfo()
{
  if(_myAttributes != 0)delete _myAttributes;
  size_t queueSize = _modifications.size();
  for(size_t i = 0; i < queueSize; i++)
  {
    _modifications.front()->write_out(_myPartitionManager);
    delete (_modifications.front());
    _modifications.pop();
  }
}

void FileInfo::write_out()
{
  /* For this write out, we only need to write out the Finode, not anything to 
   * do with the files */
  
  /* File Inode Structure 
   * fileName
   * Atrributes Block
   * 12 Direct Blocks
   * 1 1st level Indirect block
   * 1 2nd level Indirect block
   * 1 3rd level Indirect block
   * Start local tag storage...
   * possible tag cont. block
   */
  
  //TODO: If there is a cont. block but we removed enough tags, we need to make sure to free that cont block
  
  /*Write out finode*/
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  memcpy(buff, _name.c_str(), _name.size());
  
  memcpy(buff + (_myPartitionManager->get_file_name_size()), &_myFinode, sizeof(Finode));
  
  /*This is the maximum number of tags we can store before needing a cont block*/
  size_t localTagCount = ((_myPartitionManager->get_file_name_size()) - sizeof(Finode) - sizeof(BlkNumType))
                        / sizeof(BlkNumType);
  
  
  /*Read in current Finode*/
  char* localBuff = new char[_myPartitionManager->getBlockSize()];
  memset(localBuff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  _myPartitionManager->readDiskBlock(_blockNumber, localBuff);
  
  BlkNumType contBlock = 0;
  memcpy(&contBlock, localBuff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
  
  int offset = _myPartitionManager->get_file_name_size() + sizeof(Finode);
  
  if(_myTree.size() <= localTagCount)
  {
    /*If there is a cont. block, free it*/
    if(contBlock != 0)
    {
      _myPartitionManager->returnDiskBlock(contBlock);
    }
    
    /*There is room to store all the tags locally*/
    for(auto it = _myTree.begin(); it != _myTree.end(); it++)
    {
      BlkNumType blknum = it->second->get_block_number();
      memcpy(buff + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
    }
  }
  else
  {
    /*There is not room to store all the tags locally*/
    auto it = _myTree.begin();
    
    for(size_t i = 0; i < localTagCount; i++)
    {
      /*Write out as many as we can*/
      BlkNumType blknum = it->second->get_block_number();
      memcpy(buff + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
      it++;
    }
    
    /*If there is already a cont. block, just overwrite it*/
    /*If not, allocate one and write to it.*/
    if(contBlock == 0)
    {
      contBlock = _myPartitionManager->getFreeDiskBlock();
      /*Write out the cont blocknum to the finode*/
      memcpy(buff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), &contBlock, sizeof(BlkNumType));
    }
    
    offset = 0;
    char* contBlockData = new char[_myPartitionManager->getBlockSize()];
    for(it = it; it != _myTree.end(); it++)
    {
      /*Write out the rest of the tags into the cont block*/
      BlkNumType blknum = it->second->get_block_number();
      memcpy(contBlockData + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
    }
    
    /*Write out the contBlock of tags*/
    _myPartitionManager->writeDiskBlock(contBlock, contBlockData);
  }
  delete[] localBuff;
  
  if(_myFinode.attributes == 0)
  {
    init_attributes();
  }
  /*Write out Finode*/
  _myPartitionManager->writeDiskBlock(_blockNumber, buff);
    
  /*Write out attributes*/
  _myAttributes->write_out();
  
  delete[] buff;
}

void FileInfo::read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree)
{
  /*Read in all the finode data*/
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize()); //zero out memory
  
  /*Read in Finode*/
  _myPartitionManager->readDiskBlock(_blockNumber, buff);
    
  memcpy(&_myFinode, buff + (_myPartitionManager->get_file_name_size()), sizeof(Finode));
  
  
  /*This is the maximum number of tags we can store before needing a cont block*/
  int localTagCount = ((_myPartitionManager->getBlockSize() - _myPartitionManager->get_file_name_size() 
  - sizeof(Finode) - sizeof(BlkNumType)) / sizeof(BlkNumType));
  
  Index currentIndex{0,(_myPartitionManager->get_file_name_size()) + sizeof(Finode)}; 
  char* localBuff = new char[_myPartitionManager->getBlockSize()];
  string tagName;
  BlkNumType tagBlk = 0;
  
  /*Read in the Cont. block*/
  BlkNumType contBlock = 0;
  memcpy(&contBlock, buff + _myPartitionManager->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
  
  /*If there is a cont. block */
  if(contBlock != 0)
  {
    /*Read in everything we can, then all the cont block tags*/
    for(int i = 0; i < localTagCount; i++)
    {
      /*Read in tagBlk number*/
      memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
      
      /*Read in tagBlk to localbuff*/
      _myPartitionManager->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, _myPartitionManager->get_file_name_size());
      tagName = tagName.substr(0, _name.find_first_of('\0'));
      
      /*Insert tagName and tagTree* to _myTree*/
      _myTree.insert(pair<string, TreeObject*>(tagName, rootTree->find(tagName)));
    }
    
    do
    {
      /*Read in tagBlk number*/
      memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
      
      /*Read in tagBlk to localbuff*/
      _myPartitionManager->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, _myPartitionManager->get_file_name_size());
      tagName = tagName.substr(0, _name.find_first_of('\0'));
      
      /*Insert tag and tagblknum to _myTree*/
      _myTree.insert(pair<string, TreeObject*>(tagName, rootTree->find(tagName)));
      
    }while(tagBlk != 0);
    
  }
  else
  {
    /*Read in till we hit a zero entry, or the local tag count*/
    
    int i = 0;
    memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
    currentIndex.offset += sizeof(BlkNumType);
    do
    {
      /*Read in tagBlk to localbuff*/
      _myPartitionManager->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, _myPartitionManager->get_file_name_size());
      tagName = tagName.substr(0, tagName.find_first_of('\0'));
      
      /*Insert tag and tagblknum to _myTree*/
      _myTree.insert(pair<string, TreeObject*>(tagName, rootTree->find(tagName)));
      
      i++;
      /*Read in tagBlk number*/
      memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
    }while(tagBlk != 0 && i < localTagCount);
    
  }
  
  /*Read in the Attributes*/
  if(_myAttributes == 0)
  {
    _myAttributes = new Attributes(_myFinode.attributes, _myPartitionManager);
  }
  _myAttributes->read_in();
  
  
  delete[] buff; 
  delete[] localBuff;
}

void FileInfo::del()
{
  /*Return direct blocks*/
  int i = 0;
  
  while(_myFinode.directBlocks[i] != 0 && i < 12)
  {
    _myPartitionManager->returnDiskBlock(_myFinode.directBlocks[i]);
    i++;
  }
  
  delete_cont_blocks(_myFinode.level1Indirect);
  delete_cont_blocks(_myFinode.level2Indirect);
  delete_cont_blocks(_myFinode.level3Indirect);
  
  /*Return the super block*/
  _myPartitionManager->returnDiskBlock(_blockNumber);
}

void FileInfo::delete_cont_blocks(BlkNumType blknum)
{
  if(blknum == 0)
  {
    return;
  }
  
  char* buff = new char[_myPartitionManager->getBlockSize()];
  /*Read in the block from blknum*/
  _myPartitionManager->readDiskBlock(blknum, buff);
  
  BlkNumType block;
  size_t offset = 0;
  memcpy(&block, buff + offset, sizeof(BlkNumType));
  offset+= sizeof(BlkNumType);
  
  /*read in all block numbers and free them*/
  while(block != 0 && offset <= _myPartitionManager->getBlockSize())
  {
    _myPartitionManager->returnDiskBlock(block);
    memcpy(&block, buff + offset, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
  }
  
  delete[] buff;
  _myPartitionManager->returnDiskBlock(blknum);
}

void FileInfo::add_direct_block(BlkNumType blknum, int index)
{
  if(index < 0 || index > 11)
  {
    throw arboreal_logic_error("index out of bounds", "FileInfo::add_direct_block");
  }
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize());
  
  _myPartitionManager->writeDiskBlock(blknum, buff);
  
  _myFinode.directBlocks[index] = blknum;
  write_out();
  delete[] buff;
}

void FileInfo::add_indirect_block(BlkNumType blknum, short level)
{
  switch(level)
  {
    case 1:
    {
      _myFinode.level1Indirect = blknum;
      break;
    }
    case 2:
    {
      _myFinode.level2Indirect = blknum;
     break; 
    }
    case 3:
    {
      _myFinode.level3Indirect = blknum;
      break;
    }
    default:
    {
      throw arboreal_logic_error("Invalid level " + std::to_string(level), "FileInfo::add_indirect_block");
      
    }
  }
  char* buff = new char[_myPartitionManager->getBlockSize()];
  memset(buff, 0, _myPartitionManager->getBlockSize());
  _myPartitionManager->writeDiskBlock(blknum, buff);
  
  write_out();
  delete[] buff;
}

void FileInfo::insert(string name, TreeObject* ptr)
{
  /*Check for max tags*/
  unsigned int maxTags = ((_myPartitionManager->getBlockSize() - _myPartitionManager->get_file_name_size() - sizeof(finode) 
                          - sizeof(BlkNumType)) / sizeof(BlkNumType)) 
                          + (_myPartitionManager->getBlockSize() / sizeof(BlkNumType));

  if(_myTree.size() >= maxTags)
  {
    throw tag_error(_name + " cannot has reached its maximum number of tags", "FileInfo::insert");
  }
                
  auto ret = _myTree.insert(pair<string, TreeObject*>(name, ptr));
  if(!ret.second)
  {
    throw tag_error (_name + " already tagged with " + name, "FileInfo::insert");
  }
  
  /*Write updated Finode superBlock to disk*/
  this->write_out();
}

void FileInfo::insert_addition(TreeObject* add)
{
  throw arboreal_logic_error("Attempt to call insert_addition on FileInfo object", "FileInfo::insert_addition");
}

void FileInfo::erase(string name)
{
  auto object = _myTree.find(name);
  if(object == _myTree.end())
  {
    throw arboreal_logic_error(name + "Does not exist", "TreeObject::erase");
  }
  _myTree.erase(object);
}

void FileInfo::insert_deletion(TreeObject* del) 
{
  throw arboreal_logic_error("Attempt to call insert_deletion on FileInfo object", "FileInfo::insert_deletion");
}

Attributes* FileInfo::get_attributes(){return _myAttributes;}

Finode FileInfo::get_finode(){return _myFinode;}

unordered_set<string> FileInfo::get_tags()
{
  unordered_set<string> ret;
  for(auto it = _myTree.begin(); it != _myTree.end(); it++)
  {
    ret.insert(it->first);
  }
  return ret;
}

void FileInfo::init_attributes()
{
  BlkNumType attsBlock =  _myPartitionManager->getFreeDiskBlock();
  _myAttributes = new Attributes(attsBlock, _myPartitionManager);
  _myFinode.attributes = attsBlock;
  _myAttributes->set_creation_time();
  _myAttributes->set_owner(DEFAULTOWNER);
  _myAttributes->set_permissions(DEFAULTPERMISSIONS); 
}

size_t FileInfo::get_file_size()
{
  return _myAttributes->get_size();
}

void FileInfo::update_file_size(size_t bytes)
{
  _myAttributes->update_size(bytes);
}

void FileInfo::set_access(){_myAttributes->set_access();}
void FileInfo::set_edit(){_myAttributes->set_edit();}
void FileInfo::set_permissions(char* perms){_myAttributes->set_permissions(perms);}

string FileInfo::mangle()
{
  string ret = _name;
  vector<string> tempTags;

  for(auto it = _myTree.begin(); it != _myTree.end(); it++)
  {
    tempTags.push_back(it->first);
  }
  
  std::sort(tempTags.begin(), tempTags.end());
  
  for(string tag : tempTags)
  {
    ret.append("_");
    ret.append(tag);
  }
  
  return ret;
}

string FileInfo::mangle(vector<string>& tags)
{
  string ret = _name;
  
  std::sort(tags.begin(), tags.end());
  
  for(string tag : tags)
  {
    ret.append("_");
    ret.append(tag);
  }
  
  return ret;
  
}

string FileInfo::mangle(unordered_set<string>& tags)
{
  vector<string> tagVec;
  for(string tag : tags)
  {
    tagVec.push_back(tag);
  }
  return mangle(tagVec);
}

char* FileInfo::serialize(FileInfo* file, size_t& size)
{
  /*Format:
   * size of name
   * name
   * number of tags
   * tags \0 terminated
   * attributes
   */
  
  /*write out name, tags, attributes*/
  
  string filename = file->get_name();
  
  size_t localSize = 0;
  localSize+= sizeof(size_t) * 2;
  localSize+= filename.size();
  
  for(auto tagIt = file->begin(); tagIt != file->end(); ++tagIt)
  {
    localSize += tagIt->first.size() + 1;
  }
  
  localSize+= sizeof(FileAttributes);
  
  size_t offset = 0;
  char* ret = new char[localSize];
  memset(ret, 0, localSize);
  
  size_t nameSize = filename.size();
  memcpy(ret, &nameSize, sizeof(size_t));
  offset+= sizeof(size_t);
  
  memcpy(ret + offset, filename.c_str(), nameSize);
  offset+= nameSize;
  
  size_t numTags = filename.size();
  memcpy(ret + offset, &numTags, sizeof(size_t));
  offset+= sizeof(size_t);
  
  for(auto tagIt = file->begin(); tagIt != file->end(); ++tagIt)
  {
    string tagName = tagIt->first;
    memcpy(ret + offset, tagName.c_str(), tagName.size());
    offset+= tagName.size() + 1;
  }
  
  FileAttributes attributes = file->get_attributes()->get_file_attributes();
  memcpy(ret + offset, &attributes, sizeof(FileAttributes));
  offset+= sizeof(FileAttributes);
  
  size = offset;
  return ret;
}


/******************************************************************************/

