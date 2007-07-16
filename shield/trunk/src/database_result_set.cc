/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/database.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{
  
  namespace database
  {

    using namespace util;
    using namespace oracle::occi;
    using namespace shield::exception;
    
    result_set::
    result_set (Connection *conn, Statement *stmt, const string &query)
      : __conn (conn), __stmt (stmt), __is_metadata_init (false), __query (query), __is_executed (false), __rs (0), __is_closed (false)
    {
    }

    bool result_set::
    is_null (string col)
    {
      int idx = __get_col_index (col);
      return __rs->isNull (idx);
    }

    void result_set::
    __metadata_init ()
    {
      vector<MetaData> cmd;

      if (__is_metadata_init)
	{
	  return;
	}
      
      __is_metadata_init = true;
      
      cmd = __rs->getColumnListMetaData();
      
      for (int i=0; i<cmd.size (); i++)
	{
	  string name = cmd[i].getString(MetaData::ATTR_NAME);      
	  __col_lookup[to_lower (name)] = i;
	}
    }

    int result_set::
    __get_col_index (string col)
    {
      
      __metadata_init ();
      
      map<string,int>::const_iterator p = __col_lookup.find (to_lower (col));
      if (p == __col_lookup.end ())
	{
	  throw not_found (col);
	}

      return (*p).second;

    }

    string result_set::
    get_string (string col)
    {
      int idx = __get_col_index (col);

      try
	{
	  return __rs->getString (idx+1);
	}
      catch (std::exception &e)
	{
	  exit(0);
	}
    }

    bool result_set::
    next ()
    {
      __execute ();
      return __rs->next () != ResultSet::END_OF_FETCH;
    }
    
    void result_set::
    close ()
    {

      if (__is_closed)
	{
	  return;
	}

      __is_closed = true;

      if (__stmt)
	{
	  if (__is_executed)
	    {
	      __stmt->closeResultSet (__rs);
	    }
	  
	  __conn->terminateStatement (__stmt);
	}
    }

    void result_set::
    __set_parameter (const parameter &p)
    {
      __param.push_back (p);
    }


    void result_set::
    __execute ()
    {
      string q2;
      bool quoted = false;
      int param_pos = 0;

      if (__is_executed)
	return;

      __is_executed = true;
      
      for (int i = 0; i <__query.length (); i++)
	{
	  if (quoted)
	    {
	      if (__query[i] == '\'')
		quoted = false;
	      q2 += __query[i];
	    }
	  else
	    {
	      if (__query[i] == '%')
		{
		  if (param_pos == __param.size ())
		    {
		      throw syntax ("Used more parameters than was supplied in query '" + __query + "'");
		    }
		  q2 += __param[param_pos++].str ();
		}
	      else  
		{
		  if (__query[i] == '\'')
		    quoted = true;

		  q2 += __query[i];
		}
	    }
	} 
      
      if (param_pos != __param.size ())
	{
	  throw syntax ("Did not use all supplied parameters in query '" + __query + "'");
	}

      debug << q2;

      __rs = __stmt->executeQuery (q2);
    }

    result_set &operator << (result_set &r, const parameter &p)
    {
      r.__set_parameter (p);
      return r;
    }
    
  }

}

