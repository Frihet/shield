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
#include <map>
#include <occi.h>

using namespace std;

namespace shield
{
  
  namespace database
  {
    
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
       A class representing a result set.
    */
    class result_set
    {
    public:
      
      result_set (oracle::occi::Statement *stmt, const string &query);
      
      bool is_null (string col);      
      int get_int (string col);
      string get_string (string col);
      bool next ();
      
      void close ();
      
    private:
      
      friend result_set &operator << (result_set &r, const parameter &p);
      
      bool __is_metadata_init;
      bool __is_executed;
      
      map<string,int> __col_lookup;
      vector<parameter> __param;
      
      string __query;
      
      oracle::occi::Statement *__stmt;
      oracle::occi::ResultSet *__rs;
      
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
    void init (string username, string password, string host);
    
    /**
       Execute the specified query
    */
    result_set &query (string q);
    
    /**
       Get the last error
    */
    string error ();
    
  }
  
}

#endif
