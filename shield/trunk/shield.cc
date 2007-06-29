
#include <iostream>
using namespace std;

#include <locale.h>
#include <getopt.h>

#include "exception.hh"
#include "transform.hh"
#include "database.hh"

namespace shield
{

  namespace transform
  {
#include "transform_yacc.hh"
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
    string username = "", password = "", host = "localhost";

    /*    
	  Parse options
    */
    while( 1 )
      {
        static struct option
	  long_options[] =
	  {
	    {   
	      "help", no_argument, 0, 'H'
	    }
	    ,
	    {   
	      "package", no_argument, 0, 'P'
	    }
	    ,
	    {   
	      "username", required_argument, 0, 'u'
	    }
	    ,
	    {   
	      "password", required_argument, 0, 'p'
	    }
	    ,
	    {   
	      "host", required_argument, 0, 'h'
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
                               "u:p:h:PH",
                               long_options,
                               &opt_index );

        if( opt == -1 )
	  break;

        switch( opt )
	  {   
	  case 0:
	    break;
	    
	  case 'H':
	    print_help( cout );
	    exit(0);
	    
	  case 'P':
	    transform::print_package ();
	    break;

	  case 'u':
	    username = optarg;
	    break;

	  case 'p':
	    password = optarg;
	    break;

	  case 'h':
	    host = optarg;
	    break;

	  case '?':
	    exit (1);

	  }
	
      }

    database::init (username, password,host);

  }

}


int
main (int argc, char **argv)
{

  string str="";

  setlocale (LC_ALL, "");
  
  shield::parse_args (argc, argv);
  
  int err = 0;
  while( true )
    {
      int c;

      c = cin.get ();

      if (c && c != EOF)
	{
	  str += c;
	}
      else
	{
	  try
	    {

	      if (str != "")
		{
		  shield::transform::lex_set_string (str);
		  err += shield::transform::yyparse ();
		  cout << shield::transform::sep;
		  cout << shield::transform::sep;
		}

	      str="";
	      
	    }
	  catch (std::exception &e)
	    {

	      err ++;
	      cerr << e.what () << endl;
	      
	      break;
	    }

	  if (c==EOF)
	    {
	      break;
	    }
	}
    }
  
  return err;
}

