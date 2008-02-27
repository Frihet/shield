/**
   @file transform_create_index.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/cache.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    namespace 
    {
      
      void print_insert (ostream &stream, 
			 string table_name, 
			 string field_name,
			 string mangled_table_name,
			 string index_name)
      {
	
	stream << "insert into shield_table_column (table_name, column_name, "
	       << "column_type, mangled_table_name, mangled_column_name) values ("
	       << util::oracle_escape (table_name).first << ", "
	       << util::oracle_escape (field_name).first << ", " 
	       << util::oracle_escape ("DATA_TYPE_INDEX").first << ", "
	       << util::oracle_escape (mangled_table_name).first << "," 
	       << util::oracle_escape (index_name).first << ")\n\n" << sep;
      }
    }


    create_index::
    create_index ()
      : __type (PRIMARY_KEY), __table_query (0)
    {
    }
    
    void create_index::
    set_key_type (key_type type)
    {
      __type = type;
    }

    void create_index::
    set_name (printable *name)
    {
      _set_child (CHILD_NAME, name);
    }

    void create_index::
    set_key_list (chain *key_list)
    {
      _set_child (CHILD_KEY_LIST, key_list);
    }

    chain *
    create_index::get_filtered_key_list(chain *field_list)
    {
      chain *original_key_list;
      chain *key_list;

      chain::const_iterator i, j;

      bool ok;
      string key_name;

      field_spec *spec;
      
      string field_name;

      original_key_list = _get_child<chain> (CHILD_KEY_LIST);

      if (!original_key_list)
	throw exception::not_found (get_node_name () + ": Could not locate original key list");

      key_list = new chain ();
      key_list->set_separator (",");
      
      
      for (i=original_key_list->begin (); i < original_key_list->end (); i++ )
	{
	  ok = true;
	  key_name = (*i)->str ();

	  for (j=field_list->begin (); j < field_list->end (); j++ )
	    {
	      spec = dynamic_cast<field_spec *> (*j);
	      
	      if (!spec)
		{
		  throw exception::invalid_state ("Expected field list item of type field_spec *, got type " + (*j)->get_node_name () + "*");
		}
	      
	      field_name = spec->get_name ()->str ();
	      
	      if ((field_name == key_name) && (!spec->get_type ()->get_indexable ()))
		{
		  ok = false;
		  break;
		}

	    }

	  if (ok)
	    {
	      key_list->push (*i);
	    }
	}

      if (key_list->size () > 0)
	{
	  (*key_list)[0]->set_skip_space (true);
	}

      return key_list;
    }

    /*
      Compare two lists of index keys and return true if they are
      identical (contain the exact same list of elements)
    */
    static bool key_list_match (chain *key_list1, chain *key_list2)
    {
      chain::const_iterator i,j;

      string key1;
      string key2;
      bool match;

      if (key_list1->size () != key_list2->size ())
	{
	  return false;
	}

      for (i=key_list1->begin (); i<key_list1->end (); i++)
	{
	  key1 = (*i)->str ();
	  match = false;

	  for (j=key_list2->begin (); j<key_list2->end (); j++)
	    {
	      key2 = (*j)->str ();
	      if (key1 == key2)
		{
		  match = true;
		  break;
		}	  
	    }

	  if (!match)
	    {
	      return false;
	    }
	}

      return true;
    }

    void create_index::
    _print (ostream &stream)
    {

      chain *field_list;
      printable *table_name;
      chain *key_list;

      vector<printable *>::const_iterator pii;

      string f_name, t_name, name;

      printable *field_name;


      if (!_get_child (CHILD_KEY_LIST))
	throw shield::exception::not_found ("create_index node with no key list");
      
      if (!__table_query)
	throw shield::exception::not_found ("create_index::_print called without first calling internal_transform ()");

      field_list = __table_query->get_field_list ();
      
      table_name = __table_query->get_name ();
      
      if (!table_name)
	throw exception::invalid_state (get_node_name () + ": table name not set");
      
      key_list = get_filtered_key_list (field_list);
      
      if (!key_list->size ())
	{
	  /*
	    No indexable columns in this index Return.
	  */
	  return;
	}
      
      vector<printable *> &prev_index = __table_query->get_prev_index_list ();

      for (pii = prev_index.begin (); pii < prev_index.end (); pii++)
	{
	  create_index *other = dynamic_cast<create_index *> (*pii);

	  chain *other_key_list = other->get_filtered_key_list (field_list);
	  if (key_list_match (key_list, other_key_list))
	    {
	      return;
	    }
	}

      prev_index.push_back (this);
	
      
      if (__type == PRIMARY_KEY)
	{
	  
	  if (_get_child (CHILD_NAME))
	    {
	      throw exception::syntax ("Named primary keys are not supported!");
	    }

	  stream << "alter table" << *table_name << " add primary key (" << *key_list << ")";
	  stream  << endl << endl << sep;

	}
      else
	{
	  
	  if (!_get_child (CHILD_NAME))
	    {
	      throw exception::syntax ("Unnamed non-primary keys are not supported!");
	    }
	  
	  t_name = table_name->unmangled_str ();
	  field_name = _get_child (CHILD_NAME);
	  
	  if (!field_name)
	    {
	      throw exception::invalid_state ("Field for index is not of type text");
	    }

	  f_name = field_name->unmangled_str ();
	  name = t_name + "_" + f_name;
      
	  if (name.length () > 30)
	    {
	      name = t_name.substr(0, 15) + "_" + f_name;
	      name = name.substr(0, 30);
	    }

	  drop_item (stream, "index", name );
	  
	  stream << "create";

	  if (__type == UNIQUE)
	    stream << " unique";

	  stream << " index " << name << endl;
	  stream << "on" << *table_name << " (" << *key_list << ")";
	  stream  << endl << endl << sep;
	  
	  print_insert( stream, table_name->unmangled_str (), field_name->unmangled_str (), table_name->str (), name);
	  
	}
  
      cache::clear ();
    }


    printable *create_index::
    internal_transform (void)
    {
      __table_query = dynamic_cast<create_table *> (get_parent ()->get_query ());

      if (!__table_query)
	throw shield::exception::not_found ("create_index called with no parent create_table node");

      _add_query (this);
      return 0;
    }

  }

}
