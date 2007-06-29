
#include "database.hh"
#include "exception.hh"
#include "util.hh"

using namespace util;
using namespace oracle::occi;
using namespace shield::exception;

namespace shield
{
  
  namespace database
  {

    result_set::
    result_set (oracle::occi::Connection *conn, oracle::occi::Statement *stmt, const string &query)
      : __conn (conn), __stmt (stmt), __is_metadata_init (false), __query (query), __is_executed (false), __rs (0), __is_closed (false)
    {
      //cerr << "result_set ( " << __stmt << ", " << __query << "); " << this << endl;
    }

    bool result_set::
    is_null (string col)
    {
      int idx = get_col_index (col);
      
      return __rs->isNull (idx);
    }

    void result_set::
    metadata_init ()
    {
      if (__is_metadata_init)
	{
	  return;
	}

      __is_metadata_init = true;

      vector<MetaData>cmd = __rs->getColumnListMetaData();

      for (int i=0; i<cmd.size (); i++)
	{
	  string name = cmd[i].getString(MetaData::ATTR_NAME);      
	  __col_lookup[to_lower (name)] = i;
	}
    }

    int result_set::
      get_col_index (string col)
    {
      
      metadata_init ();
      
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
      int idx = get_col_index (col);
      //cerr << "Index of " << col << " is " << idx << endl;
      //cerr << "call getString on " << __rs << endl;
      try
	{
	  return __rs->getString (idx+1);
	}
      catch (std::exception &e)
	{
	  //cerr << "TRET" << endl;
	  exit(0);
	}
    }

    bool result_set::
    next ()
    {
      //cerr << "11" << endl;

      execute ();
      //cerr << "22" << endl;
      return __rs->next () != ResultSet::END_OF_FETCH;
    }
    
    void result_set::
    close ()
    {

      if (__is_closed)
	{
	  return;
	}

      //cerr << "Start close ()" << endl;

      __is_closed = true;

      if (__stmt)
	{

	  //cerr << "Close result " << __rs << endl;
	  if (__is_executed)
	    {
	      __stmt->closeResultSet (__rs);
	    }
	  
	  //cerr << "Close statement " << __stmt << endl;
	  __conn->terminateStatement (__stmt);
	}

      //cerr << "Finish close ()" << endl;
    }

    void result_set::
    set_parameter (const parameter &p)
    {
      __param.push_back (p);
    }


    void result_set::
    execute ()
    {
      if (__is_executed)
	return;

      //cerr << "execute query " << __query << " " << this << endl;
      
      __is_executed = true;
      string q2;
      bool quoted = false;
      int param_pos = 0;
      
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

      //cerr << "translated " << __query << " to " << q2 << endl;
      __rs = __stmt->executeQuery (q2);
      //cerr << "execute returned " << __rs << endl;
    }

    result_set &operator << (result_set &r, const parameter &p)
    {
      r.set_parameter (p);
      return r;
    }
    
  }

}

