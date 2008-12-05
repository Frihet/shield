/**
   @file transform_lex_test.cc

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
main (int argc, char **argv)
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
      ';',
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
  
  if (argc > 1) 
    {
      for (int i=1; i<argc; i++) 
	{
	  shield::transform::lex_set_string (argv[1]);

	  printf ("String «%s» has the following tokens:\n", argv[i]);

	  while(1)
	    {
	      int d = shield::transform::lex_do();
	      
	      printf( "Symbol %d, value «%s»\n", d, shield::transform::yytext );
	      if (!d)
		break;
	    }            
	  printf ("\n\n");
	  
	}

    } 
  else
    {

      shield::transform::lex_set_string (input);
      
      for( unsigned int i=0; i < sizeof(result)/sizeof(int);i++ )
	{
	  int d = NOTHING;
	  while(d==NOTHING)
	    {
	      d = shield::transform::lex_do();
	    }
	  
	  if (d != result[i] )
	    {
	      printf( "Symbol number %d, '%s': Got symbol ", i, shield::transform::yytext );
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
  return 0;
}
