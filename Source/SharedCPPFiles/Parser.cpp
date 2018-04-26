
#include "../SharedHeaders/Parser.h"



//=================================================================================================
std::vector<std::string> Parser::parse(int type)
{
  std::vector<std::string> parsed;
  parsed.push_back(std::to_string(type));


  if(type == FIND_TS || type == FIND_FS || type == NEW_TS || type == NEW_FS
      || type == DEL_TS)
  {
    int index = 0;
    while(_string[index] != '[' && _string[index] != '{'){index += 1;}
    _string = _string.substr(index,_string.length());
    index = 0;


    if(_string[index] == '[')
    {
      std::vector<std::string> temp = lunion(_string.substr(1,_string.length()));
      for(uint i = 0; i < temp.size(); i++)
      {
        parsed.push_back(temp[i]);
      }
      if(type == NEW_FS)
      {
        std::vector<std::string> v = get_cwd_tags();
        for(uint i = 1; i < parsed.size(); i++)
        {
          for(uint j = 0; j < v.size(); j++)
          {
            parsed[i] += ("-" + v[j]);
          }
        }
      }
    }
    else if(_string[index] == '{')
    {
      std::vector<std::string> temp = lintersect(_string.substr(1,_string.length()));
      for(uint i = 0; i < temp.size(); i++)
      {
        parsed.push_back(temp[i]);
      }
    }
    return parsed;
  }






  switch(type)
  {
    case(NEW_FP):
    {
      parse_path(parsed);
      return parsed;
    }
    case(DEL_FS):
    {
      int index = 0;
      while(_string[index] != '['){index += 1;}
      std::vector<std::string> files = lunion(_string.substr(index,_string.length()));
      std::vector<std::string> tags = get_cwd_tags();
      std::string temp = "";
      for(uint i = 0; i < files.size(); i++)
      {
        for(uint j = 0; j < tags.size(); j++)
        {
          temp += tags[j] + "-";
        }
        temp += files[i];
        parsed.push_back(temp);
        temp = "";
      }
      return parsed;
    }
    case(DEL_FP):
    {
      parse_path(parsed);
      return parsed;
    }
    case(OPEN_FP):
    {
      int index = 0;
      while(_string[index] != '-'){index += 1;}
      index += 1;
      std::string mode = "/";
      mode += _string[index];
      while(_string[index] != '/'){index += 1;}
      _string = _string.substr(index, _string.length());
      mode += _string;
      parsed.push_back(mode);
      return parsed;
    }
    case(OPEN_F):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(CLOSE_FP):
    {
      parse_path(parsed);
      return parsed;
    }
    case(CLOSE_F):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(RNAME_TS):
    {
      parse_rename(parsed);
      return parsed;
    }
    case(RNAME_FP):
    {
      std::string path;
      int index = 0;
      while(_string[index] != '/'){index += 1;}
      while(_string[index] != ' ')
      {
        path += _string[index];
        index += 1;
      }
      
      index += 4;
      std::string name;
      while(index < _string.length())
      {
        name += _string[index];
        index += 1;
      }
      path += ("/" + name);
      parsed.push_back(path);
      return parsed;
    }
    case(RNAME_FS):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(ATTR_FP):
    {
      parse_path(parsed);
      return parsed;
    }
    case(ATTR_FS):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(MERG_1_1):
    {
      parse_merge(parsed);
      return parsed;
    }
    case(MERG_M_1):
    {
      int index = 0;
      while(_string[index] != '['){index += 1;}
      std::vector<std::string> temp_vec = lunion(_string.substr(index,_string.length()));
      while(_string[index] != ' '){index += 1;}
      index += 1;
      std::string temp_str;
      while(_string[index] != ' '){index += 1;}
      index += 1;
      while(index < _string.size())
      {
        temp_str += _string[index];
        index += 1;
      }
      for(uint i = 0; i < temp_vec.size(); i++)
      {
        parsed.push_back(temp_vec[i] + "-" + temp_str);
      }
      return parsed;
    }
    case(TAG_FP):
    {
      std::string path;
      int index = 0;

       // Retrieve File Path
      jump_to_char(index,'/');
      substr_to_char(index, path, ' ');

      // Jump to tag list
      jump_to_char(index, '[');
  
      std::vector<std::string> tags = lunion(_string.substr(index,_string.length()));
      path += ">";
      for(uint i = 0; i < tags.size(); i++)
      {
        if(i + 1 >= tags.size()){path += tags[i];}
        else{path += (tags[i] + "-");}
      }

      parsed.push_back(path);
      return parsed;
    }
    case(TAG_FS):
    {
      std::string files;
      std::string tags;
      int index = 0;

      // Retieve the files list as string
      jump_to_char(index,'[');
      substr_to_char(index, files, ']');

      // Retrieve tag list as string
      jump_to_char(index,'[');
      substr_to_char(index, tags, ']');
      // Get Tag/File lists as vectors

      std::vector<std::string> vfiles = lunion(files);
      std::vector<std::string> vtags = lunion(tags);

      // build the parsed command
      std::string path;
      for(uint i = 0; i < vfiles.size(); i++)
      {
        if(_cwd != "/"){path = (_cwd + "/" + vfiles[i]);}
        else{path = _cwd + vfiles[i];}
        path += ">";
        for(uint j = 0; j < vtags.size(); j++)
        {
          if(j + 1 < vtags.size()){path += (vtags[j] + "-");}
          else{path += vtags[j];}
        }
        parsed.push_back(path);
        path = "";
      }
      return parsed;
    }
    case(UTAG_FP):
    {
      std::string path;
      int index = 0;

      // Retrieve File Path
      jump_to_char(index,'/');
      substr_to_char(index, path, ' ');

      // Jump to tag list
      jump_to_char(index, '[');
  
      std::vector<std::string> tags = lunion(_string.substr(index,_string.length()));
      path += ">";
      for(uint i = 0; i < tags.size(); i++)
      {
        if(i + 1 >= tags.size()){path += tags[i];}
        else{path += (tags[i] + "-");}
      }

      parsed.push_back(path);
      return parsed;
    }
    case(UTAG_FS):
    {
      std::string files;
      std::string tags;
      int index = 0;

      // Retieve the files list as string
      jump_to_char(index,'[');
      substr_to_char(index, files, ']');

      // Retrieve tag list as string
      jump_to_char(index,'[');
      substr_to_char(index, tags, ']');
            

            

      // Get Tag/File lists as vectors
      std::vector<std::string> vfiles = lunion(files);
      std::vector<std::string> vtags = lunion(tags);

      // build the parsed command
      std::string path;
      for(uint i = 0; i < vfiles.size(); i++)
      {
        if(_cwd != "/"){path = (_cwd + "/" + vfiles[i]);}
        else{path = _cwd + vfiles[i];}
        path += ">";
        for(uint j = 0; j < vtags.size(); j++)
        {
          if(j + 1 < vtags.size()){path += (vtags[j] + "-");}
          else{path += vtags[j];}
        }


        parsed.push_back(path);
        path = "";
      }
      return parsed;
    }
    case(READ_XP):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(READ_XCWD):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(READ_FP):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(READ_FCWD):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(WRITE_FP):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(WRITE_FCWD):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(WRITE_XFPF):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(WRITE_XFCWDF):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(CPY_FP):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(CPY_FCWD):
    {
      parsed.push_back("Coming Soon...");
      return parsed;
    }
    case(LIST_ALL):
    {
      return parsed;
    }
  }

  // Return empty vector == FAILED
  return parsed;
}
//======================================================================================================================
std::vector<std::string> Parser::get_cwd_tags()
{
  std::vector<std::string> tags;
  std::string tag;
  int index = 1;
  while(index < _cwd.length())
  {
    if(_cwd[index] != '/')
    {
      tag += _cwd[index];
      index += 1;
    }
    else
    {
      tags.push_back(tag);
      tag = "";
      index += 1;
    }
  }
  tags.push_back(tag);
  return tags;
}
//======================================================================================================================
std::vector<std::string> Parser::lunion(std::string string)
{
    std::vector<std::string> split;
    int index = 0;
    std::string temp = "";
    while(index < string.length())
    {

        if(string.length() == 0){ return split; }
        else if(string[index] == '{')
        {
            std::vector<std::string> vec = lintersect(string.substr(index+1,string.length()));
            for(uint i = 0; i < vec.size(); i++)
            {
                split.push_back(vec[i]);
            }

            while(string[index] != '}'){ index += 1;}
            index += 2;
        }
        else if(string[index] == ']')
        {
            split.push_back(temp);
            break;
        }
        else if(string[index] != ',' && string[index] != '[')
        {
            temp += string[index];
            if(temp.length() > _max_name_size)
            {
              throw ParseError("[Error]: File or Tag Name Size Exceeds Maximum Allowed Size","[Parser.cpp::lunion()]: ");
            }
            index += 1;
        }
        else
        {
            if(temp != "") split.push_back(temp);
            temp = "";
            index += 1;
        }
    }

    return split;
}
//======================================================================================================================
std::vector<std::string> Parser::lintersect(std::string string)
{
    std::vector<std::string> split;
    int index = 0;
    std::string old_string;
    std::string temp = "";
    while(index < string.length())
    {

        if(string.length() == 0){ return split; }
        else if(string[index] == '}')
        {
            if(split.size() >= 1)
            {
                for(uint i = 0; i < split.size(); i++)
                {
                    split[i] += temp;
                }
                break;
            }
            split.push_back(temp);
            break;
        }
        else if(string[index] == '[')
        {
            std::vector<std::string> vec = lunion(string.substr(index+1,string.length()));
            if(split.size() != 0)
            {
                for(uint i = 0; i < split.size(); i++)
                {
                  split[i] += temp;
                }
                split = matrix_multiply(split,vec);
            }
            else
            {
                for(uint i = 0; i < vec.size(); i++)
                {
                  split.push_back(temp + vec[i]);
                }
            }
            temp = "";
            while(string[index] != ']'){ index += 1; }
            index += 1;
        }
        else
        {
            if(string[index] != '{') temp += string[index];
            if(temp.length() > _max_name_size)
            {
              throw ParseError("[Error]: File or Tag Name Size Exceeds Maximum Allowed Size","[Parser.cpp::lintersect()]: ");
            }
            index += 1;
        }
    }
    return split;
}
//======================================================================================================================
std::vector<std::string> Parser::matrix_multiply(const std::vector<std::string> v1, const std::vector<std::string> v2)
{
    std::vector<std::string> product;
    while(product.size() < (v1.size() * v2.size()))
    {
      if(v1.size() > v2.size())
      {
          for(uint i = 0; i < v1.size(); i++)
          {
              for(uint j = 0; j < v2.size(); j++)
              {
                  product.push_back(v1[i] + v2[j]);
              }
          }
      }
      else if(v1.size() < v2.size())
      {
          for(uint i = 0; i < v2.size(); i++)
          {
              for(uint j = 0; j < v1.size(); j++)
              {
                  product.push_back(v2[i] + v1[j]);
              }
          }
      }
      else
      {
        for(uint i = 0; i < v2.size(); i++)
          {
              for(uint j = 0; j < v1.size(); j++)
              {
                  product.push_back(v2[i] + v1[j]);
              }
          }
      }
    }

    return product;
}
//======================================================================================================================
void Parser::parse_path(std::vector<std::string>& parsed)
{
  int index = 0;
  while(_string[index] != '/'){index += 1;}
  parsed.push_back(_string.substr(index,_string.length()));
}
//======================================================================================================================
void Parser::parse_rename(std::vector<std::string>& parsed)
{
  
  int index = 0;
  std::string old_names;
  std::string new_names;
  while(_string[index] != '['){index += 1;}

  while(_string[index] != ']')
  {
    old_names += _string[index];
    index += 1;
  }
  old_names += _string[index];

  while(_string[index] != '['){index += 1;}

  while(_string[index] != ']')
  {
    new_names += _string[index];
    index += 1;
  }
  new_names += _string[index];


  std::vector<std::string> _old = lunion(old_names);
  std::vector<std::string> _new = lunion(new_names);
  if(_old.size() > _new.size())
  {
    // Not enough new names
    for(uint i = 0; i < _new.size(); i++)
    {
      parsed.push_back(_old[i] + "-" + _new[i]);
    }
  }
  else if(_old.size() < _new.size())
  {
    // Too many new names
    for(uint i = 0; i < _old.size(); i++)
    {
      parsed.push_back(_old[i] + "-" + _new[i]);
    }
  }
  else
  {
    // Sizes are equal
    for(uint i = 0; i < _old.size(); i++)
    {
      parsed.push_back(_old[i] + "-" + _new[i]);
    }
  }
}
//======================================================================================================================
void Parser::parse_merge(std::vector<std::string>& parsed)
{
  std::string temp1;
  std::string temp2;
  int index = 0;
  while(_string[index] != ' '){index += 1;}
  index += 1;
  while(_string[index] != ' ')
  {
    temp1 += _string[index];
    index += 1;
  }
  index += 1;
  while(_string[index] != ' '){index += 1;}
  index += 1;
  while(index < _string.size())
  {
    temp2 += _string[index];
    index += 1;
  }

  parsed.push_back(temp1 + "-" + temp2);
}
//======================================================================================================================
std::vector<std::string> Parser::split_on_delim(std::string s, char delim)
{
  std::vector<std::string> rval;
  std::string temp;
  int index = 0;
  while(index < s.length())
  {
    if(s[index] != delim)
    {
      temp += s[index];
      index += 1;
    }
    else
    {
      if(temp != "" ){rval.push_back(temp);}
      temp = "";
      index += 1;
    }
  }
  rval.push_back(temp);
  return rval;
}
//=================================================================================================
void Parser::jump_to_char(int& index, char delim)
{
  while(_string[index] != delim){index += 1;}
}
//=================================================================================================
void Parser::substr_to_char(int& index, std::string& s, char delim)
{
  while(_string[index] != delim)
  {
    s += _string[index];
    index += 1;
  }
  if(delim  == ']'){s += _string[index];}
}
//=================================================================================================
//=================================================================================================
Parser::~Parser(){}
//=================================================================================================
Parser::Parser(){}
//=================================================================================================
Parser::Parser(char* buffer, char* cwd, int max_name_size)
{
  _string = buffer; 
  _cwd = cwd;
  _max_name_size = max_name_size;
}
//=================================================================================================
Parser::Parser(const char* buffer, const char* cwd, int max_name_size)
{
  _string = buffer; 
  _cwd = cwd;
  _max_name_size = max_name_size;
}
//=================================================================================================
Parser::Parser(std::string string, std::string cwd, int max_name_size)
{
  _string = string; 
  _cwd = cwd;
  _max_name_size = max_name_size;
}
//=================================================================================================
void Parser::reset(const char* string_lit, const char* cwd)
{
   _string = string_lit;
   if(cwd != ""){ _cwd = cwd; };
}
//=================================================================================================
void Parser::reset(std::string string, std::string cwd)
{
  _string = string;
  if(cwd != ""){ _cwd = cwd; };
}
//=================================================================================================
void Parser::reset(char* buffer, char* cwd)
{
  _string = buffer;
  if(cwd != NULL){ _cwd = cwd; }
}
//=================================================================================================
void Parser::set_cwd(std::string cwd)
{
  _cwd = cwd;
}
//======================================================================================================================
