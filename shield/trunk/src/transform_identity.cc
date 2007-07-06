#include "transform.hh"
#include "introspection.hh"
#include "exception.hh"

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
      return dynamic_cast<text *> (_get_child (__NAMESPACE));
    }

    text *identity::get_table ()
    {
      return dynamic_cast<text *> (_get_child (__TABLE));
    }

    text *identity::get_field ()
    {
      return dynamic_cast<text *> (_get_child (__FIELD));
    }

    context identity::
    get_context ()
    {
      //      cerr << "get_context on identity "<< str () << endl;

      query *q = get_query ();

      text *table = get_table ();

      if (!table)
	{
	  table = q->get_table (get_field ());
	}

      assert (table);

      text *real_table = q->unalias_table (table);
 
      //      cerr << "Table " << get_table ()->str () << " unaliased to " << real_table->str () << endl;
     
      //      cerr << "before introspection of table " << real_table->str ()<< endl;
      introspection::table &itbl = introspection::get_table (real_table->str ());
      const introspection::column &ic = itbl.get_column (get_field ()->str ());
      const introspection::column_type &tp = ic.get_type ();
      //      cerr << "after introspection" << endl;

      if (tp.is_char ())
	return CONTEXT_STRING;

      if (tp.is_number ())
	return CONTEXT_NUMBER;

      if (tp.is_lob ())
	return CONTEXT_LOB;

      throw exception::syntax ("Unknown column type in column " + this->str ());
      
    }

  }

}
