/*Arboreal_Exceptions.cpp
 * Arboreal
 * January 2018
 */
#include "Arboreal_Exceptions.h"

arboreal_exception::arboreal_exception(const char* what, const char* where)
: std::runtime_error(what), _where(where)
{}
arboreal_exception::arboreal_exception(const char* what, const string& where)
: std::runtime_error(what), _where(where)
{}
arboreal_exception::arboreal_exception(const string& what, const string& where)
: std::runtime_error(what), _where(where)
{}
arboreal_exception::arboreal_exception(const string& what, const char* where)
: std::runtime_error(what), _where(where)
{}
arboreal_exception::~arboreal_exception() throw() {}

const char* arboreal_exception::where() const
{
  return _where.c_str();
}
/******************************************************************************/

arboreal_runtime_error::arboreal_runtime_error(const char* what, const char* where)
: arboreal_exception(what, where)
{}
arboreal_runtime_error::arboreal_runtime_error(const char* what, const string& where)
: arboreal_exception(what, where)
{}
arboreal_runtime_error::arboreal_runtime_error(const string& what, const string& where)
: arboreal_exception(what, where)
{}
arboreal_runtime_error::arboreal_runtime_error(const string& what, const char* where)
: arboreal_exception(what, where)
{}
arboreal_runtime_error::~arboreal_runtime_error() throw() {}
/******************************************************************************/

arboreal_cli_error::arboreal_cli_error(const string& where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_cli_error::arboreal_cli_error(const char* where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_cli_error::arboreal_cli_error(const string& where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_cli_error::arboreal_cli_error(const char* where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_cli_error::~arboreal_cli_error() throw() {}
/******************************************************************************/

arboreal_liaison_error::arboreal_liaison_error(const string& where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_liaison_error::arboreal_liaison_error(const char* where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_liaison_error::arboreal_liaison_error(const string& where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_liaison_error::arboreal_liaison_error(const char* where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_liaison_error::~arboreal_liaison_error() throw() {}
/******************************************************************************/

arboreal_daemon_error::arboreal_daemon_error(const string& where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_daemon_error::arboreal_daemon_error(const char* where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_daemon_error::arboreal_daemon_error(const string& where, const char* what)
: arboreal_exception(what, where)
{}
arboreal_daemon_error::arboreal_daemon_error(const char* where, const string& what)
: arboreal_exception(what, where)
{}
arboreal_daemon_error::~arboreal_daemon_error() throw() {}
/******************************************************************************/

disk_error::disk_error(const char* what, const char* where)
: arboreal_runtime_error(what, where)
{}
disk_error::disk_error(const char* what, const string& where)
: arboreal_runtime_error(what, where)
{}
disk_error::disk_error(const string& what, const string& where)
: arboreal_runtime_error(what, where)
{}
disk_error::disk_error(const string& what, const char* where)
: arboreal_runtime_error(what, where)
{}
disk_error::~disk_error() throw() {}
/******************************************************************************/

tag_error::tag_error(const char* what, const char* where)
: arboreal_runtime_error(what, where)
{}
tag_error::tag_error(const char* what, const string& where)
: arboreal_runtime_error(what, where)
{}
tag_error::tag_error(const string& what, const string& where)
: arboreal_runtime_error(what, where)
{}
tag_error::tag_error(const string& what, const char* where)
: arboreal_runtime_error(what, where)
{}
tag_error::~tag_error() throw() {}
/******************************************************************************/

file_error::file_error(const char* what, const char* where)
: arboreal_runtime_error(what, where)
{}
file_error::file_error(const char* what, const string& where)
: arboreal_runtime_error(what, where)
{}
file_error::file_error(const string& what, const string& where)
: arboreal_runtime_error(what, where)
{}
file_error::file_error(const string& what, const char* where)
: arboreal_runtime_error(what, where)
{}
file_error::~file_error() throw() {}
/******************************************************************************/

/******************************************************************************/

arboreal_logic_error::arboreal_logic_error(const char* what, const char* where)
: arboreal_exception(what, where)
{}
arboreal_logic_error::arboreal_logic_error(const char* what, const string& where)
: arboreal_exception(what, where)
{}
arboreal_logic_error::arboreal_logic_error(const string& what, const string& where)
: arboreal_exception(what, where)
{}
arboreal_logic_error::arboreal_logic_error(const string& what, const char* where)
: arboreal_exception(what, where)
{}
arboreal_logic_error::~arboreal_logic_error() throw() {}
/******************************************************************************/

invalid_arg::invalid_arg(const char* what, const char* where)
: arboreal_logic_error(what, where)
{}
invalid_arg::invalid_arg(const char* what, const string& where)
: arboreal_logic_error(what, where)
{}
invalid_arg::invalid_arg(const string& what, const string& where)
: arboreal_logic_error(what, where)
{}
invalid_arg::invalid_arg(const string& what, const char* where)
: arboreal_logic_error(what, where)
{}
invalid_arg::~invalid_arg() throw() {}
