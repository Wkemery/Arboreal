/*Trees.cpp
 * Arboreal
 * November 2017
 */


#include "Trees.h"
#include<string>
#include<string.h>
#include<unordered_map>
#include<iostream>
using namespace std;

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

/******************************************************************************/

void Attributes::writeOut(PartitionManager* pm)
{
  
}

void Attributes::readIn(PartitionManager* pm)
{
  
}

/******************************************************************************/

Addition::Addition(TreeObject* obj, TreeObject* parent):Modification(obj, parent)
{}

void Addition::writeOut(PartitionManager* pm)
{  
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  Index nextEntry{0,0};
  
  
  pm->readDiskBlock(_parent->getLastEntry().blknum, buff);
  
  if(_parent->getLastEntry().blknum == 0)
  {
    nextEntry.blknum = _parent->getStartBlock();
    nextEntry.offset = 0;
  }
  else
  {
    nextEntry = _parent->getLastEntry();
    _parent->incrementAllocate(&nextEntry, pm);
  }
  
  if(_parent->getLastEntry().blknum != nextEntry.blknum)
  {
    pm->readDiskBlock(nextEntry.blknum, buff);
  }
  
  int keySize = _mod->getName().length();
  strncpy(buff + nextEntry.offset, _mod->getName().c_str(), keySize);
  memset(buff + nextEntry.offset + keySize, 0, pm->getFileNameSize() - keySize);
  
  /*write out the blocknumber of the tagTree to buffer, probably 8 bytes?*/
  BlkNumType tagBlk = _mod->getBlockNumber();
  memcpy(buff + nextEntry.offset + pm->getFileNameSize(), &tagBlk, sizeof(BlkNumType));
  
  _parent->setLastEntry(nextEntry);
  
  _mod->setIndex(_parent->getLastEntry());
  
  pm->writeDiskBlock(_parent->getLastEntry().blknum, buff);
}
/******************************************************************************/

Deletion::Deletion(TreeObject* obj, TreeObject* parent):Modification(obj, parent)
{}

void Deletion::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  if(_mod->getIndex().blknum == 0)
  {
    cerr << "Tag Tree was never written out. Not really an error, just here for debugging" << endl;
    //NOTE: even though it is just here for debugging don't remove the if, just the printout.
  }
  else
  {        
    pm->readDiskBlock(_mod->getIndex().blknum, buff);
      
    /*Zero out name and blocknumber*/
    memset(buff + _mod->getIndex().offset, 0, pm->getFileNameSize() + sizeof(BlkNumType));
    
    /*Remove that TagTrees presence on Disk*/
    _mod->del(pm);
    
    /*Write out buff to mod blknum*/
    pm->writeDiskBlock(_mod->getIndex().blknum, buff);
  }
}

/******************************************************************************/
TreeObject::TreeObject(string name, BlkNumType blknum)
:_name(name), _blockNumber(blknum)
{}

TreeObject::~TreeObject(){}

string TreeObject::getName(){return _name;}

bool TreeObject::isRead(TreeObject* obj)
{
  auto it = _readable.find(obj);
  if(it == _readable.end())
  {
    return false;
  }
  return it->second;
}

void TreeObject::setRead(TreeObject* obj)
{
  auto it = _readable.find(obj);
  if(it != _readable.end())
  {
    it->second = true;
  }
}

void TreeObject::setIndex(Index index){_index.blknum = index.blknum; _index.offset = index.offset;}

Index TreeObject::getIndex(){return _index;}

BlkNumType TreeObject::getBlockNumber(){return _blockNumber;}

Index TreeObject::getLastEntry(){return _lastEntry;}

BlkNumType TreeObject::getStartBlock(){return _startBlock;}

void TreeObject::setLastEntry(Index index){_lastEntry = index;}


void TreeObject::incrementAllocate(Index* index, PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  
  if((pm->getBlockSize() - index->offset - entrySize) < (entrySize + sizeof(BlkNumType)))
  {
    BlkNumType newBlock;
    newBlock = pm->getFreeDiskBlock();
    
    /*Read in old block*/
    pm->readDiskBlock(index->blknum, buff);
    
    /*set continuation on old block*/
    memcpy(buff + pm->getBlockSize() - sizeof(BlkNumType), &newBlock, sizeof(BlkNumType));
    
    /*Write out old block*/
    pm->writeDiskBlock(index->blknum, buff);
    
    index->blknum = newBlock;
    index->offset = 0;
  }
  else
  {
    index->offset += entrySize;
  }
}

