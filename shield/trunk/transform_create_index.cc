#include <assert.h>
#include "transform.hh"

namespace shield
{

namespace transform
{

chain *
create_index::get_filtered_key_list() const
{
  assert (__key_list);

  map<string, printable *>::const_iterator fld = __prop.find("field_list");
  assert (fld != __prop.end () );      
  chain *field_list = dynamic_cast<chain *> ((*fld).second);
  assert (field_list);

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
create_index::print (ostream &stream) const
{

  assert (__key_list);

  map<string, printable *>::const_iterator name = __prop.find("table_name");
  assert (name != __prop.end () );      
  printable *table_name = (*name).second;
  assert (table_name);

  chain *key_list = get_filtered_key_list ();

  if (key_list->size () == 0)
    {
      /*
	No indexable columns in this index Return.
      */
      return;
    }

  map<string, printable *>::const_iterator prev_index_iter = __prop.find("index");
  chain *prev_index = dynamic_cast<chain *> ((*prev_index_iter).second);

  if (prev_index)
    {
      chain::const_iterator pii;
      for (pii = prev_index->begin (); pii < prev_index->end (); pii++)
	{
	  create_index *other = dynamic_cast<create_index *> (*pii);
	  chain *other_key_list = other->get_filtered_key_list ();
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
	  throw exception::syntax_exception ("Named primary keys are not supported!");
	}

      stream << "alter table" << *table_name << " add primary key (" << *key_list << ")";

    }
  else
    {

      if (!__name)
	{
	  throw exception::syntax_exception ("Unnamed non-primary keys are not supported!");
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
  
  stream << ";" << endl << endl;

}

}

}
