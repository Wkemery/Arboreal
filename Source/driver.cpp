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
#include "Trees.h"
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
  Disk *d = new Disk(501, 512, const_cast<char *>("DISK1"));
  DiskManager * dm = new DiskManager(d);
  
  FileSystem *fs1 = new FileSystem(dm, "PartitionA");
//   FileSystem *fs2 = new FileSystem(dm, "PartitionB");
//   FileSystem *fs3 = new FileSystem(dm, "PartitionC");
  
  switch(driverNumber)
  {
    case 0:
    {
      try 
      {
        cout << "Root Tree and Tag Tree's Contents:" << endl;
        cout << "Partition A" << endl;
        //       fs1->printRoot();
        fs1->printTags();
        cout << endl << endl;
        
        cout << "Partition B" << endl;
        //       fs2->printRoot();
//         fs2->printTags();
        cout << endl << endl;
        
        cout << "Partition C" << endl;
        //       fs3->printRoot();
//         fs3->printTags();
        cout << endl << endl;
      }
      catch(arboreal_exception& e)
      {
        cerr << "Error! " << e.what() << " in " << e.where()<< endl;
      }

      break;
    }
    case 1:
    {
      try 
      {
        fs1->createTag("tag1");
        
        fs1->createTag("tag2");
        fs1->createTag("tag3");
        
//         fs2->createTag("tag1");
//         fs2->createTag("tag2");
//         fs2->createTag("tag3");
        
//         fs3->createTag("tag1");
//         fs3->createTag("tag2");
//         fs3->createTag("tag3");
        fs1->printRoot();
        fs1->writeChanges();
//         fs2->writeChanges();
//         fs3->writeChanges();
        cout << endl;
        
        //         fs1->deleteTag("tag1");
        //         fs2->deleteTag("tag2");
        //         fs3->deleteTag("tag3");
        fs1->printRoot();
        
        fs1->writeChanges();
//         fs2->writeChanges();
//         fs3->writeChanges();
      }
      catch(arboreal_exception& e)
      {
        cerr << "Error! " << e.what() << " in " << e.where()<< endl;
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
      catch(arboreal_exception& e)
      {
        cerr << "Error! " << e.what() << " in " << e.where()<< endl;
      }
      break;
    }
    case 3:
    {
      vector<string> tagSet;
      try{fs1->createFile("myfile1", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {cerr << "Error! " << e.what() << " in " << e.where() << endl;}
      tagSet.push_back("tag2");
      try{fs1->createFile("myfile2", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      tagSet.push_back("badtag");
      try{fs1->createFile("myfile3", tagSet);}//
      catch(arboreal_exception& e) {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      break;
    }
    case 4:
    {
      vector<FileInfo*>* foundFiles;
      cout << "Searching for myfile1:" << endl;
      try{foundFiles = fs1->fileSearch("myfile1");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << "found " << foundFiles->size() << " files" << endl;
      delete foundFiles; foundFiles = 0;
      
      
      cout << "Searching for myfile2:" << endl;
      try{foundFiles = fs1->fileSearch("myfile2");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << "found " << foundFiles->size() << " files" << endl;
      delete foundFiles; foundFiles = 0;
      
      
      
      break;
    }
    case 5:
    {
      string tagName = "myTag";
      for(int i = 0; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }

      fs1->writeChanges();
      
      cout << "Creation Errors:" << endl;
      for(int i = 0; i < 5; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << endl;
      
      cout << "Deletion Errors:" << endl;
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << endl;
      
      string fileName = "myFile";
      vector<string> tags;
      /*Create 5 files tagged with default*/
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*Create 5 files tagged with tag 0*/
      tags.push_back("myTag0");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*create 5 files tagged with 0,2,4*/
      tags.push_back("myTag2");
      tags.push_back("myTag4");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*Create 5 files tagged with 1,3*/
      tags.clear();
      tags.push_back("myTag1");
      tags.push_back("myTag3");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*Find those 5 files*/
      vector<FileInfo*>* foundFiles;
      try{foundFiles = fs1->tagSearch(tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      if(foundFiles->size() != 5)
      {
        cerr << "TagSearch error. ONly " << foundFiles->size() << " files found" << endl;
      }
      
      /*Create 5 files tagged with 1,3,5*/
      tags.clear();
      tags.push_back("myTag1");
      tags.push_back("myTag3");
      tags.push_back("myTag5");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*Try to tag the previous 5 files with tag 5*/
      cout << "Tag File Errors: " << endl;
      tags.clear();
      tags.push_back("myTag5");
      for(size_t i = 0; i < foundFiles->size(); i++)
      {
        try{fs1->tagFile(foundFiles->at(i), tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      }
      
      cout << endl;
      
      /*try to create another file with tags 1,3,5*/
      cout << "File Creation Errors: SHould be 2" << endl;
      tags.clear(); 
      tags.push_back("myTag1");
      tags.push_back("myTag3");
      tags.push_back("myTag5");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      /*Try to create another file with tags 0,2,4*/
      
      tags.clear(); 
      tags.push_back("myTag0");
      tags.push_back("myTag2");
      tags.push_back("myTag4");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      cout << endl;
      
      /*Try to delete a tag that DNE*/
      cout << "Deleting a badtag : should be 1" << endl;
      try{fs1->deleteTag("badtag");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      /*Delete tag1 without force*/
      cout << "No force tag deletion failure : shoudl be 1" << endl;
      try{fs1->deleteTag("myTag1");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << endl;
      
      
      /*Delete tag1 with force*/
      try{fs1->deleteTag("myTag1", true);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << endl;
      
      fs1->writeChanges();
      
      /*Try to create some files with tag1, should issue warning*/
      tags.clear();
      tags.push_back("myTag1");
      tags.push_back("myTag3");
      tags.push_back("myTag5");
      
      cout << "Creating files with nonexistent tag: should be 1 warning" << endl;
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << endl;
      
      fs1->writeChanges();
      
      break;
    }
    default:
    {
      cerr << "Driver Error! Behavior not defined for specified number" << endl;
    }
  }
  return 0;
  
}




