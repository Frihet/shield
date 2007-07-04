
#include <string>
#include "exception.hh"

namespace shield
{

  namespace exception
  {
    
    not_found::not_found( const string &not_found )
    {
      __str = "Could not find item '" + not_found + "'";
    }
    
    const char *not_found::
    what () const throw ()
    {
      return __str.c_str ();
    }
    
  }

}
