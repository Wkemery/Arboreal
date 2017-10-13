
/* This is an example of a driver to test the filesystem */

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
  Disk *d = new Disk(300, 64, const_cast<char *>("DISK1"));
  DiskPartition *dp = new DiskPartition[3];

  dp[0].partitionName = 'A';
  dp[0].partitionSize = 100;
  dp[1].partitionName = 'B';
  dp[1].partitionSize = 75;
  dp[2].partitionName = 'C';
  dp[2].partitionSize = 105;

  DiskManager *dm = new DiskManager(d, 3, dp);
  FileSystem *fs1 = new FileSystem(dm, 'A');
  FileSystem *fs2 = new FileSystem(dm, 'B');
  FileSystem *fs3 = new FileSystem(dm, 'C');
  Client *c1 = new Client(fs1);
  Client *c2 = new Client(fs1);
  Client *c3 = new Client(fs1);
  Client *c4 = new Client(fs2);
  Client *c5 = new Client(fs2);

  
  

  int r;
  char fname[2];
  fname[0] = '/';
  fname[1] = 'a';
  fname[2] = 0;
  for(int i = 0; i < 15; i++)
  {
    r = c1->myFS->createFile(fname, 2);
    fname[1]++;
  }

  r = c1->myFS->createDirectory(const_cast<char *>("/z"), 2);

  r = c1->myFS->createDirectory(const_cast<char *>("/z/y"), 4);

  r = c1->myFS->createDirectory(const_cast<char *>("/z/y/x"), 6);

  r = c1->myFS->createDirectory(const_cast<char *>("/z/y/x/d"), 8);

  r = c1->myFS->createFile(const_cast<char *>("/z/y/x/d/f"), 10);

 // r = c1->myFS->deleteFile(const_cast<char *>("/z/y/x/d/f"), 10);

 // r = c1->myFS->deleteFile(const_cast<char *>("/z/y/x/d/f"), 10);
//  cout << r << " = -1" << endl;

  r = c1->myFS->createFile(const_cast<char *>("/z/y/x/d/f"), 10);
  // Multilevel lock
  r = c1->myFS->lockFile("/z/y/x/d/f",10);
  cout << "Multilevel lock: "<< r <<endl;
  // Multilevel Unlock
  int r2 = c1->myFS->unlockFile("/z/y/x/d/f", 10, r);
  cout << "Multilevel Unlock: " << r2 << endl<<endl;

  r = c1->myFS->createDirectory(const_cast<char *>("/z/y/x/d/g"), 10);
  cout << r << " = 0" << endl;

  r = c1->myFS->createDirectory(const_cast<char *>("/z/y/x/d/h"), 10);
  cout << r << " = 0" << endl;
  
  
 // r = c1->myFS->deleteDirectory(const_cast<char *>("/z/y/x/d"), 8);
 // cout << r << " = -2" << endl;

  r = c1->myFS->createFile(const_cast<char *>("/a/f/t/y/f"), 10);
  cout << r << " = -4" << endl;

  r = c1->myFS->createFile(const_cast<char *>("/a/y/x/d/f"), 10);
  cout << r << " = -4" << endl;

  //r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/f"), 10, "/z/y/x/d/a", 10);
//  cout << r << " = 0" << endl;

 // r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/g"), 10, "/z/y/x/d/d", 10);
  //cout << r << " = 0" << endl;
 // r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/g"), 10, "/z/y/x/d/q", 10);
  //cout << r << " = -2" << endl;

 // r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/d"), 10, "/z/y/x/d/a", 10);
  //cout << r << " = -3" << endl;

// r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/a"), 10, "/z/y/x/d/d", 10);
 //cout << r << " = -3" << endl;
//
// r = c1->myFS->renameFile(const_cast<char *>("/z/y/x/d/t"), 10, "/z/y/x/d/p", 10);
//cout << r << " = -2" << endl;

   // Test locking success
   r = c1->myFS->lockFile(const_cast<char *>("/a"), 2);
   cout << "Lock return value = "<< r << endl;
   // Test unlocking success
   int ret = c1->myFS->unlockFile(const_cast<char *>("/a"), 2, r);
   cout << "unlock return value = "<< ret << endl;
   // Test Lock Failure: File DNE: works
   r = c1->myFS->lockFile(const_cast<char *>("/z"),2);
   cout << "FILE DNE(/z): lock return value = "<< r << endl<<endl;
   // Test lock of 2 files: Check unique Lock_ID: works
   r = c1->myFS->lockFile(const_cast<char *>("/c"), 2);
   cout << "Lock ID #1 = " << r << endl;

   
   ret = c1->myFS->lockFile(const_cast<char *>("/b"), 2);
   cout << "Lock ID #2 = " << ret << endl;
   // Test for unlock of multiple files: 
    int u = c1->myFS->unlockFile(const_cast<char *>("/c"), 2, r);
   cout << "Unlock ID #1 = " << u << endl;
   int u2 =c1->myFS->unlockFile(const_cast<char *>("/b"), 2, ret);
   cout << "Unlock ID #2 = " << u2 << endl;


   ret = c1->myFS->lockFile(const_cast<char *>("/b"), 2);
   cout << "Test for multiple lock: "<< ret << endl;
   r = c1->myFS->lockFile(const_cast<char *>("/b"), 2);
   cout << "Test for multiple lock: "<< r << endl;
   r = c1->myFS->unlockFile("/b", 2, ret);
   cout << "Unlock /b: " << r << endl;
   // Attempt to open a file: Success