void TreeObject::incrementFollow(Index* index, PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  int entrySize = pm->getFileNameSize() + (sizeof(BlkNumType));
  
  if((pm->getBlockSize() - index->offset) < entrySize + sizeof(BlkNumType))
  {
    /*Read in index.blknum*/
    pm->readDiskBlock(index->blknum, buff);
    
    /*Read in cont. blocknum */
    BlkNumType contBlkNum;
    memcpy(&contBlkNum, buff + pm->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
    
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
}

void TreeObject::deleteContBlocks(PartitionManager* pm, BlkNumType blknum)
{
  //TODO: finish function
  
  pm->returnDiskBlock(blknum);
}

/******************************************************************************/
RootTree::RootTree():TreeObject("Root", 1)
{
  _lastEntry.blknum = 1;
  _lastEntry.offset = 0;
  _startBlock = 0;
}

RootTree::~RootTree(){}

unordered_map<string, TagTree*>* RootTree::getMap(){return &_tree;}

void RootTree::insertAddition(TagTree* tag)
{
//   _additions.push(tag);
  _modifications.push(new Addition(tag, this));
}

void RootTree::insertDeletion(TagTree* tag)
{
//   _deletions.insert(pair<BlkNumType, TagTree*>(tag->getIndex()->blknum, tag));
  _modifications.push(new Deletion(tag, this));
  
}

void RootTree::writeOut(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory

  /******************************************************************************/

  /*Write out modifications*/
  size_t queueSize = _modifications.size();
  
  for(size_t i = 0; i < queueSize; i++)
  {
    _modifications.front()->writeOut(pm);
    _modifications.pop();
  }
  
  /******************************************************************************/
  
  /*Write out Root super block*/
  
  Index currentIndex{_blockNumber, 0};
  
  /*_blockNumber is the super block for the Root Tree */
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.size());
  currentIndex.offset+=  _name.size() + 1;
  
  size_t treeSize = _tree.size();
  
  RootSuperBlock rootInfo{treeSize, _lastEntry, _startBlock};
  
  memcpy(buff + currentIndex.offset, &rootInfo, sizeof(RootSuperBlock));
  currentIndex.offset+=  sizeof(RootSuperBlock);
  
  /*Write out RootTree superblock*/
  pm->writeDiskBlock(currentIndex.blknum, buff);
}

void RootTree::readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles)
{
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  
  /*Read in RootTree superblock*/
  pm->readDiskBlock(currentIndex.blknum, buff);
  
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
    //TODO: throw error. Root can never be empty becuase of default tag tree.
    cerr << "Error! RootTree::readIn, _startblock = 0. root tree empty" << endl;
  }
  
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  
  pm->readDiskBlock(currentIndex.blknum, buff);

  Index EOFIndex{0,0};
  
  while(currentIndex != EOFIndex)
  {
    if(buff[currentIndex.offset] != 0)
    {
      string tagName;
      BlkNumType blknum;
      
      /*Read in tagName*/
      tagName.assign(buff + currentIndex.offset, pm->getFileNameSize());
      tagName = tagName.substr(0, tagName.find_first_of('\0'));
      
      /*Read in blocknumber for that TagTree object*/
      memcpy(&blknum, buff + currentIndex.offset + pm->getFileNameSize() , sizeof(BlkNumType));
      if(blknum == 0)
      {
        //TODO: throw error
        cerr << "Error RootTree::readIn3" << endl;
      }
      
      /*Create TagTree object*/
      TagTree* tagTree = new TagTree(tagName, blknum);
      tagTree->setIndex(currentIndex);
      if(tagTree == 0)
      {
        //TODO: throw error
        cerr << "Error RootTree::readIn4" << endl;
      }
      
      /*Insert key and value into tagtree in memory*/
      auto it_ret = _tree.insert(pair<string, TagTree*>(tagName, tagTree));
      if(!it_ret.second)
      {
        throw arboreal_logic_error("Duplicate Tag Tree read in from disk", "RootTree::readIn");
      }
//       /*Insert key into _readable */
//       auto it_ret2 = _readable.insert(pair<TreeObject*, bool>(tagTree, false));
//       if(!it_ret2.second)
//       {
//         //TODO: throw error
//         cerr << "Error RootTree::readIn6" << endl;
//       }
    }
    
    incrementFollow(&currentIndex, pm);
  }
}

