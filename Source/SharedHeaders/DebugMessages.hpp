///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DebugMessages.hpp
// Handle Debug Printing
// Primary Author: Adrian Barberis
// For "Arboreal" Senior Design Project
//
//  Tue. | Mar. 20th | 2018 | 10:50 PM | Stable | Documented 
//
///////////////////////////////////////////////////////////////////////////////////////////////////



#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>


#ifndef DEBUG_H
#define DEBUG_H

std::mutex m;
std::unique_lock<std::mutex> lk(m);

class DebugMessages
{
public:


  //===============================================================================================
  /*!
   * Create a new DebugMessage object using default logfile name: 'Arboreal.log'
   * Automatically creates the log if it does not exist and if it does exist
   * it will overwrite all the data in the log with the empty string.
   * Sets the debug flag _DEBUG to FALSE on startup.
   */
  //===============================================================================================
  DebugMessages()
  {
    _DEBUG = false;
    _logfile.open(_logfile_name);
    _logfile << "";
    _logfile.close();
  }



  //===============================================================================================
  /*!
   * Create a new DebugMessage object using a user defined logfile name.
   * Automatically creates the log if it does not exist and if it does exist
   * it will overwrite all the data in the log with the empty string.
   * Sets the debug flag _DEBUG to FALSE on startup.
   */
  //===============================================================================================
  DebugMessages(std::string logfile_name)
  {
    _DEBUG = false;
    _logfile_name = logfile_name;
    _logfile.open(_logfile_name);
    _logfile << "";
    _logfile.close();
  }



  //===============================================================================================
  /*!
   * Default Destructor
   */
  //===============================================================================================
  ~DebugMessages(){}



  //===============================================================================================
  /*!
   * Turns Debugging ON
   * Sets _DEBUG to TRUE
   */
  void ON(void){_DEBUG = true;}
  //===============================================================================================



  //===============================================================================================
  /*!
   * Turns Debugging OFF
   * Sets _DEBUG to FALSE
   */
  //===============================================================================================
  void OFF(void){_DEBUG = false;}



  //===============================================================================================
  /*!
   * Template function for writing debug information to 
   * std::cout ONLY.
   * 
   * @param data: The data to be written to std::cout.  If the type of data
   * passed is not supported by std::cout, behavior is undefined.
   * 
   * @param force: If data needs to be written before debugging offically 
   * starts this flag should be set to TRUE.  Default value is FALSE.
   */
  //===============================================================================================
  template <typename T>
  void display(const T data, bool force=false)
  {
    if(!_DEBUG && !force){return;}

    std::cout << data << std::endl;
    std::cout << "---------------------------------------------------------------------------------" 
    << std::endl;
  }



  //===============================================================================================
  /*!
   * Template function for writing debug information to 
   * std::fstream ONLY.
   * 
   * @param data: The data to be written to a file.  If the type of data
   * passed is not supported by outstream operators, behavior is undefined.
   * 
   * @param force: If data needs to be written before debugging offically 
   * starts this flag should be set to TRUE.  Default value is FALSE.
   */
  //===============================================================================================
  template<typename T>
  void log(const T data, bool force=false)
  {
    if(!_DEBUG && !force){return;}


    //lock();
    _logfile.open(_logfile_name, std::ios_base::app);
    _logfile << data << std::endl;

    _logfile << "---------------------------------------------------------------------------------" 
    << std::endl;
    _logfile.close(); 
    //unlock();
  }
  

  
  //===============================================================================================
  /*!
   * Template function for writing debug information to 
   * std::cout AND std::fstream.
   * 
   * @param data: The data to be written to std::cout and a file.  If the type of data
   * passed is not supported by std::cout or outstream operators, behavior is undefined.
   * 
   * @param force: If data needs to be written before debugging offically 
   * starts this flag should be set to TRUE.  Default value is FALSE.
   */
  //===============================================================================================
  template <typename T>
  void debug(const T data, bool force=false)
  {
    if(!_DEBUG && !force){return;}
    std::cout << data << std::endl;
    std::cout << "---------------------------------------------------------------------------------" 
    << std::endl;

    _logfile.open(_logfile_name, std::ios_base::app);

    _logfile << data << std::endl;
    _logfile << "---------------------------------------------------------------------------------" 
    << std::endl; 

    _logfile.close();
  }

  void lock(){lk.try_lock();}
  void unlock(){lk.unlock();}

private:
  bool _DEBUG;
  std::ofstream _logfile;
  std::string _logfile_name = "Arboreal.log";

};
#endif
