
#include "transform.hh"
#include "introspection.hh"
#include "exception.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    query::
    query ()
    {
    }

    query *query::
    get_query ()
    {
      return this;
    }

    text *query::
    unalias_table (text *table)
    {
      return table;
    }

    text *query::
    get_table (text *field)
    {
      _make_condensed_table_list ();
      

      vector<printable *> list = _condensed_table_list;
      
      vector<printable *>::const_iterator i;
      for (i=list.begin (); i<list.end (); i++)
	{
	  
	  if (!*i)
	    throw exception::invalid_state ("Null item in _condensed_table_list");
	  
	  text *t = dynamic_cast<text *> (*i);
	  identity *id = dynamic_cast<identity *> (*i);
	  
	  if (id)
	    {
	      t = id->get_table ();
	    }

	  if (!t)
	    throw exception::invalid_state (string ("Item of unknown type in _condensed_table_list: ") + (*i)->get_node_name ());

	  text *unaliased = unalias_table (t);
	  
	  if (!unaliased)
	    throw exception::invalid_state ( string ("Could not unalias item ") + t->str ());

	  introspection::table &table = introspection::get_table (unaliased->str ());

	  try
	    {
	      const introspection::column &col = table.get_column (field->str ());
	      
	      /*
		If that column didn't exist, an exception would have been thrown. We found it! Yay!

		Note that we use \c t here, not \c unaliased. 
	      */
	      return new text(t->str ());
	    }
	  catch (exception::not_found &e)
	    {
	    }
	}

      return 0;
    }

    void query::
    _make_condensed_table_list ()
    {
      throw exception::syntax ("Called make_condensed_table_list () on query which does not supply table list");
    }

  }

}
