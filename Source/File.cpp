#include "File.h"
#include<string>
#include<string.h>
using std::string;
#include <vector>
using std::vector;

File::File(string name, const vector<string>& tags, FileAttributes attributes): _name(name), _attributes(attributes)
{
  for(size_t i = 0; i < tags.size(); ++i)
  {
    _tags.push_back(tags.at(i));
  }
}


string File::get_name(){return _name;}
vector<string>& File::get_tags(){return _tags;}
FileAttributes File::get_attributes(){return _attributes;}



File* File::read_buff(char* serializedFile)
{
  size_t offset = 0;
  
  size_t filenameSize = 0;
  memcpy(&filenameSize, serializedFile, sizeof(size_t));
  offset += sizeof(size_t);
  
  string filename;
  filename.assign(serializedFile + offset, filenameSize);
  offset += filenameSize;
  
  size_t numTags;
  memcpy(&numTags, serializedFile + offset, sizeof(size_t));
  offset += sizeof(size_t);

  vector<string> tags;
  for(size_t i = 0; i < numTags; ++i)
  {
    string tag;
    tag.assign(serializedFile + offset);
    offset += tag.size() + 1;
    tags.push_back(tag);
  }
  
  FileAttributes attributes;
  memcpy(&attributes, serializedFile + offset, sizeof(FileAttributes));
  
  File* ret = new File(filename, tags, attributes);
}
  