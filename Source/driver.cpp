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
#include "Trees.h"
#include <string>
#include <string.h>
#include<unordered_set>
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
  Disk* d = 0;
  DiskManager* dm = 0;
  FileSystem* fs1 = 0;
  
  try
  {  
    d = new Disk(501, 512, const_cast<char *>("DISK1"));
    dm = new DiskManager(d);
    fs1 = new FileSystem(dm, "PartitionA");
  }
  catch(arboreal_exception& e)
  {
    cerr << "Error! " << e.what() << " in " << e.where()<< endl;
    exit(1);
  }
//   FileSystem *fs2 = new FileSystem(dm, "PartitionB");
//   FileSystem *fs3 = new FileSystem(dm, "PartitionC");
  
  switch(driverNumber)
  {
    case 0:
    {
      try 
      {
        string delimiter(80, '-'); 
        
        cout << "Partition A:" << endl;
        cout << delimiter << endl << endl;
        
        cout << "Root Tree" << endl;
        fs1->printRoot();
        cout << delimiter << endl << endl;
        
        cout << "Tag Tree Contents:" << endl;
        fs1->printTags();
        cout << delimiter << endl << endl;
        
        cout << "All Files:" << endl;
        fs1->printFiles();
        cout << delimiter << endl << endl;
        
        
//         cout << "Partition B" << endl;
        //       fs2->printRoot();
//         fs2->printTags();
//         cout << endl << endl;
        
//         cout << "Partition C" << endl;
        //       fs3->printRoot();
//         fs3->printTags();
//         cout << endl << endl;
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
        cout << "Creating 3 Tags, Creating 9 files where filenames are reused but with different \"paths\"" << endl;
        fs1->createTag("tag1");
        fs1->createTag("tag2");
        fs1->createTag("tag3");
        
//         fs1->printRoot();
        fs1->writeChanges();
        
        unordered_set<string> tagSet;
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        
        tagSet.insert("tag1");
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        
        tagSet.insert("tag2");
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        fs1->writeChanges();
        
//         fs1->deleteTag("tag1");
        
//         fs1->writeChanges();
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
        unordered_set<string> tagSet;
        fs1->createFile("myfile1", tagSet);
        tagSet.insert("tag2");
        fs1->createFile("myfile2", tagSet);
        tagSet.insert("tag3");
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
      unordered_set<string> tagSet;
      try{fs1->createFile("myfile1", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {cerr << "Error! " << e.what() << " in " << e.where() << endl;}
      tagSet.insert("tag2");
      try{fs1->createFile("myfile2", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      tagSet.insert("badtag");
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
      string fileName = "myFile";
      unordered_set<string> tags;
      /*Create 15 files tagged with default*/
      for(int i = 0; i < 15; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      
      /*Create 15 tags*/
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
      
      /*Delete the last 5 tags*/
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }

      fs1->writeChanges();
      
      /*Create tag 10 again*/
      try{fs1->createTag("myTag10");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fs1->writeChanges();
      
      cout << "Tag Creation Errors: should be 5 errors" << endl;
      for(int i = 0; i < 5; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << endl;
      
      cout << "Tag Deletion Errors: should be 4 errors" << endl;
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << endl;
      

      
      /*Create 5 files tagged with tag 0*/
      tags.insert("myTag0");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      /*create 5 files tagged with 0,2,4*/
      tags.insert("myTag2");
      tags.insert("myTag4");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      /*Create 5 files tagged with 1,3*/
      tags.clear();
      tags.insert("myTag1");
      tags.insert("myTag3");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = fileName.substr(0, 6);
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
      tags.insert("myTag1");
      tags.insert("myTag3");
      tags.insert("myTag5");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      /*Try to tag the previous 5 files with tag 5*/
      cout << "Tag File Errors: should be 5 errors" << endl;
      tags.clear();
      tags.insert("myTag5");
      for(size_t i = 0; i < foundFiles->size(); i++)
      {
        try{fs1->tagFile(foundFiles->at(i), tags);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      }
      
      cout << endl;
      
      /*try to create another file with tags 1,3,5*/
      cout << "File Creation Errors: SHould be 2 errors" << endl;
      tags.clear(); 
      tags.insert("myTag1");
      tags.insert("myTag3");
      tags.insert("myTag5");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      /*Try to create another file with tags 0,2,4*/
      
      tags.clear(); 
      tags.insert("myTag0");
      tags.insert("myTag2");
      tags.insert("myTag4");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      cout << endl;
      
      /*Try to delete a tag that DNE*/
      cout << "Deleting a badtag : should be 1 error" << endl;
      try{fs1->deleteTag("badtag");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      /*Delete tag1 without force*/
      cout << "No force tag deletion failure : shoudl be 1 error" << endl;
      try{fs1->deleteTag("myTag1");}
      catch(arboreal_exception& e)
      {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      cout << endl;
      
      break;
    }
    case 6:
    {
      cout << "Testing tag creation/Association" << endl << endl; 
      string tagName = "Tag";
      string fileName = "File";
      unordered_set<string> tags;
      
      /*create 10 tags*/
      for(int i = 0; i < 10; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
        tagName = tagName.substr(0, 3);
      }
      
      try
      {
        tags.insert("Tag0"); tags.insert("Tag1"); tags.insert("Tag2");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.insert("Tag0"); tags.insert("Tag1");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.insert("Tag0"); tags.insert("Tag2");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.insert("Tag1");
        fs1->createFile("File2", tags); tags.clear();
        
        tags.insert("Tag1"); tags.insert("Tag5");
        fs1->createFile("File3", tags); tags.clear();
        
        tags.insert("Tag0"); tags.insert("Tag2"); tags.insert("Tag5");
        fs1->createFile("File4", tags); tags.clear();
        
        tags.insert("Tag0"); tags.insert("Tag2"); tags.insert("Tag4");
        fs1->createFile("File5", tags); tags.clear();
        
        
        fs1->writeChanges();
        fs1->printFiles();
        cout << endl <<  endl;
        
      }
      catch(arboreal_exception& e)
      {
        cerr << "Error! " << e.what() << " in " << e.where()<< endl;
      }

      break;
    }
    case 7:
    {
      cout << "Testing simple tag deletion and file deletion with path names and tagging and some untagging" << endl << endl; 
      
      try
      {
        FileInfo* file = 0;
        vector<string> fullPath;
        unordered_set<string> tags;
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag1"); fullPath.push_back("Tag2"); fullPath.push_back("File1");
        fs1->deleteFile(fullPath); fullPath.clear();
        fullPath.clear();
        
        
        fullPath.push_back("Tag1"); fullPath.push_back("Tag5"); fullPath.push_back("File3");
        fs1->deleteFile(fullPath); fullPath.clear();
        fullPath.clear();
        
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag2"); fullPath.push_back("Tag4"); fullPath.push_back("File5");
        fs1->deleteFile(fullPath); fullPath.clear();
        fullPath.clear();
        
        fs1->writeChanges();
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag2"); fullPath.push_back("File1");
        file = fs1->pathToFile(fullPath); 
        tags.insert("Tag0"); fs1->untagFile(file, tags); tags.clear();
        fs1->writeChanges();
        
        tags.insert("Tag5"); fs1->tagFile(file, tags); tags.clear();
        fs1->writeChanges();
        
        fullPath.clear();
        
        
        fullPath.push_back("Tag1"); fullPath.push_back("File2");
        file = fs1->pathToFile(fullPath); 
        tags.insert("Tag7"); tags.insert("Tag9"); fs1->tagFile(file, tags); tags.clear();
        fullPath.clear();
        
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag2"); fullPath.push_back("Tag5"); fullPath.push_back("File4");
        tags.insert("Tag6"); fs1->tagFile(fullPath, tags); tags.clear();
        fullPath.clear();
        
        fs1->deleteTag("Tag3");
        fs1->deleteTag("Tag4");
        fs1->deleteTag("Tag8");
        
        
        fs1->writeChanges();
        fs1->printFiles();
        cout << endl <<  endl;
        
      }
      catch(arboreal_exception& e)
      {
        cerr << "Error! " << e.what() << " in " << e.where()<< endl;
      }
      
      break;
    }
    case 8:
    {
      cout << "Testing file tagging and untagging conflicts and renaming tags. I've stopped printing errors for this one. Warnings still print" << endl << endl; 
      
      
      vector<string> fullPath;
      unordered_set<string> tags;
      
      try
      {
        tags.insert("Tag0"); fs1->createFile("File1", tags);
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fullPath.clear(); tags.clear();
      
//       cout << "TagFile error: should fail" << endl;
      try
      {
        fullPath.push_back("Tag0"); fullPath.push_back("File1"); 
        tags.insert("Tag1"); fs1->tagFile(fullPath, tags);
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      
      fullPath.clear(); tags.clear();
      
      try
      {
        tags.insert("Tag1"); tags.insert("Tag7"); fs1->createFile("File2", tags);
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fullPath.clear(); tags.clear();
      
      
//       cout << "UntagFile error:" << endl;
      try
      {
        fullPath.push_back("Tag1"); fullPath.push_back("Tag7"); fullPath.push_back("Tag9"); fullPath.push_back("File2");
        tags.insert("Tag9"); fs1->untagFile(fullPath, tags); 
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      
      fullPath.clear(); tags.clear();
      
      cout << "UntagFile Warnings:" << endl;
      try
      {
        fullPath.push_back("Tag1"); fullPath.push_back("Tag7"); fullPath.push_back("Tag9"); fullPath.push_back("File2");
        tags.insert("Tag0"); tags.insert("Tag3"); tags.insert("Tag8"); tags.insert("Tag6"); 
        fs1->untagFile(fullPath, tags);
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      
      fullPath.clear(); tags.clear();
      
      cout << "UntagFile: Warnings" << endl;
      try
      {
        fullPath.push_back("Tag1"); fullPath.push_back("Tag7"); fullPath.push_back("Tag9"); fullPath.push_back("File2");
        tags.insert("Tag0"); tags.insert("Tag8"); tags.insert("Tag1");
        fs1->untagFile(fullPath, tags);
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fullPath.clear(); tags.clear();
      
      cout << "TagFile: Warnings" << endl;
      try
      {
        fullPath.push_back("Tag0"); fullPath.push_back("Tag1"); fullPath.push_back("File1");
        tags.insert("Tag0"); tags.insert("Tag3"); tags.insert("Tag6");
        fs1->tagFile(fullPath, tags);
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fullPath.clear(); tags.clear();
      
      try
      {
        fs1->renameTag("Tag1", "Tag8");
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fullPath.clear(); tags.clear();
      
//       cout << "CreateTag Error: " << endl;
      try
      {
        fs1->createTag("Tag8");
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      
      fullPath.clear(); tags.clear();
      
//       cout << "DeleteTag: Error" << endl;
      try
      {
        fs1->deleteTag("Tag1");
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      
      fullPath.clear(); tags.clear();
      
//       cout << "DeleteFile Error:" << endl;
      try
      {
        fullPath.push_back("NotATaG"); fullPath.push_back("File1");
        fs1->deleteFile(fullPath);
      }
      catch(arboreal_exception& e){/*cerr << "Error! " << e.what() << " in " << e.where()<< endl;*/}
      fullPath.clear(); tags.clear();
      
      fs1->writeChanges();
      
      break;
    }
    case 9:
    {
      cout << "Testing file opening and closing and associated errors" << endl << endl; 
      
      
      vector<string> fullPath;
      unordered_set<string> tags;
      
      cout << "This first part should have no errors" << endl << endl;
      fullPath.push_back("Tag2"); fullPath.push_back("Tag5"); fullPath.push_back("File1");
      try
      {
        int fd1 = fs1->openFile(fullPath, 'r');
        fullPath.clear();
        
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag6"); fullPath.push_back("Tag8"); fullPath.push_back("File1"); 
        int fd2 = fs1->openFile(fullPath, 'w'); 
        
        int fd3 = fs1->openFile(fullPath, 'r'); fullPath.clear();
        
        fs1->closeFile(fd1);
        fs1->closeFile(fd2);
        fs1->closeFile(fd3);
        
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      fs1->writeChanges();
      
      cout << "Here is where errors/warning should begin:" << endl << endl;
      
      cout << "Attempting to close Invalid fd:" << endl; 
      
      try{fs1->closeFile(-1);}
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      cout << endl << "Attempting to close same fd twice:" << endl;
      
      fullPath.push_back("Tag0"); fullPath.push_back("Tag6"); fullPath.push_back("Tag8"); fullPath.push_back("File1"); 
      int fd = fs1->openFile(fullPath, 'x'); fullPath.clear();
      try{fs1->closeFile(fd); fs1->closeFile(fd);}
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      
      break;
    }
    case 10:
    {
      string delimiter(80, '-'); 
      
      cout << "Testing file read and write" << endl << delimiter << endl << endl; 
      
      
      vector<string> fullPath;
      unordered_set<string> tags;
      
      try
      {
        fullPath.push_back("Tag7"); fullPath.push_back("Tag9"); fullPath.push_back("File2");
        int fd1 = fs1->openFile(fullPath, 'w');
        int fd2 = fs1->openFile(fullPath, 'r'); fullPath.clear();
        
        fullPath.push_back("Tag0"); fullPath.push_back("Tag2"); fullPath.push_back("Tag5"); fullPath.push_back("Tag6"); 
        fullPath.push_back("File4"); 
        int fd3 = fs1->openFile(fullPath, 'x'); 
        fullPath.clear();
        
        int bufferSize = 4096;
        char* buff = new char[bufferSize];
        int bytes = dm->getBlockSize() * 3;
        char symbol = '@';
        memset(buff, symbol, bytes);
        
        
        size_t ret = fs1->writeFile(fd1, buff, bytes);
        cout << "Bytes written return val: " << ret << " " << symbol << " symbols To FD 1" << endl;
        
        cout << "Resetting Buffer: " << endl;
        memset(buff, 0, bufferSize);
        ret = fs1->readFile(fd2, buff, bytes);
        
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols from FD 2" << endl;
        int count = 0;
        
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        /*Lets try writing in not multiples of 512.and non consecutive writes*/
        cout << "Attempting 666 byte write/read on FD3" << endl << endl;
        
        bytes = 666;
        memset(buff, 0, bufferSize);
        symbol = '#';
        memset(buff, symbol, bytes);
        ret = fs1->writeFile(fd3, buff, bytes);
        cout << "Bytes written: " << ret << " " << symbol << " symbols To FD 3" << endl;
        
        cout << "Resetting Buffer: " << endl;
        memset(buff, 0, bufferSize);
        cout << "Resetting fd: " << endl;
        fs1->seekFileAbsolute(fd3, 0);
        ret = fs1->readFile(fd3, buff, bytes);
        
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols from FD 3" << endl;
        count = 0;
        
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        /*Jump back to byte 600 and write $*/
        cout << "Attempting seek to byte 600 on FD3 and writing 100 $\'s" << endl << endl;
        
        fs1->seekFileAbsolute(fd3, 599);
        
        bytes = 100;
        memset(buff, 0, bufferSize);
        symbol = '$';
        memset(buff, symbol, bytes);
        ret = fs1->writeFile(fd3, buff, bytes);
        cout << "Bytes written: " << ret << " " << symbol << " symbols To FD 3" << endl;
        
        cout << "Resetting Buffer: " << endl;
        cout << "Moving fd back 50 spots" << endl;
        memset(buff, 0, bufferSize);
        fs1->seekFileRelative(fd3, -50);
        
        cout << "Attempt to read 100 bytes even though there are only " << 50 << " bytes left in the file" << endl;
        ret = fs1->readFile(fd3, buff, bytes);
        
        
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols from FD 3" << endl;
        count = 0;
        
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        cout << "Appending a single !" << endl << endl;
        /*Now append a single ! to check on the absolute seek*/
        symbol = '!';
        bytes = 1;
        memset(buff, 0, bufferSize);
        memset(buff, symbol, bytes);
        ret = fs1->appendFile(fd3, buff, bytes);
        cout << "Bytes written: " << ret << " " << symbol << " symbols To FD 3" << endl;
        
        
        cout << "Resetting Buffer: " << endl;
        memset(buff, 0, bufferSize);
        cout << "Moving the fd to the ! with seekFileAbsolute" << endl;
        fs1->seekFileAbsolute(fd3, 700); //should point directly at the !
        ret = fs1->readFile(fd3, buff, bytes);
        
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols from FD 3" << endl;
        count = 0;
        
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        
        /*Now append 8000 %'s. will go into first level indirect*/
        cout << "Append 8000 % signs. should go into first level indirect" << endl;
        bufferSize*= 2;
        bytes = 8000;
        symbol = '%';
        delete buff; buff = 0;
        buff = new char[bufferSize];
        memset(buff, 0, bufferSize);
        memset(buff, symbol, bytes);
        ret = fs1->appendFile(fd3, buff, bytes);
        cout << "Bytes written: " << ret << " " << symbol << " symbols To FD 3" << endl;

        cout << "Resetting Buffer: " << endl;
        memset(buff, 0, bufferSize);
        cout << "moving the fd back 8000 spots" << endl;
        fs1->seekFileRelative(fd3, -8000);
        ret = fs1->readFile(fd3, buff, bytes);
        
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols from FD 3" << endl;
        count = 0;
        
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        fs1->closeFile(fd1);
        fs1->closeFile(fd2);
        fs1->closeFile(fd3);
        
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      
      break;
    }
    case 11:
    {
      try
      {
        
        cout << "Start testing indirectness " << endl;
        
        vector<string> fullPath;
        string delimiter(80, '-'); 
        
        fullPath.push_back("Tag0"); fullPath.push_back("File1");
        int fd1 = fs1->openFile(fullPath, 'x');      
        int ret;
        int bufferSize = 8192;
        char* buff = new char[bufferSize];
        int bytes = dm->getBlockSize() * 12;
        char symbol = '&';
        memset(buff, symbol, bytes);
        
        cout << "Writing 12 full blocks" << endl;
        ret = fs1->writeFile(fd1, buff, bytes);
        cout << "Bytes written: " << ret << " " << symbol << " symbols " << endl;
        
        cout << "Resetting fd with seekFileAbsolute" << endl;
        fs1->seekFileAbsolute(fd1, 0); 
        memset(buff, 0, bufferSize);
        
        ret = fs1->readFile(fd1, buff, bytes);
        cout << "Bytes Read return val: " << ret << " " << symbol << " symbols" << endl;
        
        int count = 0;
        for(size_t i = 0; i < ret; i++)
        {
          if (buff[i] == symbol) count++;
        }
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        
        /*Start writing to indirect*/
        cout << "Beginning indirect writing" << endl;
        cout << "Fill up the 1st level indirect block, write 64 blocks" << endl;
        symbol = '^';
        bufferSize = 512;
        bytes = 512;
        delete buff;
        buff = new char[bufferSize];
        memset(buff, 0, bufferSize);
        memset(buff, symbol, bytes);
        
        int pret = 0;
        for(int i = 0; i < 64; i++)
        {
           ret = fs1->writeFile(fd1, buff, bytes);
           pret+=ret;
        }
        
        cout << "Bytes written: " << pret << " " << symbol << " symbols " << endl;
        
        cout << "turning back fd with seekFileRelative" << endl;
        fs1->seekFileRelative(fd1, -(bytes * 64)); 
        memset(buff, 0, bufferSize);
        
        pret = 0;
        count = 0;
        
        for(int i = 0; i < 64; i++)
        {
          memset(buff, 0, bufferSize);
          ret = fs1->readFile(fd1, buff, bytes);
          pret += ret;
          
          for(size_t i = 0; i < ret; i++)
          {
            if (buff[i] == symbol) count++;
          }
        }
        cout << "Bytes Read return val: " << pret << " " << symbol << " symbols" << endl;
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
        
        cout << "Start Writing to 2nd level indirect block" << endl;
        
        symbol = '$';
        bufferSize = 512;
        bytes = 512;
        delete buff;
        buff = new char[bufferSize];
        memset(buff, 0, bufferSize);
        memset(buff, symbol, bytes);
        
        pret = 0;
        for(int i = 0; i < 100; i++)
        {
          ret = fs1->writeFile(fd1, buff, bytes);
          pret+=ret;
        }
        
        cout << "Bytes written: " << pret << " " << symbol << " symbols " << endl;
        
        cout << "turning back fd with seekFileRelative" << endl;
        fs1->seekFileRelative(fd1, -(bytes * 100)); 
        memset(buff, 0, bufferSize);
        
        pret = 0;
        count = 0;
        
        for(int i = 0; i < 100; i++)
        {
          memset(buff, 0, bufferSize);
          ret = fs1->readFile(fd1, buff, bytes);
          pret += ret;
          
          for(size_t i = 0; i < ret; i++)
          {
            if (buff[i] == symbol) count++;
          }
        }
        cout << "Bytes Read return val: " << pret << " " << symbol << " symbols" << endl;
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      break;
    }
    case 12:
    {
      try
      {
        cout << "Let's fill up the disk by writing to a file!!" << endl;
        vector<string> fullPath;
        string delimiter(80, '-'); 
        
        fullPath.push_back("Tag0"); fullPath.push_back("File1");
        int fd1 = fs1->openFile(fullPath, 'x');      
        int ret;
        int bufferSize = 512;
        char* buff = new char[bufferSize];
        int bytes = 512;
        char symbol = '@';
        memset(buff, symbol, bytes);
        
        
        int pret = 0;
        for(int i = 0; i < 500; i++)
        {
          ret = fs1->writeFile(fd1, buff, bytes);
          pret+=ret;
        }
        
        cout << "Bytes written: " << pret << " " << symbol << " symbols " << endl;
        
        cout << "turning back fd with seekFileRelative" << endl;
        fs1->seekFileRelative(fd1, -(pret)); 
        memset(buff, 0, bufferSize);
        
        pret = 0;
        int count = 0;
        for(int i = 0; i < 500; i++)
        {
          memset(buff, 0, bufferSize);
          ret = fs1->readFile(fd1, buff, bytes);
          pret += ret;
          
          
          for(size_t i = 0; i < ret; i++)
          {
            if (buff[i] == symbol) count++;
          }
        }
        cout << "Bytes Read return val: " << pret << " " << symbol << " symbols" << endl;
        
        cout << "\t" << count << " " << symbol << " symbols in buffer" << endl << delimiter << endl << endl;
        
      }
      catch(arboreal_exception& e){cerr << "Error! " << e.what() << " in " << e.where()<< endl;}
      
      break;
    }
    default:
    {
      cerr << "Driver Error! Behavior not defined for specified number" << endl;
    }
  }
  return 0;
  
}




