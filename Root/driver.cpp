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

int main(int argc, char** argv)
{
  
  //   cout << dm->getPartitionSize("PartitionA") << std::endl;
  
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
    std::cerr << "driver usage:\n driver number\n where number is the driver version we want to run." << std::endl;
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
    std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;
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
        cout << "Root Tree and Tag Tree's Contents:" << std::endl;
        cout << "Partition A" << std::endl;
        //       fs1->printRoot();
//         fs1->printTags();
        fs1->printFiles();
        cout << std::endl << std::endl;
        
//         cout << "Partition B" << std::endl;
        //       fs2->printRoot();
//         fs2->printTags();
//         cout << std::endl << std::endl;
        
//         cout << "Partition C" << std::endl;
        //       fs3->printRoot();
//         fs3->printTags();
//         cout << std::endl << std::endl;
      }
      catch(arboreal_exception& e)
      {
        std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;
      }

      break;
    }
    case 1:
    {
      try 
      {
        cout << "Creating 3 Tags, Creating 9 files where filenames are reused but with different \"paths\"" << std::endl;
        fs1->createTag("tag1");
        fs1->createTag("tag2");
        fs1->createTag("tag3");
        
//         fs1->printRoot();
        fs1->writeChanges();
        
        vector<std::string> tagSet;
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        
        tagSet.push_back("tag1");
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        
        tagSet.push_back("tag2");
        fs1->createFile("myfile1", tagSet);
        fs1->createFile("myfile2", tagSet);
        fs1->createFile("myfile3", tagSet);
        fs1->writeChanges();
        
//         fs1->deleteTag("tag1");
        
//         fs1->writeChanges();
      }
      catch(arboreal_exception& e)
      {
        std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;
      }
      break;
    }
    case 2:
    {
      try 
      {
        vector<std::string> tagSet;
        fs1->createFile("myfile1", tagSet);
        tagSet.push_back("tag2");
        fs1->createFile("myfile2", tagSet);
        tagSet.push_back("tag3");
        fs1->createFile("myfile3", tagSet);
        
        fs1->writeChanges();
      }
      catch(arboreal_exception& e)
      {
        std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;
      }
      break;
    }
    case 3:
    {
      vector<std::string> tagSet;
      try{fs1->createFile("myfile1", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {std::cerr << "Error! " << e.what() << " in " << e.where() << std::endl;}
      tagSet.push_back("tag2");
      try{fs1->createFile("myfile2", tagSet);}//This should fail, creating duplicate file
      catch(arboreal_exception& e) {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      tagSet.push_back("badtag");
      try{fs1->createFile("myfile3", tagSet);}//
      catch(arboreal_exception& e) {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      break;
    }
    case 4:
    {
      vector<FileInfo*>* foundFiles;
      cout << "Searching for myfile1:" << std::endl;
      try{foundFiles = fs1->fileSearch("myfile1");}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      cout << "found " << foundFiles->size() << " files" << std::endl;
      delete foundFiles; foundFiles = 0;
      
      
      cout << "Searching for myfile2:" << std::endl;
      try{foundFiles = fs1->fileSearch("myfile2");}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      cout << "found " << foundFiles->size() << " files" << std::endl;
      delete foundFiles; foundFiles = 0;
      
      
      
      break;
    }
    case 5:
    {
      std::string fileName = "myFile";
      vector<std::string> tags;
      /*Create 15 files tagged with default*/
      for(int i = 0; i < 15; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      
      /*Create 15 tags*/
      std::string tagName = "myTag";
      for(int i = 0; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        tagName = tagName.substr(0, 5);
      }
      
      fs1->writeChanges();
      
      /*Delete the last 5 tags*/
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        tagName = tagName.substr(0, 5);
      }

      fs1->writeChanges();
      
      /*Create tag 10 again*/
      try{fs1->createTag("myTag10");}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      
      fs1->writeChanges();
      
      cout << "Tag Creation Errors: should be 5 errors" << std::endl;
      for(int i = 0; i < 5; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << std::endl;
      
      cout << "Tag Deletion Errors: should be 4 errors" << std::endl;
      for(int i = 10; i < 15; i++)
      {
        tagName.append(to_string(i));
        try{fs1->deleteTag(tagName);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        tagName = tagName.substr(0, 5);
      }
      cout << std::endl;
      

      
      /*Create 5 files tagged with tag 0*/
      tags.push_back("myTag0");
      
      for(int i = 0; i < 5; i++)
      {
        fileName.append(to_string(i));
        try{fs1->createFile(fileName, tags);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        fileName = fileName.substr(0, 6);
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
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        fileName = fileName.substr(0, 6);
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
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      /*Find those 5 files*/
      vector<FileInfo*>* foundFiles;
      try{foundFiles = fs1->tagSearch(tags);}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      if(foundFiles->size() != 5)
      {
        std::cerr << "TagSearch error. ONly " << foundFiles->size() << " files found" << std::endl;
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
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        fileName = fileName.substr(0, 6);
      }
      
      fs1->writeChanges();
      
      /*Try to tag the previous 5 files with tag 5*/
      cout << "Tag File Errors: should be 5 errors" << std::endl;
      tags.clear();
      tags.push_back("myTag5");
      for(size_t i = 0; i < foundFiles->size(); i++)
      {
        try{fs1->tagFile(foundFiles->at(i), tags);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      }
      
      cout << std::endl;
      
      /*try to create another file with tags 1,3,5*/
      cout << "File Creation Errors: SHould be 2 errors" << std::endl;
      tags.clear(); 
      tags.push_back("myTag1");
      tags.push_back("myTag3");
      tags.push_back("myTag5");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      
      /*Try to create another file with tags 0,2,4*/
      
      tags.clear(); 
      tags.push_back("myTag0");
      tags.push_back("myTag2");
      tags.push_back("myTag4");
      
      try{fs1->createFile("myFile0", tags);}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      
      cout << std::endl;
      
      /*Try to delete a tag that DNE*/
      cout << "Deleting a badtag : should be 1 error" << std::endl;
      try{fs1->deleteTag("badtag");}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      
      /*Delete tag1 without force*/
      cout << "No force tag deletion failure : shoudl be 1 error" << std::endl;
      try{fs1->deleteTag("myTag1");}
      catch(arboreal_exception& e)
      {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
      cout << std::endl;
      
      break;
    }
    case 6:
    {
      cout << "Testing tag deletion force" << std::endl << std::endl; 
      std::string tagName = "Tag";
      std::string fileName = "File";
      vector<std::string> tags;
      
      /*create 10 tags*/
      for(int i = 0; i < 10; i++)
      {
        tagName.append(to_string(i));
        try{fs1->createTag(tagName);}
        catch(arboreal_exception& e)
        {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
        tagName = tagName.substr(0, 3);
      }
      
      try
      {
        tags.push_back("Tag0"); tags.push_back("Tag1"); tags.push_back("Tag2");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.push_back("Tag0"); tags.push_back("Tag1");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.push_back("Tag0"); tags.push_back("Tag2");
        fs1->createFile("File1", tags); tags.clear();
        
        tags.push_back("Tag1");
        fs1->createFile("File2", tags); tags.clear();
        
        tags.push_back("Tag1"); tags.push_back("Tag5");
        fs1->createFile("File3", tags); tags.clear();
        
        tags.push_back("Tag0"); tags.push_back("Tag2"); tags.push_back("Tag5");
        fs1->createFile("File4", tags); tags.clear();
        
        tags.push_back("Tag0"); tags.push_back("Tag2"); tags.push_back("Tag4");
        fs1->createFile("File5", tags); tags.clear();
        
        
        /*Create a bunch of files with just Tag1 in order to test the deleteContBlocks function*/
//         tags.push_back("Tag1");
//         for(int i = 10; i < 35; i++)
//         {
//           fileName.append(to_string(i));
//           try{fs1->createFile(fileName, tags);}
//           catch(arboreal_exception& e)
//           {std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;}
//           fileName = fileName.substr(0, 4);
//         }
        
        fs1->writeChanges();
        fs1->printFiles();
        cout << std::endl <<  std::endl;
        fs1->deleteTag("Tag1");
        
        fs1->writeChanges();
        fs1->printFiles();
        
      }
      catch(arboreal_exception& e)
      {
        std::cerr << "Error! " << e.what() << " in " << e.where()<< std::endl;
      }

      break;
    }
    default:
    {
      std::cerr << "Driver Error! Behavior not defined for specified number" << std::endl;
    }
  }
  return 0;
  
}




