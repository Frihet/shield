/**
   @file transform_insert.cc

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

using namespace shield;

namespace shield
{

  namespace transform
  {

    using namespace util;

    insert::
    insert()
      : __ignore (false)
    {
    }

    void 
    insert::set_ignore (bool i)
    {
      __ignore = i;
    }
    
    void 
    insert::set_field_list (chain *l)
    {
      _set_child (CHILD_FIELD_LIST, l);
    }

    void 
    insert::set_value_list (chain *l)
    {
      _set_child (CHILD_VALUE_LIST, l);
    }

    void 
    insert::set_name (identity *l)
    {
      _set_child (CHILD_NAME, l);
    }

    void 
    insert::set_insert_update (printable *l)
    {
      _set_child (CHILD_INSERT_UPDATE, l);
    }

    /**
       Convert an foo=bar, baz=qux style list to a standard sql (foo, baz)
       and (bar, qux) pair of lists.
    */
    void 
    insert::set_eq_list (chain *l)
    {
      chain *col = new chain();
      chain *val = new chain();

      col->set_separator (",");
      val->set_separator (",");

      chain::const_iterator i;  

      for (i=l->begin (); i<l->end (); i++)
	{
	  printable *p = *i;

	  chain *c = dynamic_cast<chain *> (p);
	  col->push ((*c)[0]);
	  val->push ((*c)[1]);
	}  
      set_field_list (col);
      set_value_list (new chain (val));
    }

    chain *insert::
    get_field_list ()
    {
      return _get_child<chain> (CHILD_FIELD_LIST);
    }

    chain *insert::
    get_value_list ()
    {
      return _get_child<chain> (CHILD_VALUE_LIST);
    }

    printable *insert::
    get_insert_update ()
    {
      return _get_child (CHILD_INSERT_UPDATE);
    }

    identity *insert::
    get_name ()
    {
      return _get_child<identity> (CHILD_NAME);
    }

    void insert::
    _print (ostream &stream)
    {
      if (!get_name () || !get_value_list ())
	{
	  throw exception::invalid_state (get_node_name () + ": name or value list not set when calling _print ()");
	}
      /*
	This code fills out the field list if not given.

	Needed since we need to know what type every column has
      */
      if (!get_field_list () )
	{
	  chain *fl = new chain ();
	  fl->set_separator (",");

	  using namespace introspection;

	  debug << (string ("Field list for table ")+ get_name ()->str ()+ " is empty, filling it in!");
	  
	  table &t = introspection::get_table (get_name ()->unmangled_str ());
	  table::column_const_iterator i;
	  for (i=t.column_begin (); i<t.column_end (); i++)
	    {
	      string n = (*i).get_name ();
	      debug << (string ("Add field item ")+ n);
	      text *fld = new text (n, IDENTIFIER);
	      identity *id = new identity (0, 0, fld);
	      fl->push (id);
	    }
	  
	    set_field_list (fl);
	  }
      

      chain::const_iterator i;  


      for (i=get_value_list ()->begin (); i<get_value_list ()->end (); i++)
	{

	  stream << "insert into" << *get_name () << endl;
	  get_field_list ()->set_skip_space (true);
	  stream << "(" << *get_field_list () << ")" << endl;
	  
	  (*i)->set_skip_space (true);

	  introspection::table &itable = introspection::get_table (get_name ()->unmangled_str ());
	  
	  stream << "values \n(";

	  chain *field_list = dynamic_cast<chain *> (get_field_list ());
	  chain *value = dynamic_cast<chain *> (*i);

	  if (field_list->size () != value->size ())
	    {
	      throw exception::invalid_state (string ("Field list has ") + stringify (field_list->size ()) + " items, but value list has " + stringify (value->size ()));
	    }

	  for (int j=0; j<value->size (); j++)
	    {
	      text *column_name = dynamic_cast<text *> ((*field_list)[j]);
	      identity *column_name_id = dynamic_cast<identity *> ((*field_list)[j]);

	      if (column_name_id)
		{
		  column_name = column_name_id->get_field ();
		}

	      if (!column_name)
		{
		  throw exception::invalid_state ("Item in field list was not of type text or identity");
		}
	      
	      const introspection::column icolumn = itable.get_column (column_name->unmangled_str ());
	      
	      if (j)
		stream << ",\n";

	      stream << "\t";

	      cast *c =new cast ((*value)[j], icolumn.get_type ().get_type ());
	      c->set_parent (this);
	      c->set_skip_space (true);
	      stream << *c;
	    }

	  stream << ")"; 
	  stream << endl << endl << sep;
	}

    }

    void insert::
    _make_condensed_table_list ()
    {
      _condensed_table_list.clear ();
      _condensed_table_list.push_back (get_name ());
    }

  }

}
