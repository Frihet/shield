
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
      chain *list = _get_condensed_table_list ();

      //      cerr << "Looking for table for field " << field->str () << endl;

      chain::const_iterator i;
      for (i=list->begin (); i<list->end (); i++)
	{

	  text *t = dynamic_cast<text *> (*i);
	  identity *id = dynamic_cast<identity *> (*i);

	  if (id)
	    {
	      t = id->get_table ();
	    }

	  assert (t);

	  text *unaliased = unalias_table (t);

	  assert (unaliased);

	  cerr << "Looking in table " << unaliased->str () << endl;

	  introspection::table &table = introspection::get_table (unaliased->str ());

	  try
	    {
	      const introspection::column &col = table.get_column (field->str ());
	      
	      cerr << "Found it" << endl;
	      /*
		If that column didn't exist, an exception would have been thrown. We found it! Yay!

		Note that we use t here, not unaliased. 
	      */
	      return new text(t->str ());
	    }
	  catch (exception::not_found &e)
	    {
	      cerr << "Not there" << endl;
	    }
	}

      return 0;
    }

    chain *query::
    _get_condensed_table_list ()
    {
      throw exception::syntax ("Called get_condensed_table_list () on query which does not supply table list");
    }

  }

}
