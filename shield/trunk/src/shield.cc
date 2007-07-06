/*
   This is the file where the Shield main loop is defined. Nothing
   interesting happens here, the main loop calls out to yyparse to do
   everything even remotely cool.
*/
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
    /**
       Ugly hack - inject symbols from transform_yacc.hh into the
       shield::transform namespace. This is a plain C file, and we
       don't want to pollute the global namespace...
    */
#include "transform_yacc.hh"
  }

  /**
     Print a short help message.

     @param stream the stream to print the message to
  */
  static void print_help (ostream &stream)
  {
    stream << "Usage: shield [OPTION]... ." << endl;
    stream << "Convert MySQLP sql code read from stdin inte Oracle equivalents written to stdout." << endl;
  }

  /**
     Parse command line arguments
  */
  static void parse_args (int argc, char **argv)
  {
    string username = "", password = "", host = "localhost";

    /*    
	  Main parse loop
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

/**
   Check that environment looks ok. 
*/
static void startup_test ()
{
  if (!getenv ("ORACLE_HOME"))
    {
      cerr << "ORACLE_HOME environment variable is not set. Define it and restart shield." << endl;
      exit (1);
    }
}

int
main (int argc, char **argv)
{

  string str="";

  startup_test ();

  setlocale (LC_ALL, "");
  
  shield::parse_args (argc, argv);

  shield::transform::debug.enable ();
  logger::logger error ("shield: error");

  error.enable ();

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
		}
	      
	      str="";
	      
	    }
	  catch (std::exception &e)
	    {
	      err ++;
	      error << e.what ();
	      
	      break;
	    }

	  cout << shield::transform::sep;
	  cout << shield::transform::sep;
	  cout.flush ();
		  
	  if (c==EOF)
	    {
	      break;
	    }
	}
    }
  
  return err;
}

