#ifndef FILE_H
#define FILE_H
#include"types.h"
#include<vector>
using std::vector;
#include<string>
using std::string;

class File
{
private:
  string _name;
  FileAttributes _attributes;
  vector<string> _tags;

  
public:
  File(string name, const vector<string>& tags, FileAttributes attributes);
  
  /**********************************************************************************************************************/
  /** @name Accessor Functions 
   */
  ///@{  
  
  string get_name();
  vector<string>& get_tags();
  FileAttributes get_attributes();
  
  ///@}
  
  /**********************************************************************************************************************/
  /** @name Static Functions 
   */
  ///@{   
  
  static File* read_buff(char* serializedFile);
  
  ///@}
};

#endif