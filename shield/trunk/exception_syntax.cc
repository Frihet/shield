
#include <string>
#include "exception.hh"

namespace shield
{

  namespace exception
  {
    
    syntax::syntax( const string &err )
    {
      __str = "Syntax error: "+ err;
    }
    
    const char *syntax::
    what () const throw ()
    {
      return __str.c_str ();
    }
  
  }

}
