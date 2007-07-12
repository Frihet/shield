/**
   Wrapper around the lexer, trying to make the interface a bit less non-c++
*/

#include <iostream>
using namespace std;

#include "include/transform.hh"

#include "src/transform_lex.cc"

int lex_internal_wrap ()
{
  return 1;
}

#undef yytext
#undef yytext

namespace shield
{
  namespace transform
  {

    int yypos=0;
    char *yytext;
    
    namespace
    {
      YY_BUFFER_STATE current=0;
      string current_string = "";

      void lex_cleanup ()
      {
	lex_internal__delete_buffer (current);
      }
    }

    string lex_get_string ()
    {
      return current_string;
    }

    void *lex_set_string (const string &str)
    {
      yypos=0;
      if (current)
	{
	  lex_cleanup ();
	}
      else
	{
	  atexit (&lex_cleanup);
	}
      current_string = str;
      current = lex_internal__scan_string (str.c_str ());
      return (void *)current;
    }

    int lex_do (void)
    {
      int res = lex_internal_lex ();
      yytext = lex_internal_text;
      yypos += strlen (yytext);
      return res;
    }

  }
}
