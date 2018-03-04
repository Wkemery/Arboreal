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

class Attributes
{
private:
  FileAttributes _atts;
  BlkNumType _blockNumber;
  PartitionManager* _myPartitionManager;
public:
  Attributes(BlkNumType blknum, PartitionManager* pm);
  
/**********************************************************************************************************************/  
  /** @name Modifier Functions
   */
  ///@{ 
  
  /*!
   * Writes out the Attributes to disk
   */
  void write_out();
  
  /*!
   * Reads in the Attributes from disk
   */
  void read_in();
  
  /*!
   * Removes the Attributes presence on disk
   */
  void del();
  
  /*!
   * Marks down the creation time of the associated FileInfo as UNIX timestamp
   */
  void set_creation_time();
  
  /*!
   * Marks the owner as their UID
   */
  void set_owner(int owner);
  
  /*!
   * sets the permisssions of the file
   * @sa FileInfo::set_permissions(char*)
   */
  void set_permissions(char* perms);
  
  /*!
   * Marks down the time as accessed time as UNIX timestamp
   */
  void set_access();
  
  /*!
   * Marks down the time as modified time as UNIX timestamp
   */
  void set_edit();
  
  /*!
   * sets the size to the specified size
   */
  void update_size(size_t size);
  ///@}
  
/**********************************************************************************************************************/  
  /** @name Accessor Functions 
   */
  ///@{ 
  
  /*!
   * @returns the UNIX time the file was created
   */
  time_t get_creation_time();
  
  /*!
   * @returns the UID of the owner of the file
   */
  int get_owner();
  
  /*!
   * @returns the permisssions
   * @sa FileInfo::get_permissions(char*)
   */
  char* get_permissions();
  
  /*!
   * @returns the UNIX time the file was last accessed
   */
  time_t get_access();
  
  /*!
   * @returns the UNIX time the file was last edited
   */
  time_t get_edit();
  
  /*!
   * @returns the size of the file in bytes
   */
  size_t get_size();
  
  ///@}
  
};

class Modification
{
protected:
  TreeObject* _mod;
  TreeObject* _parent;
  Modification(TreeObject* obj, TreeObject* parent);
  
public:
  virtual ~Modification();
  virtual void write_out(PartitionManager* pm) = 0;
};

class Addition : public Modification
{
public:
  Addition(TreeObject* obj, TreeObject* parent);
  ~Addition();
  void write_out (PartitionManager* pm);
};

class Deletion : public Modification
{
public:
  Deletion(TreeObject* obj, TreeObject* parent);
  ~Deletion();
  void write_out (PartitionManager* pm);
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
   */
  virtual void erase(string name);
  
  /*!
   * Add an Addition to the list of Modifications so that it can be written out later. Note: Do not call this on a 
   * FileInfo.
   * @param add the object that was previously inserted to this object which will be added to the list of Modifications
   * @sa FileSystem::write_out() TreeObject::insert()
   */
  virtual void insert_addition(TreeObject* add);
  
  /*!
   * Add a Deletion to the list of Modifications so that it can be written out later. Note: Do not call this on a 
   * FileInfo.
   * @param del the object that was previously erased from this object which will be added to the list of Modifications
   * @sa FileSystem::write_out() TreeObject::erase()
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
  virtual void write_out() = 0;
  
  /*!
   * Will read in all object data from disk
   * @param allFiles a pointer to the map of all files 
   * @param rootTree a pointer to the root tree
   */
  virtual void read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree) = 0;
  
  /*!
   * Will completely remove the TreeObject's presence on disk
   */
  virtual void del() = 0;

  /*!
   * Will increment the Index passed and allocate blocks if necessary to do so
   * @param index the Index to be incremented
   */
  void increment_allocate(Index* index);
  
  /*!
   * Will increment the Index passed but only follow the chain of already allocated blocks
   * @param index the Index to be incremented
   */
  void increment_follow(Index* index);

  
  ///@}
  
/*Helper Functions*/
protected:
  
  /*!
   * Will follow the chain of continuation blocks and free all of them
   * @param blknum will free the blknum and use it to follow the chain of continuation blocks
   */
  virtual void delete_cont_blocks(BlkNumType blknum);
};

class FileInfo : public TreeObject
{
private:
  Attributes* _myAttributes;
  Finode _myFinode;
public:
  /*!
   * @param filename Name of the File
   * @param blknum the blocknumber of the associated Finode on disk
   */
  FileInfo(string filename, BlkNumType blknum, PartitionManager* pm);
  ~FileInfo();
  
/**********************************************************************************************************************/
  
