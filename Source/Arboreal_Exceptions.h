/*Arboreal_Exceptions.h
 * Arboreal
 * January 2018
 */

#include<string>
#include<stdexcept>
using std::string;

#ifndef ARBOREAL_EXCEPTIONS
#define ARBOREAL_EXCEPTIONS

class arboreal_exception : public std::runtime_error
{
protected:
  string _where;
public:
  arboreal_exception(const char* what, const char* where);
  arboreal_exception(const char* what, const string& where);
  arboreal_exception(const string& what, const string& where);
  arboreal_exception(const string& what, const char* where);
  
  virtual const char* where() const;
};

class arboreal_runtime_error : public arboreal_exception
{
protected:
  string _where;
  
public:
  arboreal_runtime_error(const char* what, const char* where);
  arboreal_runtime_error(const char* what, const string& where);
  arboreal_runtime_error(const string& what, const string& where);
  arboreal_runtime_error(const string& what, const char* where);
};

class disk_error : public arboreal_runtime_error
{
public:
  disk_error(const char* what, const char* where);
  disk_error(const char* what, const string& where);
  disk_error(const string& what, const string& where);
  disk_error(const string& what, const char* where);
};

class tag_error : public arboreal_runtime_error
{
public:
  tag_error(const char* what, const char* where);
  tag_error(const char* what, const string& where);
  tag_error(const string& what, const string& where);
  tag_error(const string& what, const char* where);
};

class file_error : public arboreal_runtime_error
{
public:
  file_error(const char* what, const char* where);
  file_error(const char* what, const string& where);
  file_error(const string& what, const string& where);
  file_error(const string& what, const char* where);
};

class arboreal_logic_error : public arboreal_exception
{
public:
  arboreal_logic_error(const char* what, const char* where);
  arboreal_logic_error(const char* what, const string& where);
  arboreal_logic_error(const string& what, const string& where);
  arboreal_logic_error(const string& what, const char* where);
};

class invalid_arg : public arboreal_logic_error
{
public:
  invalid_arg(const char* what, const char* where);
  invalid_arg(const char* what, const string& where);
  invalid_arg(const string& what, const string& where);
  invalid_arg(const string& what, const char* where);
};
#endif