This is a file for specific code notes. things to do, consider, etc, that doesn't need to clutter up the main readme file.

**Doing TRY-CATCH** 

**tageSearch() returns a vector of structs with (string "filename", int fidentifier) [fidentifier can be FIONODE blknum or unique file identifier that is mapped to a FIONDE blknum]** 
**Hand off storage of file tagSearch() return vector to Danny to be stored in a "current" buffer or smoe such**/

There should probably be an attributes object to make our lives easier. and thats what real filesystems do.
**Attributes object should be stored in FINODE or another indirect block who's reference is stored in the FINODE.  Which one is used should be decided dynamically, if FIONDE is full get empty data block, store address in FIONDE (migrate data)[optional] to new block, add new data to new block, otherwise add data directly to FIONDE.  TAGS ARE ATTRIBUTES**

I think we may need two open functions. One that takes the unique file id,(block number) and one that takes the vector of tags and the file name . similar to a path. **YES**

*I removed validName() because we should check for valid input before passing it to our filesystem. as much as possible anyway.*

I think we'll be able to get rid of alot of the helper functions actually. because map will be able to do all that for us. the **big helper functions will be reading in a map and writing out a map**. which i think we can just basically write out all the key, value pairs, because a map can do that easily with its iterator. **for reading in, we'll just read in all the key value pairs and add them to the map one by one.** 
**Name Length HARD CAPS at size specified in partition info during formatting**
**NEED TREE INODE**
**READING A MAP FROM DISK TO MAIN MEMORY**
**------------------------------------------**

 **so we'll have to have a reserved spot at the end of a block for a block number to the next block of continuing data.** 

**We should write everything out in plaintext and have a converter that can change it to byte stuff that we can implement later. also we should have a flag that will zero out blocks (FOR SPEED), mainly for debugging. but can also repourpose to an encrypt flag later.** 



//LATER: we should try not to write out the whole tag tree everytime. instead we should only write out the parts that changed if we can. I know this is a tough solution, if a tag is deleted in the middle of the tree and we really have no way of knowing where stuff will be in the tree... but it might be possible to keep some sort of secondary data structure, like a vector with all the info because it doens't matter what order we reconstruct the map in memory, just that all the data is there. this is also somehting we can implement later.
**INtermeidary Data structure will store, (in addition to Memory pointer, block pointer) a tuple (int blknum, int pos_in_blknum) of the key_value pair so we can use it later for delete operations.** 

** A NOTE about speed: 
 right now, in order to do tag search, we have to read in the finode of each file in the smallest tag tree becuase I am not storing the number of tags associated with a file in the tag tree inodes. This can be changed later, but for now I just want to get it done. If, when we are testing speeds this is something that will surely improve speed.

** Estimated read in time:
O(n^2*log(n))

**Restrictions:
1. filename size restricted to no more than 1/2 block size
2. block size should be a power of 2
3. Hard cap on the number of tags that can be associated with a file. = (((blocksize / 2) - (17 * sizeof(BlkNumType))) / sizeof(BlkNumType)) + (blocksize / sizeof(BlkNumType)). 72 tags for blocksize of 512. 367 for 2k blocksize
4. 

**TODO:**
1. Incorporate storing number of tags associated with file in Tag tree on disk, not yet

