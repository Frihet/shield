/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sstream>

#include "include/logger.hh"

namespace logger
{

  /**
     The length of the date buffer. 20 should be enough, but let's be one the safe side
  */  
  namespace
  {
    const int TIME_BUFF_SIZE = 32;
  }

  logger &logger::
  operator << (const string &message)
  {
    if (!is_enabled ())
      return *this;
    
    __stream << __name;

    if (__show_timestamp)
      __stream << ": " << __timestamp ();

    if (__show_pid)
      __stream << ": " << __pid ();

    __stream << " " << message << endl;
    
    return *this;
  }

  string logger::__timestamp (void)
  {
    char buff[TIME_BUFF_SIZE];
    
    time_t tstamp = time (0);
    struct tm *t_struct = localtime (&tstamp);
    
    size_t ret_code = strftime (buff, TIME_BUFF_SIZE, "%Y-%m-%d %H:%M:%S", t_struct);
    
    if (ret_code)
      {
	return string (buff);
      }
    
    /*
      Return empty string on error - it's much more important to get
      the log message out than having correct formating, even though
      the latter is also very important.
    */
    return "";
  }

  string logger::__pid (void)
  {
    pid_t pid = getpid ();

    std::ostringstream out;
    if (!(out << pid))
      {
	/*
	  Return empty string on error - it's much more important to get
	  the log message out than having correct formating, even though
	  the latter is also very important.
	*/
	return "";
      }
    return out.str ();
  }
  
}
