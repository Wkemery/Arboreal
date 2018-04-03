#Arboreal User Guide
---
###Table of Contents


* **Installing Arboreal**
* **Starting The Command Line**
* **Valid Commands and Their Syntax**
	* **Help** 
	* **Quit**
	* **Find**
	* **New**
	* **Delete**
	* **Tag**
	* **Merge**
	* **Untag**
	* **Open**
	* **Close**
	* **Read**
	* **Write**
	* **Copy**
	* **Rename**
	* **Get File Attributes**
	* **Change Working Directory**
* **The Graphical User Interface (GUI)**
* **Troubleshooting**  

---  
##Installing Arboreal
Arboreal is currently not integrated with the kernel and as such runs similarly to a virtual file system albeit with a more experimental structure.  Future work will be focused on direct integration with the kernal in order to provide more traditional  usability.  In the meantime playing around with and testing the file system can be achived through a few easy steps:   

1.  **Download the project**  
2. **Changed directory to the folder within the project hierarchy named `Source`**  
3. **Type `make`**  
4. **You will now need to first run the daemon process.  This process intercepts al communication 	attempts with the File System and will execute functions accordingly.  There are a number of 	command line arguments that can be passed to the daemon:**  
	* `-d` **This flag is used to tell the daemon to enable debugging**  
	* `-v` **This flag is used to tell the daemon to return file information (such as that returned by a call 			to find) with as much information as possible.  Omitting this flag will cause the daemon to 			return a reduced version of file information**  
	* **You may enable either of these options or both (input order does not matter, that is `-d -v` will 		work the same as `-v -d`)**
5.   **Finally, Simply Type `./daemon ` followed by your chosen flag or flags (be sure to include a space in between).**  **For example, if I wanted to run the daemon with verbose file information and debugging enabled the command would look like: `./daemon -v -d`**  

At this point you'll be ready to move on to the next step, starting the command line or GUI interface.  Notice that the daemon does not output anything to the screen as it is running.  **This is OK!**  Its whole purpose is to be a background process that aids communication with the file system.  **If you decided to enable debugging,  the output will be located in a file called `Arboreal.log`.** 

**A final note:**  
By typing make, the "disk" will be formatted for you with the default values and partition names/ counts.  It is possible to change these to better suit your needs however it is a little bit more involved.   

1.  **Open and edit a file called `diskInfo.d` (It is located in the `Source` folder)**
2. **Using the following syntax,  edit the file as you see fit:**
	*  **Line 1 needs to always be:** `Diskfile name, number of blocks on disk, size of each block in bytes,  number of partitions` **(Omit the commas in favor of spaces)**
	*  **Lines 2 - X need to always be:** `Partition name, number of blocks in the partition, maximum filename size` **(Again omit commas in favor of spaces)**  
	* **There are some restrictions on allowed values for the `diskInfo` file to see these please checkout the Arboreal Technical Documentation**
3. **Next you will have to open `daemon.cpp` ( it is located under `Source/Filesystem/`) and edit this line of code:**  
	* `d = new Disk(#1, #,2 const_cast<char *>("diskfile_name"));`

	* **Change** `#1` **to whatever value you picked for** `number of blocks` **in** `diskInfo.d`**.** 
		* So if I decided that I wanted 4000 blocks I would type `4000` in for `#1` ( **The number here and in** `diskInfo.d` **MUST MATCH**).
	* **Change** `#2` **to whatever value you picked for** `block size in bytes` **in** `diskInfo.d`
		* So if I decided that I wanted blocks to be 4096 bytes large I would type `4096` in for `#2` ( **Once 			again I stress that the number here and in** `diskInfo.d` **MUST MATCH**)
	* **Finally, change** `diskfile_name` **to the name of the disk file you chose in** `diskInfo.d`
4. **You are now almost ready, the final step is to type** `make clean` **followed by** `make` **in the shell and run the through the same steps as above for starting the daemon**
5. **You are now good to go!**

## Starting The Command Line
**Before beginning anything below, make sure that a daemon process (and ONLY ONE daemon process) is running, if the command line cannot connect to the daemon process it will quit on startup with an appropriate error message**  

