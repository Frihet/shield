/**
   @file transform_cast.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/


#include "include/transform.hh"
#include "include/exception.hh"
#include "include/introspection.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    cast::
    cast (printable *p, int contexts)
      : __contexts (contexts)
    {
      if (!contexts)
	throw shield::exception::syntax ("Cast called with no legal type");

      while (1) 
	{
	  cast *c = dynamic_cast<cast *> (p);
	  if (c) 
	    {
	      p = c->_get_child (CHILD_ITEM);
	    }
	  else
	    {
	      break;
	    }
	}

      _set_child (CHILD_ITEM, p);
    }

    data_type cast::
    get_context (void)
    {
      printable *inner = get_item ();
      int desired;
      data_type real;

      if (!inner)
	throw shield::exception::syntax ("Cast of null node");

      real = inner->get_context ();
      
      /*
	Desired is a bitmask of all types that are ok. 
      */
      desired = __contexts;

      /*
	First check if the actual type is one of the allowed types. If so, do no casting.
      */
      if (real & desired)
	return real;

      data_type order[] = 
	{
	  DATA_TYPE_CLOB,
	  DATA_TYPE_VARCHAR,
	  DATA_TYPE_CHAR,
	  DATA_TYPE_NUMBER,
	  DATA_TYPE_FLOAT,
	  DATA_TYPE_DATETIME,
	  DATA_TYPE_DATE
	}
      ;

      for (size_t i=0; i<(sizeof(order)/sizeof(data_type)); i++)
	{
	  if (desired & order[i])
	    {
	      return order[i];
	    }
	}
      
      throw shield::exception::syntax ("Unknown desired context " + desired);
    }
    

    void cast::
    _print (ostream &stream)
    {
      printable *inner = get_item ();
      data_type real;
      int desired;

      if (!inner)
	throw shield::exception::syntax ("Cast of null node");

      real = inner->get_context ();
      
      /*
	Desired is a bitmask of all types that are ok. 
      */
      desired = __contexts;
      
      //      debug << ("Cast from " + ENUM_TO_STRING (data_type, real) + " to whatever");
      
      /*
	First check if the actual type is one of the allowed types. If so, do no casting.
      */
      if (real & desired)
	{
	  stream << *inner;
	}
      else
	{
	  stream << " ";

	  if (desired & (DATA_TYPE_CHAR | DATA_TYPE_VARCHAR))
	    {
	      if (real & (DATA_TYPE_CHAR | DATA_TYPE_VARCHAR))
		{
		  stream << *inner;
		}
	      else if (real & DATA_TYPE_DATETIME)
		{
		  inner->set_skip_space (true);
		  stream << "shield.handle_zero_date (to_char (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss'))";
		}
	      else if (real & DATA_TYPE_DATE)
		{
		  inner->set_skip_space (true);
		  stream << "shield.handle_zero_date (to_char (" << *inner << ", 'yyyy-mm-dd'))";
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "to_char (" << *inner << ")";
		}
	    }
	  else if (desired & DATA_TYPE_CLOB)
	    {  
	      inner->set_skip_space (true);

	      if (real & DATA_TYPE_DATETIME)
		{
		  stream << "to_clob (shield.handle_zero_date (to_char (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss')))";
		}
	      else if (real & DATA_TYPE_DATE)
		{
		  stream << "to_clob (shield.handle_zero_date (to_char (" << *inner << ", 'yyyy-mm-dd')))";
		}
	      else 
		{
		  stream << "to_clob (" << *inner << ")";
		}
	    }
	  else if (desired & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
	    {
	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  stream << *inner;
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "shield.to_number_ (" << *inner << ")";
		  
		}
	    }
	  else if (desired & DATA_TYPE_DATETIME)
	    {
	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  inner->set_skip_space (true);
		  stream << "shield.to_date_ (" << *inner << ", 'yyyymmddhh24miss')";
		}
	      else if (real & (DATA_TYPE_DATE | DATA_TYPE_DATETIME))
		{
		  stream << *inner;
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "shield.to_date_ (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss')";
		}
	    }
	  else if (desired & DATA_TYPE_DATE)
	    {
	      cerr << "My heart desires dates" << endl;

	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  inner->set_skip_space (true);
		  stream << "shield.to_date_ (" << *inner << ", 'yyyymmdd')";
		}
	      else if (real & (DATA_TYPE_DATETIME))
		{
		  stream << "shield.date_ (" << *inner << ")";
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "shield.to_date_ (" << *inner << ", 'yyyy-mm-dd')";
		}
	    }
	  else
	    {
	      throw shield::exception::syntax ("Unknown desired context: " + util::stringify(desired));
	    }	      
	}
      
    }

  }

}
