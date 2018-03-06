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
  ~arboreal_exception() throw();

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
  ~arboreal_runtime_error() throw();
};

class arboreal_cli_error : public arboreal_exception
{
public:
  arboreal_cli_error(const string& where, const string& what);
  arboreal_cli_error(const char* what, const char* where);
  arboreal_cli_error(const char* what, const string& where);
  arboreal_cli_error(const string& what, const char* where);
  ~arboreal_cli_error() throw();
};

class arboreal_liaison_error : public arboreal_exception
{
public:
  arboreal_liaison_error(const string& where, const string& what);
  arboreal_liaison_error(const char* what, const char* where);
  arboreal_liaison_error(const char* what, const string& where);
  arboreal_liaison_error(const string& what, const char* where);
  ~arboreal_liaison_error() throw();
};

class arboreal_daemon_error : public arboreal_exception
{
public:
  arboreal_daemon_error(const string& where, const string& what);
  arboreal_daemon_error(const char* what, const char* where);
  arboreal_daemon_error(const char* what, const string& where);
  arboreal_daemon_error(const string& what, const char* where);
  ~arboreal_daemon_error() throw();
};


class disk_error : public arboreal_runtime_error
{
public:
  disk_error(const char* what, const char* where);
  disk_error(const char* what, const string& where);
  disk_error(const string& what, const string& where);
  disk_error(const string& what, const char* where);
  ~disk_error() throw();
};

class tag_error : public arboreal_runtime_error
{
public:
  tag_error(const char* what, const char* where);
  tag_error(const char* what, const string& where);
  tag_error(const string& what, const string& where);
  tag_error(const string& what, const char* where);
  ~tag_error() throw();
};

class file_error : public arboreal_runtime_error
{
public:
  file_error(const char* what, const char* where);
  file_error(const char* what, const string& where);
  file_error(const string& what, const string& where);
  file_error(const string& what, const char* where);
  ~file_error() throw();
};

class arboreal_logic_error : public arboreal_exception
{
public:
  arboreal_logic_error(const char* what, const char* where);
  arboreal_logic_error(const char* what, const string& where);
  arboreal_logic_error(const string& what, const string& where);
  arboreal_logic_error(const string& what, const char* where);
  ~arboreal_logic_error() throw();
};

class invalid_arg : public arboreal_logic_error
{
public:
  invalid_arg(const char* what, const char* where);
  invalid_arg(const char* what, const string& where);
  invalid_arg(const string& what, const string& where);
  invalid_arg(const string& what, const char* where);
  ~invalid_arg() throw();
};
#endif
