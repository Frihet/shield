/**
   @file database.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <stdlib.h>

#include "include/database.hh"
#include "include/exception.hh"

namespace shield
{

  namespace database
  {
    using namespace oracle::occi;
    using namespace shield;

    logger::logger warning ("shield: database warning");
    logger::logger debug ("shield: database debug");

    namespace
    {
      string username="", password="", host="";
      bool is_init=false, is_connect=false;

      Environment *env=0;
      Connection *conn=0;

      result_set last_rs (0, 0, "");

      void destroy (void);

      void connect (void)
      {
	if (is_connect)
	  {
	    return;
	  }

	warning << "Setting up new connection";

	if (!is_init)
	  {
	    throw exception::syntax ("Tried to use database without supplying login information");
	  }

	try
	  {
	    env = Environment::createEnvironment ();
	    conn = env->createConnection (username, password, host);
	  }
	catch (std::exception &e)
	  {
		  warning << "Oracle exception:";
		  warning << e.what();
	  }

	if (!conn)
	  {
	    throw shield::exception::database ("Could not create database connection. Wrong username/password?");
	  }

	warning << "Have new connection";

	atexit (&destroy);
	is_connect = true;
      
      }

      void destroy (void)
      {
	cerr.flush ();
	is_connect = false;
	
	last_rs.close ();
	
	if (env) 
	  {
	    if (conn) 
	      {
		env->terminateConnection (conn);
	      }
	    Environment::terminateEnvironment (env);
	  }
      }

    }


    void init (string u, string p, string h) throw (exception::database)
    {
      username = u;
      password=p;
      host=h;    
      is_init = true;
    }

    void reset()
    {
      warning << "Resetting connection to Oracle";
      try 
	{
	  is_connect = false;
	  if (env) 
	    {
	      if (conn) 
		{
		  env->terminateConnection (conn);
		}
	      Environment::terminateEnvironment (env);
	    }
	  env = 0;
	  conn = 0;
	}
      catch (...) 
	{
	  warning << "Error while resetting oracle connection";
	}

    }

    result_set &query (string q) throw (exception::database)
    {
      connect ();

      try 
	{
	  Statement *stmt = conn->createStatement ();
	  if (!stmt)
	    throw shield::exception::database ("Could not create database statement. Connection timeout?");
	  
	  last_rs.close ();
	  last_rs = result_set (conn, stmt, q);
	  return last_rs;
	}
      catch (std::exception &e) {
	/*
	 * If a query fails in shield, there is a 99.9 % chance this
	 * is caused by an Oracle disconnect, but there are way to
	 * many different possible error codes for this. We just
	 * assume the worst and disconnect.
	 */
	is_connect = false;
	
	if (env && conn) 
	  {
	    reset();
	  }

	throw e;
      }
    }


  }

}
