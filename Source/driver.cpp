/*Driver.cpp
 * Arboreal
 * October, 12, 2017
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "client.h"
#include <string>
#include <string.h>
using namespace std;

int main()
{

//   cout << dm->getPartitionSize("PartitionA") << endl;
  
//   dp[0].partitionName = 'A';
//   dp[0].partitionSize = 100;
//   dp[1].partitionName = 'B';
//   dp[1].partitionSize = 75;
//   dp[2].partitionName = 'C';
//   dp[2].partitionSize = 105;
  
//   DiskManager *dm = new DiskManager(d, 3, dp);

//   Client *c1 = new Client(fs1);
//   Client *c2 = new Client(fs1);
//   Client *c3 = new Client(fs1);
//   Client *c4 = new Client(fs2);
//   Client *c5 = new Client(fs2);
  
  try
  {
    //TODO: NOTE don't really want to put a bunch of these in a single try becuase it will quit if one fails
    Disk *d = new Disk(100, 512, const_cast<char *>("DISK1"));
    DiskManager * dm = new DiskManager(d);
    
    FileSystem *fs1 = new FileSystem(dm, "PartitionA");
    FileSystem *fs2 = new FileSystem(dm, "PartitionB");
    FileSystem *fs3 = new FileSystem(dm, "PartitionC");
    
    
    
//     fs1->createTag("tag1");
    fs1->createTag("tag1");
    
    fs1->createTag("tag2");
    fs1->createTag("tag3");
    
    fs2->createTag("tag1");
    fs2->createTag("tag2");
    fs2->createTag("tag3");
    
    fs3->createTag("tag1");
    fs3->createTag("tag2");
    fs3->createTag("tag3");
    fs1->printRoot();
    cout << endl;
    
    fs1->deleteTag("tag1");
    fs2->deleteTag("tag2");
    fs3->deleteTag("tag3");
    fs1->printRoot();
    
    fs1->writeOut();
    fs2->writeOut();
    fs3->writeOut();
    
  }
  catch(std::exception& e)
  {
    cout << e.what() << endl;
  }

  
  return 0;
}