The command line utility has multiple optional arguments but it does contain a single mandatory argument.  This is the **Partition Name** that the command line will be working on.  If no partition name is given the command line will fail on startup with an appropriate error message.  Additionally it is important to note that **the partition that is given to the command line must already exist**.  If it does not, an appropriate error will be thrown. Finally, **it does not matter if the partition is already in use by another command line  and it does not matter how many command lines are currently active**, in both cases you will still be able to work wth the file system (provided that the partition you gave exists).  After providing the partitoon name you are free to run the command line.  However, should you wish to, there are some optional command line arguments:  

* `-d` **This argument will enable debugging for both the command line and the liaison process**  
* `-s` **This argument will alert the command line that input will be coming from a file rather than a user**
* `-s -d` **This will enable debugging for the command line AND alert it to the fact that input will be coming from a file rather than a user**

For example, if i wished to pipe input from a file to the command line and enable debugging, I would run the command line process like so:
`./commandline PartitionName -s -d < some_random_file.ext`  

(*Note that* `./commandline -d -s < some_random_file.ext` *will not work, that is, make sure the debug flag comes last!*)

But if I wanted to just enable debugging and read from user input, I would run the command line process like so:  
`./commandline PartitionName -d`  

At this point you should see the arboreal header and `Arboreal >>` indicating that the command line is ready to accept input.  **To send input to the command line simply type the command you wish to execute** (see *Valid Commands And Their Syntax* section for commands or type `help` or `h`) **and press enter.** 

**Note**  
If you chose to enable debugging for the command line, all debug output will be written to a file named `Arboreal.log`.  Do not worry if this file does not yet exist, it will be created for you on startup.

##Valid Commands And Their Syntax

###Help Commands
	Arboreal >> help
	Arboreal >> h
**These two commands will bring up a helper subprocess which will display a list of the command archetypes and show the user the specific commands (and their syntax) that are housed under each archetype.** The helper subprocess continues running until the user decides to quit it.

	Arboreal >> -h --command_archetype
	
	e.g.
	Arboreal >> -h --find	
**This version of the help command will show the usage for a single command archetype.**  (Unlike the `help` or `h` commands it will not start a  "helper" subprocess but will simply display the usage for the particular archetype and await the next file system command)  
  
  ----------------------------------

###Quit Commands
	Arboreal >> quit
	Arboreal >> q
	Arboreal >> Q
**All of these will attempt to terminate the current command line process.**  This command does not affect other concurrently running command lines it will only quit the currently active command line process.  The user must confirm the quit before the command will actually be executed.  this is to prevent accidental quits.  The quit commands are built with proper cleanup in mind and should not leave any junk behind.  
  
  ---
###Find Commands
	Arboreal >> find -t [tagname1,tagname2,...]
	Arboreal >> find -t {tagname1,tagname2,...}
	Arboreal >> find -t [tagname1,{tagname2,tagname7,...},tagname10,...]
	Arboreal >> find -t {tagname1,tagnam3,[tagname5,tagname6,...],...}
**This command searches for files by tag.** It is quite powerful and allows you to search for any combination of tags.  Commands that use `{}` are called `sets` and will tell the file system to **search for ALL files which are tagged with ALL of the specified tags.**  You can think of this as a bunch of `&&` operations, that is, you want a file tagged with :  

`{ this tag, and this tag, and this tag, ... etc}`  

-
Commands that use `[]` are called `lists` and will tell the system to **search for ANY file which is tagged with ANY of the tags specified.**  You can think of this as a bunch of `||` operations, that is, you want a file tagged with:  

`[this tag, or this tag, or this tag, ... etc]`  

-
  **What's great is that you can actually nest any of these within one another!** Although nesting a bunch of `sets` or `lists` won't be any diffferent from simply using one big list or set (i.e. `[t1,[t2,t3,t4]]` is the exact same as `[t1,t2,t3,t4]` this goes for `sets` as well).  However, tings get interesting when you pass a command such as:
    
  `find -t [tag1,tag2,{tag45,tag78,[tag9,tag10],tag5},tag100]`  
  
  This particular command will search for any file with:  
 
  ~~~
   	tag1  	 
   	tag2  
   	tag100  
   	tag1 && tag45 && tag78 && tag9 && tag5 && tag100  
   	tag1 && tag45 && tag78 && tag10 && tag5 && tag100  
   	tag2 && tag45 && tag78 && tag9 && tag5 && tag100  
   	tag2 && tag45 && tag78 && tag10 && tag5 && tag100 
  ~~~
   	
   (*Of course you accomplish similar things even with a command that is a `list` nested within a `set` rather than this example which is a `set` nested within a `list`*)
   
