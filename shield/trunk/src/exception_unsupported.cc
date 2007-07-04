
#include <string>
#include "exception.hh"
#include "util.hh"

namespace shield
{

  namespace exception
  {

    unsupported::unsupported( const string &file, int line )
    {
      __str = "Unsupported feature.\nFile: " + file+ ", line: " + util::stringify (line);
    }

    const char *unsupported::
    what () const throw ()
    {
      return __str.c_str ();
    }
    
}

}
