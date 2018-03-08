///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File.h
//  File Header File
//  Primary Author: Wyatt Emery
//  For "Arboreal" Senior Design Project
//
//  Spring 2018
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_H
#define FILE_H
#include "types.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class File
{
private:
  string _name;
  FileAttributes _attributes;
  vector<string> _tags;

  
public:
  /*!
   * @param name the name of the File
   * @param tags the tags to be associated with the File
   * @param attributes the File attributes
   */
  File(string name, const vector<string>& tags, FileAttributes attributes);
  
  /**********************************************************************************************************************/
  /** @name Accessor Functions 
   */
  ///@{  
  
  /*!
   * @returns The name of the File
   */
  string get_name();
  
  /*!
   * @returns The tags associated with this File
   */
  vector<string>& get_tags();
  
  /*!
   * @returns the attributes associated with this File
   */
  FileAttributes get_attributes();
  
  ///@}
  
  /**********************************************************************************************************************/
  /** @name Static Functions 
   */
  ///@{   
  
  /*!
   * Will take a char* buffer and create a File object from it. The buffer must have been serialized in the correct format
   * @param serializedFile the serializedFile object
   * @returns a File* to the created File
   * @sa FileInfo::serialize()
   */
  static File* read_buff(char* serializedFile);
  
  ///@}
};

#endif