void RootTree::del(PartitionManager* pm)
{
  //TODO: stub
  
}

/******************************************************************************/

TagTree::TagTree(string tagName, BlkNumType blknum):TreeObject(tagName, blknum)
{
//   _lastEntry.blknum = blknum;
//   _lastEntry.offset = 0;
  _lastEntry.blknum = 0;
  _startBlock = 0;
}

TagTree::~TagTree(){}

unordered_map<string, FileInfo*>* TagTree::getMap() {return &_tree;}

void TagTree::insertAddition(FileInfo* file)
{
//   _additions.push(file);
  _modifications.push(new Addition(file, this));
  
}

void TagTree::insertDeletion(FileInfo* file)
{
//   _deletions.insert(pair<BlkNumType, FileInfo*>(file->getIndex()->blknum, file));
  _modifications.push(new Deletion(file, this));
  
}

void TagTree::writeOut(PartitionManager* pm)
{
  //TODO: incorporate file number of tags
  
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  if(_startBlock == 0)
  {
    /*Need to allocate a new block to start appending*/
    BlkNumType newblknum = 0;
    newblknum = pm->getFreeDiskBlock();
    _lastEntry.blknum = 0;
    _lastEntry.offset = 0;
    _startBlock = newblknum;
  }
  /****************************************************************************/
  
  /*Write out modifications*/
  size_t queueSize = _modifications.size();
  
  for(size_t i = 0; i < queueSize; i++)
  {
    _modifications.front()->writeOut(pm);

    _modifications.pop();
  }
  
  /****************************************************************************/
  
  Index currentIndex{_blockNumber, 0};
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  /*_blockNumber is the super block for this tagTree*/
  /*Layout - 
   * TagName - fileNameSize - 64 for now
   * Size - size_t - 4 ?
   * Index of end of tag tree on disk - sizeof(Index)
   * startBlocknumber - start of entries on disk - sizeof(BlkNumType)
   */
  
  memcpy(buff + currentIndex.offset, _name.c_str(), _name.size());
  currentIndex.offset+= pm->getFileNameSize();
  
  TagTreeSuperBlock tagInfo{_tree.size(), _lastEntry, _startBlock};
  memcpy(buff + currentIndex.offset, &tagInfo, sizeof(TagTreeSuperBlock));
  currentIndex.offset+=  sizeof(TagTreeSuperBlock);
  
  /*Write out TagTree superblock*/
  pm->writeDiskBlock(currentIndex.blknum, buff);
  

}

void TagTree::readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles)
{
  //TODO: incorporate file number of tags
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  Index currentIndex{_blockNumber, 0};
  
  /*Read in tagTree superblock*/
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::readIn1" << endl;}
  
  /*Store values from superblock*/
  _name.assign(buff + currentIndex.offset, pm->getFileNameSize());
  _name = _name.substr(0, _name.find_first_of('\0'));
  currentIndex.offset+=  pm->getFileNameSize();
  
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
    return;
  }
  
  currentIndex.blknum = _startBlock;
  currentIndex.offset = 0;
  
  //TODO: fix catch statement.
  try{pm->readDiskBlock(currentIndex.blknum, buff);}
  catch(...){cerr << "Error TagTree::readIn2" << endl;}
  
  Index EOFIndex{0,0};
  
  while(currentIndex != EOFIndex)
  {
    if(buff[currentIndex.offset] != 0)
    {
      string fileName;
      BlkNumType blknum;
      
      /*Read in fileName*/
      fileName.assign(buff + currentIndex.offset, pm->getFileNameSize());
      fileName = fileName.substr(0, fileName.find_first_of('\0'));
      
      /*Read in blocknumber for that Finode object*/
      memcpy(&blknum, buff + currentIndex.offset + pm->getFileNameSize() , sizeof(BlkNumType));
      if(blknum == 0)
      {
        //TODO: throw error
        cerr << "Error TagTree::readIn3" << endl;
      }
      
      /*Create FileInfo object*/
      FileInfo* finode = new FileInfo(fileName, blknum);
      finode->setIndex(currentIndex);
      if(finode == 0)
      {
        //TODO: throw error
        cerr << "Error TagTree::readIn4" << endl;
      }
      
      /*Read in the finode*/
      finode->readIn(pm, allFiles);
      
      /*Insert key and value into FileInfo object in memory*/
      auto it_ret = _tree.insert(pair<string, FileInfo*>(finode->mangle(), finode));
      if(!it_ret.second)
      {
        throw arboreal_logic_error("Duplicate File read in from Disk", "TagTree:readIn");
      }
      
      /*add to allFiles*/
      allFiles->insert(pair<string, FileInfo*>(fileName, finode));
      
//       /*add key to _readable*/
//       auto it_ret2 = _readable.insert(pair<TreeObject*, bool>(finode, false));
//       if(!it_ret2.second)
//       {
//         //TODO: throw error
//         cerr << "Error TagTree::readIn6" << endl;
//       }
    }
    
    incrementFollow(&currentIndex, pm);
  }
}

