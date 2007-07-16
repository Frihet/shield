/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

/**
   Unit test for the lexer
*/

#include <iostream>

#include "include/transform.hh"

namespace shield
{
  namespace transform
  {
    /**
       Ugly hack - inject symbols from transform_yacc.hh into the
       shield::transform namespace. This is a plain C file, and we
       don't want to pollute the global namespace...
    */
#include "build/transform_yacc.hh"

  }
}

int
main (void)
{
  using namespace std;

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
  
  shield::transform::lex_set_string (input);
  
  for( int i=0; i < sizeof(result)/sizeof(int);i++ )
    {
      int d = shield::transform::lex_do();
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
