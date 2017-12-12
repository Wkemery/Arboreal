/*Format.cpp
 * Arboreal
 * December 2017
 */

#include"types.h"
#include<iostream>
#include<vector>
#include <stdlib.h>
#include<string.h>
#include<fstream>
using namespace std;


int main(int argc, char** argv)
{
  if(argc != 5)
  {
    cerr << "Error ! \nUsage: format DiskName NumberOfBlocks BlockSize NumberOfPartitions" << endl;
    return -1;
  }
  
  BlkNumType numBlocks = atoll(argv[2]);
  int blockSize = atoi(argv[3]);
  int numParts = atoi(argv[4]);
  
  /*Create disk*/
  ofstream disk;
  disk.open (argv[1], ios::binary);
  if(!disk)
  {
    cerr << "Could not create disk!" << endl;
    return -1;
  }
  char* buff = new char[blockSize];
  memset(buff, 0, blockSize);
  for(BlkNumType i  = 0; i < numBlocks; i++)
  {
    disk.write(buff, blockSize);
  }
  
  /*set up partition information*/
  vector<BlkNumType> partSizes;
  BlkNumType partStart = 0;
  int offset = 0;
  memcpy(buff, &numParts, sizeof(int));
  offset+= sizeof(int);
  for(int i = 0; i < numParts; i++)
  {
    string partitionName;
    cout << "Enter name for partition number " << i << " : ";
    cin >> partitionName;
    if(partitionName.length() > 16)
    {
      cerr << "All partition names must be no greater than 16 bytes long" << endl;
      return -1;
    }
    
    /*write out partition name*/
    strncpy(buff + offset, partitionName.c_str(), 16);
    offset+= 16;
    
    BlkNumType partitionsize;
    cout << "Enter size(in blocks) for partition " << partitionName << " : ";
    cin >> partitionsize;
    cout << partitionsize << endl;
    partSizes.push_back(partitionsize);
    /*write out partitionsize*/
    memcpy(buff + offset, &partitionsize, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    /*Write out parititionstart position*/
    memcpy(buff + offset, &partStart, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    partStart+= partitionsize;
  }
  
  disk.seekp(0);
  disk.write(buff, blockSize);
  

  /*Make free list*/
  for(unsigned int i = 0; i < partSizes.size(); i++)
  {
    memset(buff, 0, blockSize);
    
    BlkNumType prev = partSizes[i] - 1;
    BlkNumType next = 1;
    cout << partSizes[i] << endl;
    
    memcpy(buff, &next, sizeof(BlkNumType));
    memcpy(buff + sizeof(BlkNumType), &prev, sizeof(BlkNumType));
    disk.write(buff, blockSize);
    prev = 0;
    next++;
    for(BlkNumType k = 1; k < partSizes[i]; k++)
    {
      memset(buff, 0, blockSize);
      memcpy(buff, &prev, sizeof(BlkNumType));
      prev++;
      
      if(k == partSizes[i] - 1)
      {
        next = 0;
      }
      
      memcpy(buff + sizeof(BlkNumType), &next, sizeof(BlkNumType));
      next++;
      
      disk.write(buff, blockSize);
    }
  }
  
  disk.close();
  
  return 0;
}