void TagTree::del(PartitionManager* pm)
{
  char* buff = new char[pm->getBlockSize()];
  
  /*Read in the super block of this tagTree*/
  pm->readDiskBlock(_blockNumber, buff);
  
  TagTreeSuperBlock tagInfo;
  memcpy(&tagInfo, buff + pm->getFileNameSize(), sizeof(TagTreeSuperBlock));
  
  /* This value could be in memory. but it may differ, and we only care about 
   * what is written to disk. plus if we are deleting this tag tree anyway the 
   * values in memory will be handled elsewhere*/
  BlkNumType myStartBlock = tagInfo.startBlock; 
  
  if(myStartBlock != 0)
  {
    deleteContBlocks(pm, myStartBlock);
  }
  
  pm->returnDiskBlock(_blockNumber);

  
}

/******************************************************************************/
FileInfo::FileInfo(string fileName,BlkNumType blknum):TreeObject(fileName, blknum)
{
  memset(&_myFinode, 0, sizeof(Finode));
}

FileInfo::~FileInfo(){}

map<string, BlkNumType>* FileInfo::getMap()
{
  return &_tags;
}

void FileInfo::writeOut(PartitionManager* pm)
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
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  memcpy(buff, _name.c_str(), _name.size());
  
  memcpy(buff + (pm->getFileNameSize()), &_myFinode, sizeof(Finode));
  
  /*This is the maximum number of tags we can store before needing a cont block*/
  int localTagCount = ((pm->getFileNameSize()) - sizeof(Finode) - sizeof(BlkNumType))
                        / sizeof(BlkNumType);
  
  
  /*Read in current Finode*/
  char* localBuff = new char[pm->getBlockSize()];
  memset(localBuff, 0, pm->getBlockSize()); //zero out memory
  
  pm->readDiskBlock(_blockNumber, localBuff);
  
  BlkNumType contBlock = 0;
  memcpy(&contBlock, localBuff + pm->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
  
  int offset = pm->getFileNameSize() + sizeof(Finode);
  
  if(_tags.size() <= localTagCount)
  {
    /*If there is a cont. block, free it*/
    if(contBlock != 0)
    {
      pm->returnDiskBlock(contBlock);
    }
    
    /*There is room to store all the tags locally*/
    for(auto it = _tags.begin(); it != _tags.end(); it++)
    {
      BlkNumType blknum = it->second;
      memcpy(buff + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
    }
  }
  else
  {
    /*There is not room to store all the tags locally*/
    auto it = _tags.begin();
    
    for(int i = 0; i < localTagCount; i++)
    {
      /*Write out as many as we can*/
      BlkNumType blknum = it->second;
      memcpy(buff + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
      it++;
    }
    
    /*If there is already a cont. block, just overwrite it*/
    /*If not, allocate one and write to it.*/
    if(contBlock == 0)
    {
      contBlock = pm->getFreeDiskBlock();
      /*Write out the cont blocknum to the finode*/
      memcpy(buff + pm->getBlockSize() - sizeof(BlkNumType), &contBlock, sizeof(BlkNumType));
    }
    
    offset = 0;
    memset(buff, 0, pm->getBlockSize());
    for(it = it; it != _tags.end(); it++)
    {
      /*Write out the rest of the tags into the cont block*/
      BlkNumType blknum = it->second;
      memcpy(buff + offset, &blknum, sizeof(BlkNumType));
      offset+= sizeof(BlkNumType);
      it++;
    }
    
    /*Write out the contBlock of tags*/
    pm->writeDiskBlock(contBlock, buff);
  }
  delete localBuff;
  /*Write out Finode*/
  pm->writeDiskBlock(_blockNumber, buff);
    
  /*Write out attributes*/
  _myAttributes.writeOut(pm);
  
}

void FileInfo::readIn(PartitionManager* pm, unordered_multimap<string, FileInfo*>* allFiles)
{
  /*Read in all the finode data*/
  char* buff = new char[pm->getBlockSize()];
  memset(buff, 0, pm->getBlockSize()); //zero out memory
  
  /*Read in Finode*/
  pm->readDiskBlock(_blockNumber, buff);
    
  memcpy(&_myFinode, buff + (pm->getFileNameSize()), sizeof(Finode));
  
  /*This is the maximum number of tags we can store before needing a cont block*/
  int localTagCount = ((pm->getFileNameSize()) - sizeof(Finode) - sizeof(BlkNumType))
  / sizeof(BlkNumType);
  
  Index currentIndex{0,(pm->getFileNameSize()) + sizeof(Finode)}; 
  char* localBuff = new char[pm->getBlockSize()];
  string tagName;
  BlkNumType tagBlk = 0;
  
  /*Read in the Cont. block*/
  BlkNumType contBlock = 0;
  memcpy(&contBlock, buff + pm->getBlockSize() - sizeof(BlkNumType), sizeof(BlkNumType));
  
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
      pm->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, pm->getFileNameSize());
      tagName = tagName.substr(0, _name.find_first_of('\0'));
      
      /*Insert tag and tagblknum to _tags*/
      _tags.insert(pair<string, BlkNumType>(tagName, tagBlk));
    }
    
    do
    {
      /*Read in tagBlk number*/
      memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
      
      /*Read in tagBlk to localbuff*/
      pm->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, pm->getFileNameSize());
      tagName = tagName.substr(0, _name.find_first_of('\0'));
      
      /*Insert tag and tagblknum to _tags*/
      _tags.insert(pair<string, BlkNumType>(tagName, tagBlk));
      
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
      pm->readDiskBlock(tagBlk, localBuff);
      
      /*Save tag name*/
      tagName.assign(localBuff, pm->getFileNameSize());
      tagName = tagName.substr(0, tagName.find_first_of('\0'));
      
      /*Insert tag and tagblknum to _tags*/
      _tags.insert(pair<string, BlkNumType>(tagName, tagBlk));
      
      i++;
      /*Read in tagBlk number*/
      memcpy(&tagBlk, buff + currentIndex.offset, sizeof(BlkNumType));
      currentIndex.offset += sizeof(BlkNumType);
    }while(tagBlk != 0 && i < localTagCount);
    
  }
  
  /*Read in the Attributes*/
  _myAttributes.readIn(pm);
}

void FileInfo::del(PartitionManager* pm)
{}

string FileInfo::mangle()
{
  string ret = _name;
  for(auto it = _tags.begin(); it != _tags.end(); it++)
  {
    ret.append("_");
    ret.append(it->first);
  }
  return ret;
}

string FileInfo::mangle(vector<string>& tags)
{
  map<string, int> tempTags;
  string ret = _name;
  
  for(size_t i = 0; i < tags.size(); i++)
  {
    tempTags.insert(pair<string, int>(tags[i], 0));
  }
  
  for(auto it = _tags.begin(); it != _tags.end(); it++)
  {
    tempTags.insert(pair<string, int>(it->first, 0));
  }
  
  for(auto it = tempTags.begin(); it != tempTags.end(); it++)
  {
    ret.append("_");
    ret.append(it->first);
  }
  
  return ret.substr(0, ret.find_first_of('\0'));
  
}

/******************************************************************************/


