/**
   Wrapper around the lexer, trying to make the interface a bit less non-c++
*/

#include <iostream>
using namespace std;

#include "transform.hh"

#include "src/transform_lex.cc"

namespace shield
{
  namespace transform
  {

    YY_BUFFER_STATE current=0;
    
    static void lex_cleanup ()
    {
      yy_delete_buffer (current);
    }

    void *lex_set_string (const string &str)
    {
      if (current)
	{
	  lex_cleanup ();
	}
      else
	{
	  atexit (&lex_cleanup);
	}

      current = yy_scan_string (str.c_str ());
      return (void *)current;
    }

  }
}
