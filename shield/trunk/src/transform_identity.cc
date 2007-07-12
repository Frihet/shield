#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/exception.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    void identity::
    _print (ostream &stream)
    {
      bool printed = false;

      if (get_skip_space ())
	{
	  if (get_namespace () && get_namespace ()->length ())
	    get_namespace ()->set_skip_space (true);
	  else if (get_table () && get_table ()->length ())
	    get_table ()->set_skip_space (true);
	  if (get_field () && get_field ()->length ())
	    get_field ()->set_skip_space (true);
	}

      if (get_namespace () && get_namespace ()->length ())
	{
	  stream << *get_namespace ();
	  printed = true;
	}

      if (get_table () && get_table ()->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      get_table ()->set_skip_space (true);
	    }
	  printed = true;
	  stream << *get_table ();	
	}
    
      if (get_field () && get_field ()->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      get_field ()->set_skip_space (true);
	    }

	  stream << *get_field ();	
	}

    }

    text *identity::get_namespace ()
    {
      return _get_child<text> (CHILD_NAMESPACE);
    }

    text *identity::get_table ()
    {
      return _get_child<text> (CHILD_TABLE);
    }

    text *identity::get_field ()
    {
      return _get_child<text> (CHILD_FIELD);
    }

    data_type identity::
    get_context ()
    {

      query *q = get_query ();
      
      if (!q)
	{
	  cerr << "parent = " << get_parent ()->get_node_name () << endl;
	  throw exception::invalid_state ("No parent query in identity node " + get_path ());
	}

      text *table = get_table ();

      if (!table)
	{
	  table = q->get_table (get_field ());
	}

      if (!table)
	throw exception::invalid_state ("Could not introspect table in identity::get_context ()");

      text *real_table = q->unalias_table (table);
 
      introspection::table &itbl = introspection::get_table (real_table->str ());
      const introspection::column &ic = itbl.get_column (get_field ()->str ());
      const introspection::column_type &tp = ic.get_type ();
      
      return tp.get_type ();
      
    }

  }

}
