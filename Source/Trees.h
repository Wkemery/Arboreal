/*Trees.h
 * Arboreal
 * November 2017
 */

#include "types.h"
#include<unordered_map>
#include<string>
#include <unordered_set>
#include"partitionmanager.h"
#include<queue>
#include<map>
#include<algorithm>
using namespace std;

#ifndef TREES_H
#define TREES_H

//! A BREIF DESCRIPTION
bool operator ==(Index& lhs, Index& rhs);
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
bool operator !=(Index& lhs, Index& rhs);

class Attributes
{
private:
  FileAttributes _atts;
  BlkNumType _blockNumber;
  PartitionManager* _myPartitionManager;
public:
  Attributes(BlkNumType blknum, PartitionManager* pm);
  void writeOut();
  void readIn();
  void del();
  void setCreationTime();
  void setOwner(int owner);
  void setPermissions(char* perms);
  void setAccess();
  void setEdit();
  void updateSize(size_t size);
  
  /*Accessor Functions*/
  time_t getCreationTime();
  int getOwner();
  char* getPermissions();
  time_t getAccess();
  time_t getEdit();
  size_t getSize();
};

class Modification
{
protected:
  TreeObject* _mod;
  TreeObject* _parent;
  Modification(TreeObject* obj, TreeObject* parent);
public:
  virtual void writeOut(PartitionManager* pm) = 0;
};

class Addition : public Modification
{
public:
  Addition(TreeObject* obj, TreeObject* parent);
  void writeOut (PartitionManager* pm);
};

class Deletion : public Modification
{
public:
  Deletion(TreeObject* obj, TreeObject* parent);
  void writeOut (PartitionManager* pm);
};

class TreeObject
{
protected:
  queue<Modification*> _modifications; //!< A collection of associated Modifications
  unordered_map<string, TreeObject*> _myTree;//!< A collection of contained TreeObjects 
  string _name;//!< name or value
  BlkNumType _blockNumber; //!< Blocknumber of the superblock on disk
  unordered_map<TreeObject*, Index> _indeces;//!< location(s) of the superblock entry(ies) on disk
  Index _lastEntry;  //!< Index of the last entry of data on disk
  BlkNumType _startBlock; //!< blocknumber of the start of this data on disk
  PartitionManager* _myPartitionManager;//!< Associated PartitionManager

public:
/*Constructors*/
  virtual ~TreeObject();
  
  /*!
   * \param  name name of this object
   * \param  blknum  blocknumber of the superblock
   * \param  pm    PartitionManager object to be associated with this object
   */
  TreeObject(string name, BlkNumType blknum, PartitionManager* pm);
  
  /** @name Accessor Functions 
   */
  ///@{ 
  
/*Accessor Functions*/
  /*!
   *@return The name 
   */
  string get_name() const;
  
  /*!
   * \return The blocknumber of the superblock
   */
  BlkNumType get_block_number() const;
  
  /*!
   * Searches for obj and returns the Index of obj on disk, if found
   * \param  obj object whose position is desired
   * \return The Index of obj on disk, 
   * \throw arboreal_logic_error 
   */
  Index get_index(TreeObject* obj) const;
  
  /*!
   * Find the Index of the last entry for this object on disk
   * \return Index of the last entry on disk
   */
  Index get_last_entry() const;
  
  /*!
   * @return The start block of data for this object
   */
  BlkNumType get_start_block() const;
  
  /*!
   * @return The size of _myTree
   */
  size_t size() const;
  
  /*!
   * @return An iterator to the beginning of the TreeObjects associated with this object
   */
  unordered_map<string, TreeObject*>::iterator begin();
  
  /*!
   * @return An iterator to the end of the TreeObjects associated with this object
   */
  unordered_map<string, TreeObject*>::iterator end();
  
  /*!
   * Search _myTree for the specified name
   * @param name the name of the desired object
   * @return a pointer to the object if found, 0 otherwise
   */
  TreeObject* find(string name) const;

  ///@}
/**********************************************************************************************************************/

/** @name Modifier Functions 
 */
///@{ 

  /*!
  * Set the name
  * @param name The new name
  */
  void set_name(string name);
  
  /*!
   * Add an index to _indeces for the specified TreeObject. If the index already existed. nothing happpens
   * @param obj the object that the Index references to
   * @param index the Index of obj
   */
  void add_index(TreeObject* obj, Index index);
  
  /*!
   * Set the last Index for the last entry belonging to this object on disk
   * @param index The last Index
   */
  void set_last_entry(Index index);
  
  /*!
   * Associate a TreeObject with this object
   * @param name name of the object, mangled if inserting a FileInfo
   * @param obj the object to be inserted
   * @throw tag_error
   * @sa FileInfo::insert()
   */
  virtual void insert(string name, TreeObject* obj);
  
  /*!
   * Disassociate the given name from this object
   * @param name the name of the object to be erased.
   * @throw arboreal_logic_error
   * @sa FileInfo::erase()
   */
  virtual void erase(string name);
  
  /*!
   * Add an Addition to the list of Modifications so that it can be written out later. Note: Do not call this on a 
   * FileInfo.
   * @param add the object that was previously inserted to this object which will be added to the list of Modifications
   * @sa FileSystem::writeOut() TreeObject::insert()
   */
  virtual void insert_addition(TreeObject* add);
  
  /*!
   * Add a Deletion to the list of Modifications so that it can be written out later. Note: Do not call this on a 
   * FileInfo.
   * @param del the object that was previously erased from this object which will be added to the list of Modifications
   * @sa FileSystem::writeOut() TreeObject::erase()
   */
  virtual void insert_deletion(TreeObject* del);

  ///@}
  
/**********************************************************************************************************************/
  

/** @name Disk Functions 
 */
///@{ 

/*!
 * Intended to write out the object to disk
 */
  virtual void writeOut() = 0;
  virtual void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree) = 0;
  virtual void del() = 0;
    /*This will completely remove the TreeObject's presence on disk.*/
    
  void incrementAllocate(Index* index);
  void incrementFollow(Index* index);
  
  ///@}
  
/*Helper Functions*/
protected:
  virtual void deleteContBlocks(BlkNumType blknum);
  /* deleteContBlocks will take a blknum and free it. it will follow the chain 
   * of continuation blocks and free all of them too*/
};

class FileInfo : public TreeObject
{
private:
  Attributes* _myAttributes;
  Finode _myFinode;
public:
  FileInfo(string filename, BlkNumType blknum, PartitionManager* pm);
  ~FileInfo();
  string mangle(); 
  string mangle(vector<string>& tags);
  string mangle(unordered_set<string>& tags);
  Finode getFinode();
  void addDirectBlock(BlkNumType blknum, int index);
  void addIndirectBlock(BlkNumType blknum, short level);
  size_t getFileSize();
  void updateFileSize(size_t bytes);
  void setAccess();
  void setEdit();
  void setPermissions(char* perms);
  Attributes* getAttributes();
  unordered_set<string> getTags();
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void erase(string name);
  void insert(string name, TreeObject* ptr);
  void del();
  void deleteContBlocks(BlkNumType blknum);
  void insert_addition(TreeObject* add);
  void insert_deletion(TreeObject* del);

  
};

class TagTree : public TreeObject
{
public:
  TagTree(string tagName, BlkNumType blknum, PartitionManager* pm);
  ~TagTree();
  
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
};

class RootTree : public TreeObject
{
public:
  RootTree(PartitionManager* pm);
  ~RootTree();
  
  /*Function Overrides*/
  void writeOut();
  void readIn(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
  
};

#endif