/*
  This is the file where the Shield main loop is defined. Nothing
  interesting happens here, the main loop calls out to yyparse to do
  everything even remotely cool.
*/
#include <iostream>
using namespace std;

#include <locale.h>
#include <getopt.h>

#include "include/util.hh"
#include "include/exception.hh"
#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/database.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace transform
  {
    /**
       Ugly hack - inject symbols from transform_yacc.hh into the
       shield::transform namespace. This is a plain C file, and we
       don't want to pollute the global namespace...
    */
#include "include/transform_yacc.hh"
  }

}
 
namespace
{
  
  logger::logger error ("shield: error");

  vector<string> command;

    /**
       Print a short help message.

       @param stream the stream to print the message to
    */
    void print_help (ostream &stream)
    {
      stream << "Usage: shield [OPTION]... ." << endl;
      stream << "Convert MySQLP sql code read from stdin inte Oracle equivalents written to stdout." << endl;
    }

    /**
       Parse command line arguments
    */
    void parse_args (int argc, char **argv)
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
		"warning", required_argument, 0, 'w'
	      }
	      ,
	      {   
		"debug", required_argument, 0, 'd'
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
				 "c:u:p:h:PHd:w:",
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
	      shield::transform::print_package ();
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

	    case 'w':
	      if (contains(optarg, "transform", "all"))
		{
		  shield::transform::warning.enable ();
		}

	      if (contains(optarg, "catalyst", "all"))
		{
		  shield::catalyst::warning.enable ();
		}

	      if (contains(optarg, "introspection", "all"))
		{
		  shield::introspection::warning.enable ();
		}

	      if (contains(optarg, "database", "all"))
		{
		  shield::database::warning.enable ();
		}
	      break;

	    case 'd':
	      if (contains(optarg, "transform", "all"))
		{
		  shield::transform::debug.enable ();
		}

	      if (contains(optarg, "catalyst", "all"))
		{
		  shield::catalyst::debug.enable ();
		}

	      if (contains(optarg, "introspection", "all"))
		{
		  shield::introspection::debug.enable ();
		}

	      if (contains(optarg, "database", "all"))
		{
		  shield::database::debug.enable ();
		}
	      break;

	    case '?':
	      exit (1);

	    }
	
	}

      for (int i =optind; i<argc; i++)
	{
	  command.push_back (string(argv[i]));
	}

      shield::database::init (username, password,host);

    }

  /**
     Check that environment looks ok. 
  */
  void startup_test ()
  {
    if (!getenv ("ORACLE_HOME"))
      {
	cerr << "ORACLE_HOME environment variable is not set. Define it and restart shield." << endl;
	exit (1);
      }
  }


  void translate (const string &str) throw ()
  {
    try
      {
	if (str != "")
	  {
	    shield::transform::lex_set_string (str);
	    shield::transform::yyparse ();
	  }
	
      }
    catch (const shield::exception::exception &e)
      {
	error << e.what ();
	cout << shield::transform::sep;
      }
    catch (const std::exception &e)
      {
	error << string("Non-shield exception thrown: ")+e.what ();
	cout << shield::transform::sep;
      }
    catch (...)
      {
	error << "Unknown error";
	cout << shield::transform::sep;
      }
    
    cout << shield::transform::sep;
    cout.flush ();
  }
    

}

int
main (int argc, char **argv)
{

  string str="";

  startup_test ();

  setlocale (LC_ALL, "");
  
  parse_args (argc, argv);

  error.enable ();

  if (command.size ())
    {
      vector<string>::const_iterator it;
      
      for (it=command.begin (); it != command.end (); ++it)
	{
	  translate (*it);
	}
    }
  else
    {
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
	      translate (str);
	      str="";
			      
	      if (c==EOF)
		{
		  break;
		}
	    }
	}
    }
  
  return 0;
}

