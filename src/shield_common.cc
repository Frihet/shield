/**
   @file shield_common.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/
#include <locale.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <unistd.h>

#include "include/transform.hh"
#include "include/shield.hh"
#include "include/logger.hh"
#include "include/util.hh"
#include "include/catalyst.hh"
#include "include/database.hh"
#include "include/introspection.hh"

using namespace std;


namespace shield
{

  logger::logger error ("shield: error");
  
  namespace
  {

    const char *GETOPT_ARG = "c:u:p:Phd:w:s:";
    
    string socket_name = "/tmp/shield.socket";
    
    /**
       Check that environment looks ok. 
    */
    void startup_test ()
    {
    }
    
    
    /**
       Print a short help message.
       
       @param stream the stream to print the message to
    */
    void print_help (const string &program, ostream &stream)
    {
      stream << "Usage: " << program << " [OPTION]... ." << endl;
      stream << "Convert MySQLP sql code read from stdin inte Oracle equivalents written to stdout." << endl;
    }

  }

    /**
       Parse command line arguments
    */
  int parse_args (int argc, char **argv)
    {
      string username = "", password = "", host = "localhost";

      char *env;

      env = getenv("SHIELD_USERNAME");
      if( env )
	{
	  username = env;
	}

      env = getenv("SHIELD_PASSWORD");
      if( env )
	{
	  password = env;
	}

      env = getenv("SHIELD_CONNECTION");
      if( env )
	{
	  host = env;
	}

      startup_test ();

      /*    
	    Main parse loop
      */
      while( 1 )
	{
	  int opt_index = 0;

#ifdef HAVE_GETOPT_LONG
	  static struct option
	    long_options[] =
	    {
	      {   
		"help", no_argument, 0, 'h'
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
		"connection", required_argument, 0, 'c'
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
		"socket-file", required_argument, 0, 's'
	      }
	      ,
	      {
		0, 0, 0, 0
	      }
	    }
	  ;


	  int opt = getopt_long (argc,
				 argv,
				 GETOPT_ARG,
				 long_options,
				 &opt_index);
#else
	  int opt = getopt (argc,
			    argv,
			    GETOPT_ARG);
				

#endif //#ifdef HAVE_GETOPT_LONG

	  if (opt == -1)
	    break;

	  switch (opt)
	    {   
	      
	    case 0:
	      break;

	    case 'h':
	      print_help (argv[0],cout);
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

	    case 'c':
	      host = optarg;
	      break;

	    case 'w':
	      if (contains (optarg, "transform", "all"))
		{
		  shield::transform::warning.enable ();
		}

	      if (contains (optarg, "catalyst", "all"))
		{
		  shield::catalyst::warning.enable ();
		}

	      if (contains (optarg, "introspection", "all"))
		{
		  shield::introspection::warning.enable ();
		}

	      if (contains (optarg, "database", "all"))
		{
		  shield::database::warning.enable ();
		}
	      break;

	    case 'd':
	      if (contains (optarg, "transform", "all"))
		{
		  shield::transform::debug.enable ();
		}

	      if (contains (optarg, "catalyst", "all"))
		{
		  shield::catalyst::debug.enable ();
		}

	      if (contains (optarg, "introspection", "all"))
		{
		  shield::introspection::debug.enable ();
		}

	      if (contains (optarg, "database", "all"))
		{
		  shield::database::debug.enable ();
		}
	      break;

	    case 's':
	      {
		socket_name = optarg;
		break;
	      }

	    case '?':
	      exit (1);

	    }
	}
      
      shield::database::init (username, password, host);


      return optind;

    }

  std::string 
  get_socket_name ()
    {
      return socket_name;
    }

}
