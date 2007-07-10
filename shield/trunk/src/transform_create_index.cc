#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    create_index::
    create_index ()
      : __table_query (0), __type (PRIMARY_KEY)
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
      chain *original_key_list = _get_child<chain> (CHILD_KEY_LIST);
      
      if (!original_key_list)
	throw exception::not_found (get_node_name () + ": Could not locate original key list");

      chain *key_list = new chain ();
      key_list->set_separator (",");
      
      chain::const_iterator i, j;
      
      for (i=original_key_list->begin (); i < original_key_list->end (); i++ )
	{
	  bool ok = true;

	  string key_name = (*i)->str ();

	  for (j=field_list->begin (); j < field_list->end (); j++ )
	    {
	      field_spec *s = dynamic_cast<field_spec *> (*j);
	      
	      if (!s)
		{
		  throw exception::invalid_state ("Expected field list item of type field_spec *, got type " + (*j)->get_node_name () + "*");
		}
	      
	      string field_name = s->get_name ()->str ();
	      
	      if ((field_name == key_name) && (!s->get_type ()->get_indexable ()))
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

    
    bool key_list_match (chain *key_list1, chain *key_list2)
    {
      chain::const_iterator i,j;

      if (key_list1->size () != key_list2->size ())
	{
	  return false;
	}

      for (i=key_list1->begin (); i<key_list1->end (); i++)
	{
	  string k1 = (*i)->str ();
	  bool match = false;
	  for (j=key_list2->begin (); j<key_list2->end (); j++)
	    {
	      string k2 = (*j)->str ();
	      if (k1 == k2)
		{
		  match = true;
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

      if (!_get_child (CHILD_KEY_LIST))
	throw shield::exception::not_found ("create_index node with no key list");
      
      if (!__table_query)
	throw shield::exception::not_found ("create_index::_print called without first calling internal_transform ()");

      chain *field_list = __table_query->get_field_list ();
      
      printable *table_name = __table_query->get_name ();
      if (!table_name)
	throw exception::invalid_state (get_node_name () + ": table name not set");
      
      chain *key_list = get_filtered_key_list (field_list);
      
      if (!key_list->size ())
	{
	  /*
	    No indexable columns in this index Return.
	  */
	  return;
	}
      
      vector<printable *> &prev_index = __table_query->get_prev_index_list ();
      
      vector<printable *>::const_iterator pii;
      for (pii = prev_index.begin (); pii < prev_index.end (); pii++)
	{
	  create_index *other = dynamic_cast<create_index *> (*pii);
	  chain *other_key_list = other->get_filtered_key_list (field_list);
	  if (key_list_match (key_list, other_key_list))
	    {
	      return;
	    }
	}
	
      
      if (__type == PRIMARY_KEY)
	{
	  
	  if (_get_child (CHILD_NAME))
	    {
	      throw exception::syntax ("Named primary keys are not supported!");
	    }

	  stream << "alter table" << *table_name << " add primary key (" << *key_list << ")";

	}
      else
	{
	  
	  if (!_get_child (CHILD_NAME))
	    {
	      throw exception::syntax ("Unnamed non-primary keys are not supported!");
	    }
      
	  string t_name = table_name->str ();
	  string f_name = _get_child (CHILD_NAME)->str ();
	  string name = t_name + "_" + f_name;
      
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
	}
  
      stream  << endl << endl;

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
