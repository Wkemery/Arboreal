
#include "Parser.h"



//======================================================================================================================
std::vector<std::string> Parser::parse(int type)
{
    std::vector<std::string> parsed;
    parsed.push_back(std::to_string(type));

    if(type == 4 || type == 5 || type == 6 || type == 7
        || type == 9 || type == 10 || type == 11 || type == 16)
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
      case(8):
      {
        std::string temp;
        int index = 0;

        // Skip command start (i.e. command name + flag(s))
        while(_string[index] != '-'){ index += 1;}
        index += 3;

        // Read in file name
        while(_string[index] != ' ')
        {
          temp += _string[index];
          index += 1;
        }
        parsed.push_back(temp);

        // Skip to tag list
        while(_string[index] != '['){ index += 1;}

        // Read in tag list
        std::vector<std::string> temp_vec = lunion(_string.substr(index,_string.length()));
        for(uint i = 0; i < temp_vec.size(); i++)
        {
          parsed.push_back(temp_vec[i]);
        }
        return parsed;
      }
      case(12):
      {
        parse_path(parsed);
        return parsed;
      }
      case(13):
      {
        parse_path(parsed);
        return parsed;
      }
      case(14):
      {
        parse_rename(parsed);
        return parsed;
      }
      case(15):
      {
        parse_rename(parsed);
        return parsed;
      }
      case(17):
      {
        parse_merge(parsed);
        return parsed;
      }
      case(18):
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
      case(19):
      {
        int index = 0;
        std::string filename;
        while(_string[index] != ' '){index += 1;}
        index += 1;
        while(_string[index] != ' ')
        {
          filename += _string[index];
          index += 1;
        }
        while(_string[index] != '['){index += 1;}
        std::vector<std::string> tags = lunion(_string.substr(index,_string.length()));

        for(uint i = 0; i < tags.size(); i++)
        {
          parsed.push_back(filename + "-" + tags[i]);
        }
        return parsed;
      }
      case(20):
      {
        int index = 0;
        while(_string[index] != '['){index += 1;}
        std::vector<std::string> files = lunion(_string.substr(index,_string.length()));

        while(_string[index] != '>'){index += 1;}
        index += 2;

        std::string tag;
        while(index < _string.size())
        {
          tag += _string[index];
          index += 1;
        }

        for(uint i = 0; i < files.size(); i++)
        {
          parsed.push_back(files[i] + "-" + tag);
        }

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
  while(_string[index] != '['){index += 1;}
  _string = _string.substr(index,_string.length());
  std::vector<std::string> old_names = lunion(_string);
  index += 1;
  while(_string[index] != '['){index += 1;}
  _string = _string.substr(index,_string.length());
  std::vector<std::string> new_names = lunion(_string);

  if(old_names.size() > new_names.size())
  {
    // Not enough new names
    for(uint i = 0; i < new_names.size(); i++)
    {
      parsed.push_back(old_names[i] + "-" + new_names[i]);
    }
  }
  else if(old_names.size() < new_names.size())
  {
    // Too many new names
    for(uint i = 0; i < old_names.size(); i++)
    {
      parsed.push_back(old_names[i] + "-" + new_names[i]);
    }
  }
  else
  {
    // Sizes are equal
    for(uint i = 0; i < old_names.size(); i++)
    {
      parsed.push_back(old_names[i] + "-" + new_names[i]);
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
Parser::Parser(char* buffer, char* cwd){ _string = buffer; _cwd = cwd;}
Parser::Parser(std::string string, std::string cwd){ _string = string; _cwd = cwd;}
Parser::Parser(){}
Parser::~Parser(){}
void Parser::reset(const char* string_lit, const char* cwd)
{
   _string = string_lit;
   if(cwd != ""){ _cwd = cwd; };
 }
void Parser::reset(std::string string, std::string cwd)
{
  _string = string;
  if(cwd != ""){ _cwd = cwd; };
}
void Parser::reset(char* buffer, char* cwd)
{
  _string = buffer;
  if(cwd != NULL){ _cwd = cwd; }
}
//======================================================================================================================
