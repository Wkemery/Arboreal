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
  
  return 0;
}