  /** @name Accessor Functions 
    */
  ///@{

  /*!
   *@brief mangles the filename with its tags 
   * 
   * The name is mangled as follows:
   * Each tag is placed in alphabetical order and appended to the filename
   * using '_' as the seperator.
   * 
   *@returns the mangled name of this file.
   *@sa mangle(vector<string>&) mangle(unordered_set<string>& tags)
   */  
  string mangle(); 
  
  /*!
   * @brief mangles the filename with the specified tags 
   * 
   * The name is mangled as follows:
   * Each tag is placed in alphabetical order and appended to the filename
   * using '_' as the seperator.
   * @returns the mangled name of this file.
   * @param tags the tags you wish to mangle the filename with
   * @sa mangle() mangle(unordered_set<string>& tags)
   */ 
  string mangle(vector<string>& tags);
  
  /*!)
   * @brief mangles the filename with the specified tags 
   * 
   * The name is mangled as follows:
   * Each tag is placed in alphabetical order and appended to the filename
   * using '_' as the seperator.
   * 
   * @returns the mangled name of this file.
   * @param tags the tags you wish to mangle the filename with
   * @sa mangle() mangle(unordered_set<string>& tags
   */
  string mangle(unordered_set<string>& tags);
  
  /*!
   * @returns the Finode associated with this file
   */
  Finode get_finode();
  
  /*!
   * @returns the size of this file in bytes
   */
  size_t get_file_size();
  
  /*!
   * @returns the Attributes accociated with this file
   */
  Attributes* get_attributes();
  
  /*!
   * @returns The tags associated with this file
   */
  unordered_set<string> get_tags();
  
  ///@}
  
/**********************************************************************************************************************/
  /** @name Modifier Functions 
  */
  ///@{  
 
 /*!
  * adds the specified blocknumber to the array of direct blocks in this file's Finode
  * @param blknum the block number of the direct block that has already been allocated
  * @param index the index of the blknum in the array, must be less than 12 and at least 0.
  * @throws arboreal_logic_error index out of bounds
  * @sa add_indirect_block
  */
  void add_direct_block(BlkNumType blknum, int index);
  
  /*!
   * adds the specified blocknumber to the Finode as the start of the specified level of indirect blocks
   * @param blknum the block number of the indirect block that has already been allocated
   * @param level the level that the block number is associated with. must be 1, 2 or 3. 
   * @throws arboreal_logic_error Invalid level
   * @sa add_direct_block
   */
  void add_indirect_block(BlkNumType blknum, short level);
  
  /*!
   * Sets the file size to the specified bytes. Only the filesystem should call.
   * @param bytes the new file size
   */
  void update_file_size(size_t bytes);
  
  /*!
   * marks the file as accessed at the current UNIX time
   */
  void set_access();
  
  /*!
   * marks the file as edited at the current UNIX time
   */
  void set_edit();
  
  /*!
   * @brief sets the permisssions for this file
   * 
   * The permisssions format is as follows. a 1 for true 0 false
   * Byte 0, 1, 2 : reserved, for now
   * Byte 3 - 5 : read write and execute permisssions for the user
   * Byte 6 - 8 : read write and execute permisssions for the group
   * Byte 9 - 11 : read write and execute permisssions for the world
   * Currently there is no differentiation between user group and world
   * 
   * @param perms the permisssions in the correct format
   */
  void set_permissions(char* perms);
  
  ///@}
  
/**********************************************************************************************************************/
  
  /*Function Overrides*/
  void write_out();
  void read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void erase(string name);
  void insert(string name, TreeObject* ptr);
  void del();
  void delete_cont_blocks(BlkNumType blknum);
  /*!
   * Do not call on FileInfo
   */
  void insert_addition(TreeObject* add);
  /*!
   * Do not call on FileInfo
   */
  void insert_deletion(TreeObject* del);

private:
  void init_attributes();
  
};

class TagTree : public TreeObject
{
public:
  /*!
   * @param tagName the name of this tag
   * @param blknum the blocknumber for the superblock of this tagTree
   */
  TagTree(string tagName, BlkNumType blknum, PartitionManager* pm);
  
  ~TagTree();
  
  /*Function Overrides*/
  void write_out();
  void read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
};

class RootTree : public TreeObject
{
public:
  RootTree(PartitionManager* pm);
  
  ~RootTree();
  
  /*Function Overrides*/
  void write_out();
  void read_in(unordered_multimap<string, FileInfo*>* allFiles, RootTree* rootTree);
  void del();
  
};

#endif