/*Driver.cpp
 * Arboreal
 * October, 12, 2017
 */
 #include <chrono>
 #include <ctime>
 #include<fstream>
 #include<string>
 #include <stdlib.h>
 #include<vector>
#include <random>
#include <math.h>       /* round, floor, ceil, trunc */

#include "DaemonDependancies/FileSystem/FileSystem.h"

#define CREATEFILEDATA "Data/create_file_time.txt"
#define CREATETAGDATA "Data/create_tag_time.txt"
#define TAGSEARCHDATA "Data/tag_search_time.txt"
#define FILESEARCHDATA "Data/file_search_time.txt"
#define TAGFILEDATA "Data/tag_file_time.txt"
#define RENAMETAGDATA "Data/rename_tag_time.txt"
#define STARTTUPDATA "Data/startup_time.txt"

using namespace std;


int main(int argc, char** argv)
{
/*
  std::ofstream outfile;
  outfile.open(TAGSEARCHDATA, std::ofstream::out | std::ofstream::app);
  auto t_start = std::chrono::high_resolution_clock::now();

  auto t_end = std::chrono::high_resolution_clock::now();
  outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
          << fd_fs_map[fd]->num_of_files() << " " << " " << fd_fs_map[fd]->num_of_tags()
          << " " << rval->size() << endl;
  outfile.close();
  */

  if(argc != 2)
  {
    cerr << "timer usage:\n test number\n where number is the test version we want to run." << endl;
    exit(1);
  }

  int driverNumber = atoi(argv[1]);

  Disk* d = 0;
  DiskManager* dm = 0;
  FileSystem* fs1 = 0;

  try
  {
    d = new Disk(300000, 4096, const_cast<char *>("DISK1"));
    dm = new DiskManager(d);

    std::ofstream outfile;
    outfile.open(STARTTUPDATA, std::ofstream::out | std::ofstream::app);
    auto t_start = std::chrono::high_resolution_clock::now();
    fs1 = new FileSystem(dm, "PartitionA");
    auto t_end = std::chrono::high_resolution_clock::now();
    outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
            << fs1->num_of_files() << " " << fs1->num_of_tags()
            << " " << endl;
    outfile.close();

  }
  catch(arboreal_exception& e)
  {
    cerr << "Error! " << e.what() << " in " << e.where()<< endl;
    exit(1);
  }

  switch(driverNumber)
  {
    case(0):
    {
      srand (time(NULL));

      std::default_random_engine generator;
      std::normal_distribution<double> distribution(8.0,3.0);

      cout << "doing a startup timing test" << endl;
      size_t numTags = 125;
      size_t numFiles = 1000;
      string tagName = "tag_";
      unordered_set<string> tagSet;
      string fileName = "file_";
      int file_number = 0;

      /*Create Tags*/
      for(size_t i = 0; i < numTags; ++i){
        string tempTag = tagName; tempTag.append(to_string(i));
        fs1->create_tag(tempTag);
      }
      fs1->write_changes();

      for(int i = 0; i < numFiles; ++i){

         int tags_in_file = (int) round(distribution(generator));
         // if(tags_in_file < 0) tags_in_file = 0;
        for(int k = 0; k < tags_in_file; ++k){
          tagSet.insert(tagName + to_string(rand() % numTags));
        }

        try{
          string tempFileName = fileName; tempFileName.append(to_string(file_number));
          fs1->create_file(tempFileName, tagSet);
            // cout << "Creation of file " << tempFileName << " Sucessful" <<endl;
          }
        catch(arboreal_exception&e){cerr << "There was an error!" << e.what() << e.where() <<endl ;
          cerr << fileName << " The file number is: " << file_number << endl;
        }
        fs1->write_changes();

        if(rand() % 100 == 0){
          cout << "REstarting Filesystem" << endl;
          delete fs1; cout << "Filesystem Deleted"<< endl;
          std::ofstream outfile;
          outfile.open(STARTTUPDATA, std::ofstream::out | std::ofstream::app);
          auto t_start = std::chrono::high_resolution_clock::now();
          fs1 = new FileSystem(dm, "PartitionA");
          auto t_end = std::chrono::high_resolution_clock::now();
          outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                  << fs1->num_of_files() << " " << fs1->num_of_tags()
                  << " " << endl;
          outfile.close();
          cout <<"Filesystem restarted" << endl;

          int tags_to_add = rand() % 5;
          for(int k = 0; k < tags_to_add; ++k)
          {
            string tempTag = tagName; tempTag.append(to_string(numTags));
            fs1->create_tag(tempTag);
            ++numTags;
          }
        }

        ++file_number;
      }



      break;
    }
    case (1):
    {
      srand (time(NULL));

      vector<string> duplicate_files;
      int file_dup_count = 0;
      int file_dup_num = 0;
      bool duplicating = false;
      size_t numFiles = 100000;
      size_t numTags = 500;
      string fileName = "file_";
      string tagName = "tag_";
      std::default_random_engine generator;
      std::normal_distribution<double> distribution(8.0,5.0);

      /*Create Tags*/
      for(size_t i = 0; i < numTags; ++i){
        string tempTag = tagName; tempTag.append(to_string(i));
        fs1->create_tag(tempTag);
      }

      fs1->write_changes();
      /*Create 100000 Files*/
      int file_number = 0;
      for(size_t i = 0; i < numFiles; ++i){

        if((i % 5000 == 0) && (i != 0)){cout << i << " files down" << endl;}
        unordered_set<string> tagSet;

        /*Insert and random number of random tags to this tagset*/
        int tags_in_file = (int) round(distribution(generator));
        // if(tags_in_file < 0) tags_in_file = 0;
        for(int k = 0; k < tags_in_file; ++k){
          tagName.append(to_string(rand() % numTags));
          tagSet.insert(tagName);
          tagName = "tag_";
        }

        if(rand() % 1000 == 0){
          cout << "doing a timing operation set" << endl;
          /*Do the timing operations*/
          std::ofstream outfile;
          vector<FileInfo*>* rval2 = 0;
          try{
            outfile.open(CREATEFILEDATA, std::ofstream::out | std::ofstream::app);
            string file_to_create = fileName; file_to_create.append(to_string(file_number));

            auto t_start = std::chrono::high_resolution_clock::now();
            fs1->create_file(file_to_create, tagSet);
            auto t_end = std::chrono::high_resolution_clock::now();

            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fs1->num_of_files() << " " << fs1->num_of_tags() << " " << tagSet.size()
                    << " " << endl;
            outfile.close();
            ++file_number;
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          try{
            outfile.open(CREATETAGDATA, std::ofstream::out | std::ofstream::app);
            string tag_to_create = tagName; tag_to_create.append(to_string(numTags));

            auto t_start = std::chrono::high_resolution_clock::now();
            fs1->create_tag(tag_to_create);
            auto t_end = std::chrono::high_resolution_clock::now();

            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fs1->num_of_files() << " " << fs1->num_of_tags()
                    << " " << endl;
            outfile.close();
            ++numTags;
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          try{
            outfile.open(TAGSEARCHDATA, std::ofstream::out | std::ofstream::app);
            int distribution_num = (int) round(distribution(generator));
            if(distribution_num < 1) distribution_num = 1;
            int number_tags_to_search_for = rand() % distribution_num;
            unordered_set<string> tags_to_search_for;
            for(int k = 0; k < number_tags_to_search_for; ++k){
              string tempTag = tagName; tempTag.append(to_string(rand() % numTags));
              tags_to_search_for.insert(tempTag);
            }

            auto t_start = std::chrono::high_resolution_clock::now();
            vector<FileInfo*>* rval = fs1->tag_search(tags_to_search_for);
            auto t_end = std::chrono::high_resolution_clock::now();

            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fs1->num_of_files() << " " << fs1->num_of_tags() << " " << rval->size() << " "
                    << tags_to_search_for.size() << endl;
            outfile.close();
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          try{
            outfile.open(FILESEARCHDATA, std::ofstream::out | std::ofstream::app);
            string file_to_search_for;
            if((rand() % 2 == 0) && (duplicate_files.size() > 0)){
              cout << "choosign from duplicate_files" << endl;
              file_to_search_for = duplicate_files.at(0);
            }
            else{
              file_to_search_for = fileName; file_to_search_for.append(to_string(rand() % file_number));
            }
            auto t_start = std::chrono::high_resolution_clock::now();
            rval2 = fs1->file_search(file_to_search_for);
            auto t_end = std::chrono::high_resolution_clock::now();

            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fs1->num_of_files() << " " << fs1->num_of_tags() << " " << rval2->size() << endl;
            outfile.close();
            if(duplicate_files.size() > 0) duplicate_files.erase(duplicate_files.begin());
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          try{
            outfile.open(TAGFILEDATA, std::ofstream::out | std::ofstream::app);
            int number_tags_tag_file_with = (int) round(distribution(generator));
            // if (number_tags_tag_file_with < 0) number_tags_tag_file_with = 0;
            unordered_set<string> tags_tag_file_with;
            for(int k = 0; k < number_tags_tag_file_with; ++k){
              string tempTag = tagName; tempTag.append(to_string(rand() % numTags));
              tags_tag_file_with.insert(tempTag);
            }

            auto t_start = std::chrono::high_resolution_clock::now();
            fs1->tag_file(rval2->at(0), tags_tag_file_with);
            auto t_end = std::chrono::high_resolution_clock::now();

            outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                    << fs1->num_of_files() << " " << fs1->num_of_tags() << " " << tags_tag_file_with.size() << endl;
            outfile.close();
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          try{
            // if(rand() % 8 == 0){
              outfile.open(RENAMETAGDATA, std::ofstream::out | std::ofstream::app);
              /*Pick a tag to rename*/
              string newTagName = "myNewTAgName";
              string oldTagName = tagName + to_string(rand() % numTags);
              unordered_set<string> tag_to_find_size_of; tag_to_find_size_of.insert(oldTagName);
              vector<FileInfo*>* tagSize = fs1->tag_search(tag_to_find_size_of);
              auto t_start = std::chrono::high_resolution_clock::now();
              fs1->rename_tag(oldTagName, newTagName);
              auto t_end = std::chrono::high_resolution_clock::now();

              outfile << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " "
                      << fs1->num_of_files() << " " << fs1->num_of_tags() << " " << tagSize->size()
                      << " " << endl;
              outfile.close();
              /*Name it back*/
              fs1->rename_tag(newTagName, oldTagName);
            // }
          }
          catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

          cout << "Timing Done" <<endl;
          fs1->write_changes();
        }
        else{
          try{
            string tempFileName = fileName; tempFileName.append(to_string(file_number));
            fs1->create_file(tempFileName, tagSet);
              // cout << "Creation of file " << tempFileName << " Sucessful" <<endl;
            }
          catch(arboreal_exception&e){cerr << "There was an error!" << e.what() << e.where() <<endl ;
            cerr << fileName << " The file number is: " << file_number << endl;
          }
        }

        try{
          if(rand() % 200 == 0){
            string tempTag = tagName; tempTag.append(to_string(numTags));
            fs1->create_tag(tempTag);
            ++numTags;
          }
        }
        catch(arboreal_exception&e){cerr <<" Error Occured: " << e.what() << " " << e.where();}

        if(duplicating){
          ++file_dup_num;
          if(file_dup_count <= file_dup_num ){
            duplicating = false;
            file_dup_count = 0;
          } //get some duplicate file names
        }
        else{
          if(rand() % 10 != 0 ) ++file_number;
        }

        if(((rand() % 1000) == 0) && !duplicating) {
          //duplicate filname x times
          duplicating = true;
          file_dup_num = 0;
          duplicate_files.push_back(fileName + to_string(file_number));
          file_dup_count = (int) round(((i/450) + 1));
        }

      }
      break;
    }
    default:
    {
      cerr << "unrecognized input command number" << endl;
    }
  }

  fs1->write_changes();

  if(d != 0) delete d;
  if(dm != 0) delete dm;
  if(fs1 != 0) delete fs1;
  return 0;
}