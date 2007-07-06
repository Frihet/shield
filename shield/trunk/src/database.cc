#include <stdlib.h>

#include "database.hh"
#include "exception.hh"

namespace shield
{

  namespace database
  {
    using namespace oracle::occi;
    using namespace shield;

    namespace
    {
      string username="", password="", host="";
      bool is_init=false, is_connect=false;

      Environment *env;
      Connection *conn;

      result_set last_rs (0, 0, "");

      void destroy ();

      void connect ()
      {
	if (is_connect)
	  {
	    return;
	  }

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
	  }

	if (!conn)
	  throw shield::exception::database ("Could not create database connection. Wrong username/password?");
	
	atexit (&destroy);
      
      }

      void destroy ()
      {
	last_rs.close ();

	/*
	  FIXME:
	
	  These are commented out because they cause the app to
	  crash on exit. According to
	  http://forums.oracle.com/forums/thread.jspa?threadID=400555&tstart=-1
	  this may be a known Oracle bug. Will have to investigate more later.
	*/
	//env->terminateConnection (conn);
	//Environment::terminateEnvironment (env);
      }


    }


    void init (string u, string p, string h) throw (exception::database)
    {
      username = u;
      password=p;
      host=h;    
      is_init = true;
    }

    result_set &query (string q) throw (exception::database)
    {
      connect ();

      Statement *stmt = conn->createStatement ();
      if (!stmt)
	throw shield::exception::database ("Could not create database statement. Connection timeout?");

      last_rs.close ();
      last_rs = result_set (conn, stmt, q);
      return last_rs;
    }


  }

}
