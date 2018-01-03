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

int main(int argc, char** argv)
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
  if(argc == 1)
  {
    cerr << "driver usage:\n driver number\n where number is the driver version we want to run." << endl;
    exit(1);
  }
  
  int driverNumber = atoi(argv[1]);
  
  //TODO: NOTE don't really want to put a bunch of these in a single try becuase it will quit if one fails
  Disk *d = new Disk(100, 512, const_cast<char *>("DISK1"));
  DiskManager * dm = new DiskManager(d);
  
  FileSystem *fs1 = new FileSystem(dm, "PartitionA");
  FileSystem *fs2 = new FileSystem(dm, "PartitionB");
  FileSystem *fs3 = new FileSystem(dm, "PartitionC");
  
  switch(driverNumber)
  {
    case 0:
    {
      cout << "Root Tree and Tag Tree's Contents:" << endl;
      cout << "Partition A" << endl;
//       fs1->printRoot();
      fs1->printTags();
      cout << endl << endl;
      
      cout << "Partition B" << endl;
//       fs2->printRoot();
      fs2->printTags();
      cout << endl << endl;
      
      cout << "Partition C" << endl;
//       fs3->printRoot();
      fs3->printTags();
      cout << endl << endl;
      
      break;
    }
    case 1:
    {
      try
      {
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
        fs1->writeChanges();
        fs2->writeChanges();
        fs3->writeChanges();
        cout << endl;
        
        fs1->deleteTag("tag1");
        fs2->deleteTag("tag2");
        fs3->deleteTag("tag3");
        fs1->printRoot();
        
        fs1->writeChanges();
        fs2->writeChanges();
        fs3->writeChanges();
        
      }
      catch(std::exception& e)
      {
        cout << e.what() << endl;
      }
      break;
    }
    case 2:
    {
      try
      {
        vector<string> tagSet;
        fs1->createFile("myfile1", tagSet);
        tagSet.push_back("tag2");
        fs1->createFile("myfile2", tagSet);
        tagSet.push_back("tag3");
        fs1->createFile("myfile3", tagSet);
        
        fs1->writeChanges();
      }
      catch(std::exception& e)
      {
        cout << e.what() << endl;
      }
      break;
    }
    default:
    {
      cerr << "Driver Error! Behavior not defined for specified number" << endl;
    }
      
  }


  
  return 0;
}
