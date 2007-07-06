/**
   @file database.hh

   This file contains an extremely trivial abstraction of a single
   process-wide database connection. One needs to initialize using the
   init function to provide username, password, etc. The connection is
   then not actually created until the first query is executed. If a
   connection has been created, it is automatically closed on exit.

   The functions in this function may throw the exception::database exception

   @package Shield
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 or later.

*/
#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>
#include <map>
#include <occi.h>

#include "exception.hh"

namespace shield
{
  
  namespace database
  {
    
    using namespace std;

    /**
       A wrapper class used to give parameters to an sql query. Used
       together with a result_set as given from the query function and
       the '<<' operator.
    */

    class parameter
    {
    public:
      parameter (const char *s);
      parameter (const string &s);
      parameter (int i);
      parameter (const parameter &p);

      string str () const;
      
    private:
      string __val;
    }
    ;

    /**
       A class representing a result set. This class uses lazy
       evaluation and some magic to get a nice
       interface. Specifically, the << operator is overloaded to allow
       you to write things like:

       result_set &rs = query ("select * from test where foo=%") << "foo";

    */
    class result_set
    {
    public:
      
      result_set (oracle::occi::Connection *conn, oracle::occi::Statement *stmt, const string &query);

      bool is_null (string col);      
      int get_int (string col);
      string get_string (string col);
      bool next ();
      void close ();
      
    private:
      
      result_set (const result_set &rs)
      {
	throw shield::exception::syntax ("Can't copy result sets");
      }

      friend result_set &operator << (result_set &r, const parameter &p);
      friend result_set &query (string q) throw (exception::database);
      
      bool __is_metadata_init;
      bool __is_executed;
      bool __is_closed;

      /**
	 The column id lookup table for this resultset. It is used
	 when accessing a column by name. Note that column indices in
	 this map are 0-based, but the get* functions in occi are
	 1-based, so you need to add 1 to the value in the
	 map. Classical C vs. Pascal issue. Cost me two hours of
	 debuging. :-(
      */
      map<string,int> __col_lookup;
      vector<parameter> __param;
      
      string __query;
      
      oracle::occi::Statement *__stmt;
      oracle::occi::ResultSet *__rs;
      oracle::occi::Connection *__conn;
      
      int get_col_index (string col);
      void metadata_init ();
      void set_parameter (const parameter &p);
      void execute ();
    }
    ;
    
    result_set &operator << (result_set &r, const parameter &p);
    
    /**
       Call this function at atartup to set login information. This
       function must not be called exactly once.
    */
    void init (string username, string password, string host) throw (exception::database);
    
    /**
       Execute the specified query
    */
    result_set &query (string q) throw (exception::database);
    
  }
  
}

#endif
