/*format.cpp
 * Arboreal
 * December 2017
 */

#include"../Filesystem/DaemonDependancies/Types/types.h"
using namespace std;

int main(int argc, char** argv)
{
  int fileNameSize;
  
  if(argc != 2)
  {
    cerr << "Error ! \nUsage: format filename" << endl;
    cerr << "Where the contents of filename has format: \n"<< 
    " diskname numblocks blocksize numpartitions" << endl;
    cerr << "(partitionName partitionSize(in blocks))*"<< endl << endl;
    cerr << "Ensure you list exacly the number of partiions specified with numpartitions " <<
    "and the number of blocks all add up. Else behavior is undefined." << endl;
    return -1;
  }
  
  ifstream file;
  file.open(argv[1], ios::in);
  vector<string> diskInfo;
  for(int i = 0; i < 4; i++)
  {
    string temp; 
    file >> temp;
    diskInfo.push_back(temp);
  }
  
  BlkNumType numBlocks = atoll(diskInfo[1].c_str());
  int blockSize = atoi(diskInfo[2].c_str());
  int numParts = atoi(diskInfo[3].c_str());
  
  /*Create disk*/
  ofstream disk;
  disk.open (diskInfo[0], ios::binary | ios::out);
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
  BlkNumType partStart = 1;
  int offset = 0;
  memcpy(buff, &numParts, sizeof(int));
//   cout << numParts << endl;
//   memcpy(&numParts, buff, sizeof(int));
//   cout << numParts << endl;
  offset+= sizeof(int);
  for(int i = 0; i < numParts; i++)
  {
    string partitionName;
    file >> partitionName;
    if(partitionName.length() > 16)
    {
      cerr << "All partition names must be no greater than 16 bytes long" << endl;
      return -1;
    }
    
    /*write out partition name*/
    strncpy(buff + offset, partitionName.c_str(), 16);
    offset+= 16;
    
    BlkNumType partitionsize;
//     cout << "Enter size(in blocks) for partition " << partitionName << " : ";
//     cin >> partitionsize;
    file >> partitionsize;
//     cout << partitionsize << endl;
    partSizes.push_back(partitionsize);
    /*write out partitionsize*/
    memcpy(buff + offset, &partitionsize, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    
    /*Write out parititionstart position*/
    memcpy(buff + offset, &partStart, sizeof(BlkNumType));
    offset+= sizeof(BlkNumType);
    partStart+= partitionsize;
    
    file >> fileNameSize;
    /*Write out filenamesize limit*/
    memcpy(buff + offset, &fileNameSize, sizeof(int));
    offset+= sizeof(int);
  }
  
  disk.seekp(0);
  disk.write(buff, blockSize);
  

  /*Make free list*/
  for(unsigned int i = 0; i < partSizes.size(); i++)
  {
    memset(buff, 0, blockSize);
    
    BlkNumType prev = partSizes[i] - 1;
    BlkNumType next = 4;
//     cout << partSizes[i] << endl;
    
    memcpy(buff, &next, sizeof(BlkNumType));
    memcpy(buff + sizeof(BlkNumType), &prev, sizeof(BlkNumType));
   
    /*Block 0 of part. is part super block*/
    disk.write(buff, blockSize);
    
    /*Block 1 of part. is root super block*/
    string rootName = "root";
    memset(buff, 0, blockSize);
    memcpy(buff, rootName.c_str(), rootName.size());
    struct index tempindex{2,0};
    struct rootSuperBlock tempSuperBlock{0, tempindex, 2};
    memcpy(buff + 5, &tempSuperBlock, sizeof(rootSuperBlock));
    disk.write(buff, blockSize);
    
    /*Block 2 of part. is root start block contains entry for default tag tree*/
    string def = "default";
    BlkNumType defTagStart = 3;
    memset(buff, 0, blockSize);
    memcpy(buff, def.c_str(), def.size());
    memcpy(buff + fileNameSize, &defTagStart, sizeof(BlkNumType));
    
    disk.write(buff, blockSize);
    
    /*Block 3 of part. is default tag tree super block*/
    memset(buff, 0, blockSize);
    
    strncpy(buff, def.c_str(), def.size());
    disk.write(buff, blockSize);
    
    prev = 0;
    next++;
    
    /*Start at partition Block 4*/
    for(BlkNumType k = 4; k < partSizes[i]; k++)
    {
      if(k == 5) prev = 4;
      
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