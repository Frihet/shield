#include <stdlib.h>

#include <occi.h>

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

    env = Environment::createEnvironment();
    conn = env->createConnection(username, password, host);

  }

  static void destroy ()
  {
    //    conn->terminateStatement(stmt);
    env->terminateConnection(conn);
  }

}

}
