
#include <iostream>
using namespace std;

#include <locale.h>
#include <getopt.h>

#include "shield_exception.hh"
#include "shield.hh"
namespace shield
{
#include "shield_yacc.hh"


/* Called by yyparse on error.  */
void yyerror (char const *s)
{
  cerr << "shield: " << s << "\n";
}

/*

replace from TABLE set key=4, col=7;

=>

delete from TABLE
where ((not shield.is_unique('key','TABLE')) or key = 4)
and ((not shield.is_unique('col','TABLE')) or col = 7);

insert into TABLE
(key, col)
values
(4, 7)

*/

static void print_help (ostream &s)
{
  s << "Usage: shield [OPTION]... ." << endl;
  s << "Convert MySQLP sql code read from stdin inte Oracle equivalents written to stdout." << endl;
}

static void parse_args (int argc, char **argv)
{
  /*    
	Parse options
  */
  while( 1 )
    {
        static struct option
            long_options[] =
	  {
	    {   
	      "help", no_argument, 0, 'h'
	    }
	    ,
	    {   
	      "package", no_argument, 0, 'p'
	    }
	    ,
	    {
	      0, 0, 0, 0
	    }
	  }
        ;

        int opt_index = 0;

        int opt = getopt_long( argc,
                               argv,
                               "p",
                               long_options,
                               &opt_index );

        if( opt == -1 )
	  break;

        switch( opt )
	  {   
	  case 0:
	    break;
	    
	  case 'h':
	    print_help( cout );
	    exit(0);
	    
	  case 'p':
	    print_package ();
	    break;

	  case '?':
	    exit (1);

	  }
}
}

}


int
main (int argc, char **argv)
{
  
  setlocale (LC_ALL, "");
  
  shield::parse_args (argc, argv);
  
  int err = 0;
  while( !err )
    {
      if (feof (stdin))
	{
	  break;
	}

      try
	{
	  err += shield::yyparse ();
	}
      catch( shield::unsupported_exception e )
	{
	  err ++;
	  cerr << e;
	  
	  break;
	}
      catch( shield::syntax_exception e )
	{
	  err ++;
	  cerr << e;
	  
	  break;
	}
    }
  
  return err;
}