//   ret = c1->myFS->openFile(const_cast<char *>("/z/y/x/d/f"), 10, 'w', r);
//   cout << "File #1 OPEN return value: " << ret << endl;

   // Attempt to close a file: Success
//   int closeVal = c1->myFS->closeFile(ret); // used the returned FILE DESCIPTOR OF openFile
//   cout << "File #1 CLOSE return Value: " << closeVal << endl;

//    r = c1->myFS->createFile("/z/a", 4);

   int filea = c1->myFS->openFile(const_cast<char *>("/a"), 2, 'w', -1);
   cout << "File open return val: " << filea << endl;

   r = c1->myFS->lockFile(const_cast<char *>("/a"), 2);
   cout << "Locking an OPEN FILE: -3 = " << r << endl;

  

   string message = "hello this is the first write test.";
   r= c1->myFS->writeFile(filea, message.c_str(), message.length());
   cout << "Write return value: "<< r << endl;

  string message2 = "Start second write test. need more bytes to fill up first direct address block.";
  r= c1->myFS->writeFile(filea, message2.c_str(), message2.length());
  cout << "Write 2 return value: "<< r << endl;

  /* this one actually has enough bytes to start on indirect inode writing. */
  string message3 = "Start third write test. again just making bytes to fill second direct address block.";
  r= c1->myFS->writeFile(filea, message3.c_str(), message3.length());
  cout << "Write 3 ret val (indirect addr): "<< r << endl;

  r = c1->myFS->closeFile(filea);
  cout << "Close file A: " << r << endl;
  r = c1->myFS->deleteFile("/a", 2);
  cout << "Delete File A: " << r << endl;

 int fileb = c1->myFS->openFile(const_cast<char *>("/b"), 2, 'w', -1);
 char buff[64];
 memset(buff, 'J', 64);

 for(int i = 0; i < 19; i++)
 {
   r = c1->myFS->writeFile(fileb, buff, 64);
 }
//    for(int i = 0; i < 15; i++)
//  {
//    r = c1->myFS->writeFile(fileb, buff, 64);
//  }
//  r = c1->myFS->writeFile(fileb, buff, 64);
//  cout << r << endl;
//   r = c1->myFS->writeFile(fileb, buff, 64);
//  cout << r << endl;
// r = c1->myFS->writeFile(fileb, buff, 128);

  r = c1->myFS->closeFile(fileb);
  r = c1->myFS->deleteFile("/b", 2);
//   fname[1] = 'a';
//
//   r = c1->myFS->deleteFile(const_cast<char *>("/a"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/a"), 2);
//   cout << r << " = -1"<< endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/k"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/l"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/m"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/d"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/n"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/o"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/h"), 2);
//   cout << r << endl;
//   r = c1->myFS->lockFile(const_cast<char *>("/p"), 2);

//   r = c1->myFS->createFile(const_cast<char *>("/p"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/q"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/r"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/s"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/t"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/u"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/t"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/v"), 2);
//   cout << r << endl;
//
//   r = c1->myFS->createDirectory(const_cast<char *>("/b"), 2);
//   cout << r << " = -4"<< endl;
//   r = c1->myFS->createDirectory(const_cast<char *>("/z"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteFile(const_cast<char *>("/b"), 2);
//   cout << r << endl;
//   r = c1->myFS->createDirectory(const_cast<char *>("/b"), 2);
//   cout << r << endl;
//   r = c1->myFS->createDirectory(const_cast<char *>("/x"), 2);
//   cout << r << endl;
//   r = c1->myFS->createDirectory(const_cast<char *>("/y"), 2);
//   cout << r << endl;
//   r = c1->myFS->createFile(const_cast<char *>("/w"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteDirectory(const_cast<char *>("/b"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteDirectory(const_cast<char *>("/y"), 2);
//   cout << r << endl;
//   r = c1->myFS->deleteDirectory(const_cast<char *>("/w"), 2);
//   cout << r << " = -1"<< endl;

  /*for(int i = 0; i < 15; i++)
  {
    r = c1->myFS->deleteFile(fname, 2);
    fname[1]++;
    cout << r << endl;
  }*/
  //r = c1->myFS->createFile(const_cast<char *>("/a"), 2);
  //r = c1->myFS->createFile(const_cast<char *>("/b"), 2);
  //r = c2->myFS->createFile(const_cast<char *>("/a"), 2);
  //r = c4->myFS->createFile(const_cast<char *>("/a"), 2);
  //r = c1->myFS->deleteFile(const_cast<char *>("/b"), 2)
  //int fd = c2->myFS->openFile(const_cast<char *>("/b"), 2, 'w', -1);
  //c2->myFS->writeFile(fd, const_cast<char *>("aaaabbbbcccc"), 12);
  //c2->myFS->closeFile(fd);
  
  delete c1;
  delete c2;
  delete c3;
  delete c4;
  delete c5;
  delete fs1;
  delete fs2;
  delete fs3;
  delete dm;
  delete d;
  delete dp;
  return 0;
}
