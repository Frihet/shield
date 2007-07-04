#include <stdlib.h>
#include <assert.h>

#include "database.hh"
#include "exception.hh"

using namespace oracle::occi;
using namespace shield;

namespace shield
{

  namespace database
  {
    static string username="", password="", host="";
    static bool is_init=false, is_connect=false;

    static Environment *env;
    static Connection *conn;

    static result_set last_rs (0, 0, "");

    static void destroy ();

    void init (string u, string p, string h)
    {
      username = u;
      password=p;
      host=h;    
      is_init = true;
    }

    static void connect ()
    {
      if (is_connect)
	{
	  return;
	}

      if (!is_init)
	{
	  throw exception::syntax ("Tried to use database without supplying login information");
	}

      env = Environment::createEnvironment ();
      conn = env->createConnection (username, password, host);

      assert (conn);

      atexit (&destroy);
      
    }

    static void destroy ()
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

    result_set &query (string q)
    {
      connect ();

      Statement *stmt = conn->createStatement ();
      assert (stmt);

      last_rs.close ();
      last_rs = result_set (conn, stmt, q);
      return last_rs;
    }


  }

}
