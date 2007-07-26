/**
   @file shield.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

/*
  This is the file where the Shield main loop is defined. Nothing
  interesting happens here, the main loop calls out to yyparse to do
  everything even remotely cool.
*/
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "include/util.hh"
#include "include/exception.hh"
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
 
namespace
{
  
  vector<string> command;
    

}


int
main (int argc, char **argv)
{
  int count = 0;
  string str="";

  int remaining;

  setlocale (LC_ALL, "");
  
  shield::error.enable ();
  remaining = shield::parse_args (argc, argv);

  for (int i = remaining; i<argc; i++)
    {
      command.push_back (string(argv[i]));
    }

  if (command.size ())
    {
      vector<string>::const_iterator it;
      
      for (it=command.begin (); it != command.end (); ++it)
	{
	  cout << shield::transform::translate (*it);
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
	      cout << shield::transform::translate (str);
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

