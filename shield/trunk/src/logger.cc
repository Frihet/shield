#include <time.h>

#include "logger.hh"

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
    
    __stream << __name << ": " << __timestamp () << " " << message << endl;
    
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
  
}
