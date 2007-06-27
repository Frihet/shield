/**
   Unit test for the lexer
*/

#include <iostream>
using namespace std;

#include "transform.hh"
namespace shield
{
#include "shield_yacc.hh"
}

#include "shield_lex.cc"

namespace shield
{
  int foo ();

  int foo ()
  {
    return yylex ();
  }

}

int
main (void)
{
  int err=0;

  /*
    The input string
  */
  string input = "sellect `select` 'select' \"\n\t\n\nafd'\\g\\\"\" select\tinsert\n\n\ncreate    "
    "where table from limit dual not is distinct as asc;,.|&-+*/%^ ~ ()";

  /*
    The token types of the input
  */
  int result[] = 
    {
      IDENT,
      IDENT_QUOTED,
      TEXT_STRING,
      TEXT_STRING,
      SELECT_SYM,
      INSERT,
      CREATE,
      WHERE,
      TABLE_SYM,
      FROM,
      LIMIT,
      DUAL_SYM,
      NOT_SYM,
      IS,
      DISTINCT,
      AS,
      ASC,
      END_OF_INPUT,
      ',',
      '.',
      '|',
      '&',
      '-',
      '+',
      '*',
      '/',
      '%',
      '^',
      '~',
      '(',
      ')',
      0
    }
  ;
  
  yy_scan_string (input.c_str ());
  
  for( int i=0; i < sizeof(result)/sizeof(int);i++ )
    {
      int d = shield::foo();
      if (d != result[i] )
	{
	  printf( "Symbol number %d: Got symbol ", i );
	  printf( (d>=32 && d < 256)?"'%c'":"%d", d );
	  printf (", expected symbol " );
	  printf ( (result[i]>=32 && result[i] < 256)?"'%c'":"%d", result[i] );
	  printf( "\n");
	  err++;
	  
	  if (!d)
	    break;
	}            
    }
  
  printf ("Lexed %d tokens with %d errors\n", sizeof(result)/sizeof(int), err);
  
  return err;
}
