#include "transform.hh"
#include "introspection.hh"
#include "exception.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    void identity::
    print (ostream &stream)
    {
      bool printed = false;

      if (get_skip_space ())
	{
	  if (__namespace && __namespace->length ())
	    __namespace->set_skip_space (true);
	  else if (__table && __table->length ())
	    __table->set_skip_space (true);
	  if (__field && __field->length ())
	    __field->set_skip_space (true);
	}

      if (__namespace && __namespace->length ())
	{
	  stream << *__namespace;
	  printed = true;
	}

      if (__table && __table->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      __table->set_skip_space (true);
	    }
	  printed = true;
	  stream << *__table;	
	}
    
      if (__field && __field->length ())
	{
	  if (printed)
	    {
	      stream << ".";
	      __field->set_skip_space (true);
	    }

	  stream << *__field;	
	}

    }

    text *identity::get_namespace ()
    {
      return __namespace;
    }

    text *identity::get_table ()
    {
      return __table;
    }

    text *identity::get_field ()
    {
      return __field;
    }

    printable *identity::
    transform (catalyst &catalyst)
    {
      
      if (__namespace)
	__namespace = dynamic_cast<text *> (__namespace->transform (catalyst));

      if (__table)
	__table = dynamic_cast<text *> (__table->transform (catalyst));

      if (__field)
	__field = dynamic_cast<text *> (__field->transform (catalyst));

      return catalyst (this);
    }

    context identity::
    get_context ()
    {
      //      cerr << "get_context on identity "<< str () << endl;

      query *q = get_environment ()->get_query ();

      if (!__table)
	{
	  __table = q->get_table (__field);
	}

      assert (__table);

      text *real_table = q->unalias_table (__table);
 
      //      cerr << "Table " << __table->str () << " unaliased to " << real_table->str () << endl;
     
      //      cerr << "before introspection of table " << real_table->str ()<< endl;
      introspection::table &itbl = introspection::get_table (real_table->str ());
      const introspection::column &ic = itbl.get_column (__field->str ());
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
