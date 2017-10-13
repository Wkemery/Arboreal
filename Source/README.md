This is a file for specific code notes. things to do, consider, etc, that doesn't need to clutter up the main readme file.

//TODO: think about a better way to error handle. returning an int = bad. We can leave it for now but, maybe some sort of try catch stuff?? yeah I'm definitely thinking try catch and when theres an error we'll throw an exception.

//TODO: what do we return from tagSearch()? obviously, its a "list" of files. but what does that mean? do we want some sort of file object that contains perhaps a unique file id number, which i think we may need at some point. we'll need the filename for sure to display to the user. but when the user clicks on it, we need to have a quick easy way to open. It would be stupid to search for that file again just to open it. I'm thinking a small file object, with a few members like diskblknode number, attributes, tags, name etc...

There should probably be an attributes object to make our lives easier. and thats what real filesystems do.

Maybe the diskblocknode number of the finode for a file can be its unique id??

I think we may need two open functions. One that takes the unique file id,(block number) and one that takes the vector of tags and the file name . similar to a path. 

I removed validName() because we should check for valid input before passing it to our filesystem. as much as possible anyway.

I think we'll be able to get rid of alot of the helper functions actually. because map will be able to do all that for us. the big helper functions will be reading in a map and writing out a map. which i think we can just basically write out all the key, value pairs, because a map can do that easily with its iterator. for reading in, we'll just read in all the key value pairs and add them to the map one by one. the tough part might be knowing where a tree stops and starts. I'm thinking we need to do this in a fixed length manner. i.e every key value pair takes up exactly the same space, so maybe some padding can be used. this wastes space, but... Its probably better. either that, or we'll need some sort of delimiter. We should set a hard cap on the filename length. we can base it off block size though. 

right now I'm thinking we'll need to write out the intermediary data structure that is the value in the key value pair. It will be small so we should just write out each value in sequence then read them in construct the object and add it as a value. when we write out, we'll need to keep in mind that we need to tie togehter blocks if data is too big. so we'll have to have a reserved spot at the end of a block for a block number to the next block of continuing data. 

We should write everything out in plaintext and have a converter that can change it to byte stuff that we can implement later. also we should have a flag that will zero out blocks, mainly for debugging. but can also repourpose to an encrypt flag later. 




//LATER: we should try not to write out the whole tag tree everytime. instead we should only write out the parts that changed if we can. I know this is a tough solution, if a tag is deleted in the middle of the tree and we really have no way of knowing where stuff will be in the tree... but it might be possible to keep some sort of secondary data structure, like a vector with all the info because it doens't matter what order we reconstruct the map in memory, just that all the data is there. this is also somehting we can implement later.
