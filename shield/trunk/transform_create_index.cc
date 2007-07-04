#include <assert.h>
#include "transform.hh"

namespace shield
{

  namespace transform
  {
    
      void create_index::
      set_key_type (key_type type)
      {
	__type = type;
      }

      void create_index::
      set_name (printable *name)
      {
	__name = name;
	if (__name)
	  __name->set_parent (this);
      }

      void create_index::
      set_key_list (chain *key_list)
      {
	__key_list = key_list;
	
	if (__key_list)
	  __key_list->set_parent (this);
      }


    chain *
    create_index::get_filtered_key_list(chain *field_list)
    {
      assert (__key_list);

      chain *key_list = new chain ();
      key_list->set_separator (",");

      chain::const_iterator i, j;

      for (i=__key_list->begin (); i < __key_list->end (); i++ )
	{
	  bool ok = true;

	  string key_name = (*i)->str ();

	  for (j=field_list->begin (); j < field_list->end (); j++ )
	    {
	      field_spec *s = dynamic_cast<field_spec *> (*j);
	      assert (s);

	      string field_name = s->get_name ()->str ();

	      if (field_name == key_name && !s->get_type ()->get_indexable ())
		{
		  ok = false;
		}

	    }

	  if (ok)
	    key_list->push (*i);
	}
  
      if (key_list->size () > 0)
	{
	  (*key_list)[0]->set_skip_space (true);
	}

      return key_list;
    }

    static bool key_list_match (chain *key_list1, chain *key_list2)
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

    void 
    create_index::print (ostream &stream)
    {

      assert (__key_list);      

      create_table *table_query = dynamic_cast<create_table *> (get_query ());
      assert (table_query);

      chain *field_list = table_query->get_field_list ();

      printable *table_name = table_query->get_name ();
      assert (table_name);

      chain *key_list = get_filtered_key_list (field_list);

      if (key_list->size () == 0)
	{
	  /*
	    No indexable columns in this index Return.
	  */
	  return;
	}

      chain *prev_index = table_query->get_prev_index_list ();

      if (prev_index)
	{
	  chain::const_iterator pii;
	  for (pii = prev_index->begin (); pii < prev_index->end (); pii++)
	    {
	      create_index *other = dynamic_cast<create_index *> (*pii);
	      chain *other_key_list = other->get_filtered_key_list (field_list);
	      if (key_list_match (key_list, other_key_list))
		{
		  return;
		}
	    }
	}

      if (__type == PRIMARY_KEY)
	{

	  if (__name)
	    {
	      throw exception::syntax ("Named primary keys are not supported!");
	    }

	  stream << "alter table" << *table_name << " add primary key (" << *key_list << ")";

	}
      else
	{

	  if (!__name)
	    {
	      throw exception::syntax ("Unnamed non-primary keys are not supported!");
	    }
      
	  string t_name = table_name->str ();
	  string f_name = __name->str ();
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
    transform (catalyst &catalyst)
    {
      if (__name)
	__name = __name->transform (catalyst);

      if (__key_list)
	__key_list = dynamic_cast<chain *> (__key_list->transform (catalyst));

      return catalyst (this);
    }

  }

}
