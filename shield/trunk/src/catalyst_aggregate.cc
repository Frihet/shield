
/**
   @file catalyst_aggregate.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/catalyst.hh"
#include "include/introspection.hh"
#include "include/exception.hh"

namespace shield
{

  namespace catalyst
  {

    using transform::identity;
    using transform::printable;
    using transform::text;
    using transform::function;
    using transform::paran;
    using transform::query;

    aggregate::
    aggregate (query *q, const vector<identity *> &group_field)
      : __query (q), __group_field (group_field)
    {
    }


    printable *aggregate::
    catalyze (printable *node)
    {
      identity *id = dynamic_cast<identity *> (node);
      
      printable *parent;
      function *func;
      text *table_name;
      vector<identity *>::const_iterator it;

      /*
	Check that this is a field
      */
      if (!id)
	{
	  return node;
	}

      /*
	Check if this field is aggregated using a function
      */
      for (parent = node->get_parent (); parent; parent = parent->get_parent ())
	{
	  func = dynamic_cast<function *> (parent);
	  if (func && func->get_aggregate ())
	    {
	      return node;
	    }
	}

      /*
	Check if this field is part of the 'group by'
      */
      
      for (it = __group_field.begin (); it != __group_field.end (); ++it)
	{
	  /*
	    FIXME:

	    Are we positive that both identifiers are in a canonical
	    form so a simple string comparison is enough?
	  */
	  if ((*it)->str () == node->str ())
	    {
	      return node;
	    }
	}

      if (id->get_namespace ())
	{
	  throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. I'm lazy. Sorry.");
	}
      
      if (id->get_table ())
	{
	  table_name = id->get_table ();
	}
      else
	{
	  table_name = __query->get_table (id->get_field ());
	}

      return __aggregate (id->get_field (), table_name);
    }

      /**
	 This function uses introspection to find out what type a table
	 column is of, and use the correct version of the arbitrary
	 aggregation functions defined toghether with the shield
	 package.
      */
      
      printable *aggregate::
      __aggregate (text *field, 
		   text *table_alias)
      {
	string func_name="";
	string field_name;
      
	text *table = __query->unalias_table (table_alias);

	introspection::table &t = introspection::get_table (table->unmangled_str ());
	field_name = field->unmangled_str ();
	const introspection::column &c = t.get_column (field_name);
      	introspection::column_type y = c.get_type ();

	if (y.is_char () )
	  {
	    func_name = "shield_arb_agg_char";
	  }
	else if (y.is_lob ())
	  {
	    func_name = "shield_arb_agg_clob";
	  } 
	else if (y.is_number ())
	  {
	    func_name = "shield_arb_agg_num";
	  }
	else if (y.is_date ())
	  {
	    func_name = "shield_arb_agg_date";
	  }
	else
	  {
	    throw exception::invalid_state (string ("Don't know how to aggregate column ") + field->unmangled_str ());
	  }
	
	return new function (new text (func_name), y.get_type (), new identity (0, table_alias, field), true);
      }

  }
}
