# Arboreal
Tag based tree-structured filesystem


## Psudocode FileSystem


		// ***  TAGS MUST BE UNIQUE ***

		// ** ADD and REMOVE node both UPDATE TAG TREE SIZE


### What does the root tree look like in Storage: 

	[] = block of data
	[TagName, diskBlocknum of "TagName", TagName, diskBlocknum of "TagName",....]

### What does the tag tree look like in Storage: [] = block of data

	[] = block of data
	[Filename, Fionde Blknum, # of associated tags, Filename, Fionde Blknum, # of associated tags,.....]

### What does the file Inode look like:

	[] = block of data
	[size, tags (as the associated tagTree blknum on disk), attributes, X-directblocks, and X indirect blocks]


### Searching by Tags:

	- Single tag:
		* find the tag in root tree
		* list files in tag tree pointed to by root tree

	- Multi Tag:
		* Use pin node to find smallest tree among the tags you want to search
		* Search the smallest tree:
			# elimnate all nodes with tag count != the number of tags you are searching for
			# search remainng files for exact tag match
			# return (list) the found file(s)

	** THIS IS SEARCH SMALLEST **


### Searching by Filename:

	- Binary search for file name from largest to smallest tag tree
	- Worst case i (number of trees) * log(n)

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
	- rebalnce root tree
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



	




	



	