-
   
   As you can see, nesting these operations creates some really powerful search options!  
###Important! DO NOT put spaces in between the `list` or `set` items!!  
-
~~~
Arboreal >> find -f [file1,file2,...]
~~~

   	




















> This is a file for specific code notes. things to do, consider, etc, that doesn't need to clutter up the main readme file.

> **Doing TRY-CATCH** 

> **tageSearch() returns a vector of structs with (string "filename", int fidentifier) [fidentifier can be FIONODE blknum or unique file identifier that is mapped to a FIONDE blknum]** 
**Hand off storage of file tagSearch() return vector to Danny to be stored in a "current" buffer or smoe such**/

> There should probably be an attributes object to make our lives easier. and thats what real filesystems do.
**Attributes object should be stored in FINODE or another indirect block who's reference is stored in the FINODE.  Which one is used should be decided dynamically, if FIONDE is full get empty data block, store address in FIONDE (migrate data)[optional] to new block, add new data to new block, otherwise add data directly to FIONDE.  TAGS ARE ATTRIBUTES**

> I think we may need two open functions. One that takes the unique file id,(block number) and one that takes the vector of tags and the file name . similar to a path. **YES**

> *I removed validName() because we should check for valid input before passing it to our filesystem. as much as possible anyway.*

> I think we'll be able to get rid of alot of the helper functions actually. because map will be able to do all that for us. the **big helper functions will be reading in a map and writing out a map**. which i think we can just basically write out all the key, value pairs, because a map can do that easily with its iterator. **for reading in, we'll just read in all the key value pairs and add them to the map one by one.** 
**Name Length HARD CAPS at size specified in partition info during formatting**
**NEED TREE INODE**
**READING A MAP FROM DISK TO MAIN MEMORY**
**------------------------------------------**

> **so we'll have to have a reserved spot at the end of a block for a block number to the next block of continuing data.** 

> **We should write everything out in plaintext and have a converter that can change it to byte stuff that we can implement later. also we should have a flag that will zero out blocks (FOR SPEED), mainly for debugging. but can also repourpose to an encrypt flag later.** 



> //LATER: we should try not to write out the whole tag tree everytime. instead we should only write out the parts that changed if we can. I know this is a tough solution, if a tag is deleted in the middle of the tree and we really have no way of knowing where stuff will be in the tree... but it might be possible to keep some sort of secondary data structure, like a vector with all the info because it doens't matter what order we reconstruct the map in memory, just that all the data is there. this is also somehting we can implement later.
**INtermeidary Data structure will store, (in addition to Memory pointer, block pointer) a tuple (int blknum, int pos_in_blknum) of the key_value pair so we can use it later for delete operations.** 

> **A NOTE about speed: 
right now, in order to do tag search, we have to read in the finode of each file in the smallest tag tree becuase I am not storing the number of tags associated with a file in the tag tree inodes. This can be changed later, but for now I just want to get it done. If, when we are testing speeds this is something that will surely improve speed.**

> **Estimated read in time for everything on startup:
O(n^2*log(n))***

> **FileInode structure
filename - filenameSize
Finode struct = sizeof(finode struct)
local tag storage = rest of the space
possible tag cont. block = sizeof(blknumType)**

> **Restrictions:**
> 1. filename size restricted to no more than 1/2 block size
> 2. block size should be a power of 2
> 3. Hard cap on the number of tags that can be associated with a file. = (((blocksize - filenamesize - 136) / sizeof(BlkNumType)) + (blocksize / sizeof(BlkNumType)). 103 tags for blocksize of 512. and 64b filename
> 4. max block size = 16k

> **TODO:**
> 1. Incorporate storing number of tags associated with file in Tag tree on disk, not yet
> 2. add renameTag function
> 3. don't allow duplicate tags to be sent to the filesystem when sending a tagset of any kind

