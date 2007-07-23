/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/catalyst.hh"
#include "include/exception.hh"
#include "include/introspection.hh"
#include "include/enum_char.hh"
#include "include/util.hh"
#include "include/cache.hh"

namespace shield
{

  namespace transform
  {

    create_table::create_table()
      : __check (false)
    {
    }

    void 
    create_table::set_check_existance (bool check)
    {
      __check = check; 
    }

    void create_table::
    _print (ostream &stream)
    {
      if (!get_name () || (!get_field_list () && !get_like_clause ()))
	throw exception::invalid_state ("Required child nodes unset in create table node");
  
      if (__check)
	{
	  introspection::table &t = introspection::get_table (get_name ()->unmangled_str ());
	  if (t.exists ())
	    return;
	}

      stream << "create table" << *get_name () << endl;
      
      if (get_like_clause ())
	{
	  stream << "like" << *get_like_clause ();
	}
      else
	{
	  stream << "(" << *get_field_list () << " )" ;
	}
      
      stream << endl << endl << sep;
      introspection::clear_table (get_name ()->unmangled_str ());
      cache::clear ();
    }

    printable *create_table::
    internal_transform ()
    {
      create_lookup_entries();
      return this;
    }

    void create_table::
    create_lookup_entries()
    {
      using util::oracle_escape;

      chain *field_list = get_field_list ();
      if (field_list)
	{
	  chain::const_iterator it;
	  for (it=field_list->begin (); it!=field_list->end (); ++it)
	    {
	      printable *node = *it;
	      field_spec *spec = dynamic_cast<field_spec *> (node);
	      
	      if (!spec)
		throw exception::invalid_state ("Field spec for create table contains non-field_spec type node " + node->get_node_name ());
	      
	      string str = "insert into shield_table_column (table_name, column_name, column_type, mangled_table_name, mangled_column_name) values (";
	      
	      str += oracle_escape (get_name ()->unmangled_str ())+", "
		+ oracle_escape (spec->get_name ()->unmangled_str ()) + ", " 
		+ oracle_escape (ENUM_TO_STRING (data_type, spec->get_type ()->get_type ())) + ", "
		+ oracle_escape (get_name ()->str ()) + "," 
		+ oracle_escape (spec->get_name ()->str ()) 
		+ ")\n\n";
	      
	      str += sep;
	      _add_query (new fake_query (new text (str, EXACT, false)));
	      
	    }
	}
      else
	{
	  throw exception::unsupported ("Create table ... like-queries are not supported");
	}
    }

  }

}
