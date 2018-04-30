# Arboreal


# FlipGrid Link
**(For videos that are not on the communal flipgrid)**

*FLipGrip Link:* https://flipgrid.com/1tcwvu


# Time Sheet Link
**(Read only)**

https://docs.google.com/spreadsheets/d/1vL4BsiUzvJpM0IO-PRyWgokJArVKhy6LYVstcVRkcSU/edit?usp=sharing


---








### some notes

Tag based tree-structured filesystem

1. complete back end with drivers
2. implementing the CLI

We plan to use a map. The key will be something along the lines of the tree name. the value will be an intermediary data structure containing, tag tree pointer, location on disk, etc...

Then the tagtree structure is almost the same thing as the root tree but uses a map of key=filename value = another intermediary structure contaning the location on disk, and maybe other things that I can't think of right now. 

overhead of loading stuff into memory.
	We will load tagtrees into memory as needed. tagtrees will remain in memory until pushed out when we have deemed the memory taken up is too much. the least recently used tag trees will be put out first. Maybe we can try to keep the largest tag trees in memory to maximize the possiblity of us having what the user needs ready. loading an entire tree into storage will take a little time. However, on first start up, maybe load up a few of the largest or most commonly used tag trees. The nice thing is that a tag tree does not take up a lot of space on disk or in memory. EG writing a map out to disk, which as of right now i don't know how to do,  has only two values, a string and a pointer to an object with more information. a map of size 1000 might only have a size of roughly 38KB. 

Some problems to consider in the future: 
how to write a map out to disk such that we can read it in and reconstruct it quickly.
Does FUSE even allow non directory based filesystems?



## End Goals
Limit Reduced functionality. (usable as a regular filesystem)
Finish Virtual version by end of semester. (command line)
Complete command line version. 
	includes all commands, create tag, list files, etc...
	
(Optional)
Hopefully, work to integrate with the Linux kernel, such that it is usable as a full fledged filesystem.


## Psudocode FileSystem


		// ***  TAGS MUST BE UNIQUE ***

		// ** ADD and REMOVE node both UPDATE TAG TREE SIZE


### What does the root tree look like in Storage: 

	[] = block of data
	[TagName, diskBlocknum of "TagName", TagName, diskBlocknum of "TagName",....]

### What does the tag tree look like in Storage:

	[] = block of data
	[Filename, Fionde Blknum, # of associated tags, Filename, Fionde Blknum, # of associated tags,.....]

### What does the file Inode look like:

	[] = block of data
	[filename, size, tags (as the associated tagTree blknum on disk), attributes, X-directblocks, and X indirect blocks]


### Searching by Tags:

	- Single tag:
		* find the tag in root tree
		* list files in tag tree pointed to by root tree

	- Multi Tag:
		* Use size field in root node of tag tree to find smallest tree among the tags you want to search
		* Search the smallest tree:
			# elimnate all nodes with tag count < the number of tags you are searching for
			# search remainng files for exact tag match O(n)
			# return (list) the found file(s)

	** THIS IS SEARCH SMALLEST **
This is an O(n) algorithm. However, we hope the smallest tree will actually be relatively small, because it is O(n) where n is the number of files in the smallest tag tree. In reality, it is very rare to have large numbers of files under a single tag. Even more rare to have 3 examples of that and have those three examples in a single search. Note: O(n) is unavoidable here because the output must consist of the n files you are requesting. 

Note: potential fix, sacrifice space for time. store the combinations of tags as tag trees themselves. eg a tag tree for docs and a tag tree for docs,2017 and a tag tree for docs,2017,homework. I think the space complexity gets too large to be feasible for this but maybe not...

### Searching by Filename:

	- Binary search for file name from largest to smallest tag tree
	- Worst case is (number of trees) * log(n)

### Creating a new Tag tree:

	- Make sure tag is unique
	- Get a block from disk to store tag tree
	- set up block in disk as empty tag tree
	- initialize tree in main memory
	- add respective node to root tree (write TagTree block num to Node as well as TagTree memory address to Node)
	- rebalance  Root tree
	- write Root tree to disk


### Deleting a tag:

	- CANNOT delete tag if tag tree Size > 0
	- remove node from Root tree
	- delete all references to the tag tree from Fileinodes
	- rebalance root tree
	- encryption requires no zero-ing of disk
	- write the root tree out to disk


### Merging Tags:

	- create new tag tree if needed
	- Move all Nodes in largest tag tree to new (assuming new tree was created otherwise add to existsing tree) Tree
		* delete refrences to old tags in Fionde as you go
	- Move Nodes of second Tag Tree:
		* check that node is not already in destination tree
		* if Yes: Skip
		* if NO: Add
		* Repete


### Creating a new File:

	- Get an open block and intiialize Fionde
	- If tag not given then add file to "default" tag tree
		* File remains in default tag tree until a non-default tag is associated with file
	- If tag is given and tag is NOT new
		* Call Tagfile()
	- else
		* call createTag()
		* call TagFile()


### Deleting a File:

 	NOTE: File will be referenced by Finode block number
	- check to make sure file exists
	- dissasociate all tags from the file (Call untagFile())
	- Free all data blocks
	- Free Finode block on disk
	- Write Updated Tag Tree to Disk


### Tagging a File:

 	- If tag does not exist create a new Tag tree
	- Get From Finode: Blknum
	- Get From Finode: Number of Tags
	- Create and Add new Node to TagTree
	- Add Tag to Fionde (write updated Fionde to disk)
	- Update TagTree Size
	- Write updated TagTree to disk


### Untagging A File

	- Use Filblknum that was passed to search the TagTree
	- Remove Node from TagTree
	- Delete tag from FileInode (reduce "# of tags" Fionde feild by X where X is number of tags removed)
	- write updated tag tree to disk



### Open a file:

	** Direct File Operations are pretty much the same as OS **
	- Get File Blknum
	- Get Tags from Finode
	- Steal Code from OS to see what else we need to do


### Closing a File:

	- Get File Blknum
	- Get Tags from Finode
	- Steal Code from OS to see what else we need to do



	




	



	














