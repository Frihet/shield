#include <stdlib.h>
#include <assert.h>

#include "database.hh"
#include "exception.hh"

using namespace oracle::occi;

using namespace shield::exception;

namespace shield
{

namespace database
{
  string username="", password="", host="";
  bool is_init=false, is_connect=false;

  Environment *env;
  Connection *conn;

  result_set last_rs = result_set (0, "");

  static void destroy ();

  void init (string u, string p, string h)
  {
    username = u;
    password=p;
    host=h;    
    is_init = true;

    atexit (&destroy);
  }

  static void connect ()
  {
    if (is_connect)
      {
	return;
      }

    if (!is_init)
      {
	throw syntax_exception ("Tried to use database without supplying login information");
      }

    env = Environment::createEnvironment ();
    conn = env->createConnection (username, password, host);

    assert (conn);

  }

  static void destroy ()
  {
    //    conn->terminateStatement (stmt);
    env->terminateConnection (conn);
    //    env->terminateEnvironment ();
  }

  result_set &query (string q)
  {
    connect ();

    Statement *stmt = conn->createStatement ();
    assert (stmt);

    last_rs = result_set (stmt, q);
    return last_rs;
  }


}

}
