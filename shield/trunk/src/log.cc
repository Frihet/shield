#include <time.h>

#include "log.hh"

namespace log
{

  /**
     The length of the date buffer. 20 should be enough, but let's be one the safe side
  */  
  const static int TIME_BUFF_SIZE = 32;

  log &log::
  operator << (const string &message)
  {
    if (!is_enabled ())
      return *this;
    
    __stream << __name << ": " << __timestamp () << " " << message << endl;

    return *this;
  }

  string log::__timestamp (void)
  {
    char buff[TIME_BUFF_SIZE];
    
    time_t tstamp = time (0);
    struct tm *t_struct = localtime (&tstamp);

    size_t ret_code = strftime (buff, TIME_BUFF_SIZE, "%Y-%m-%d %H:%M:%S", t_struct);

    if (ret_code)
      {
	return string (buff);
      }

    return "";
  }
  
}
