/**
   \file database.hh

   This file contains an extremely trivial abstraction of a single
   process-wide database connection. One needs to initialize using the
   init function to provide username, password, etc. The connection is
   then not actually created until the first query is executed. If a
   connection has been created, it is automatically closed on exit.
*/
#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>

using namespace std;

namespace shield
{
  
  namespace database
  {
    
    /**
       Call thsi function at atartup to set login information. This
       function must not be called exactly once.
    */
    void init (string username, string password, string host);
    
    /**
       Execute the specified query
    */
    bool query (string q, ...);
    
    /**
       Get the last error
    */
    string error ();
    
  }
  
}

#endif
