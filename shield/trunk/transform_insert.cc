
#include <assert.h>

#include "transform.hh"
#include "introspection.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    insert::
    insert()
      : __field_list (0), 
	__value_list (0), 
	__insert_update (0),
	__ignore (false),
	__name (0)
    {
    }

    void 
    insert::set_ignore (bool i)
    {
      __ignore = i;
    }

    void 
    insert::set_field_list (printable *l)
    {
      __field_list = l;
      if (l)
	l->set_parent (this);
    }

    void 
    insert::set_value_list (chain *l)
    {
      __value_list = l;
      if (l)
	l->set_parent (this);
    }

    void 
    insert::set_name (printable *l)
    {
      __name = l;
      if (l)
	l->set_parent (this);
    }

    void 
    insert::set_insert_update (printable *l)
    {
      __insert_update = l;
      if (l)
	l->set_parent (this);
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

    void insert::
    print (ostream &stream)
    {
      assert (__name);
      assert (__value_list);

      /*
	This code fills out the field list if not given. For a while,
	I though this might be needed for some types of inserts, but
	it seems I was wrong. I have not removed the code though,
	since it may be useful in the future.
      */
      /*
      if (!__field_list)
	{
	  chain *fl = new chain ();
	  fl->set_separator (",");

	  using namespace introspection;
	  
	  table &t = introspection::get_table (__name->str ());
	  table::column_const_iterator i;
	  for (i=t.column_begin (); i<t.column_end (); i++)
	    {
	      string n = (*i).get_name ();
	      text *fld = new text (n);
	      identity *id = new identity (0, 0, fld);
	      fl->push (id);
	    }
	  
	  __field_list = fl;
	}
      */

      chain::const_iterator i;  

      for (i=__value_list->begin (); i<__value_list->end (); i++)
	{
	  stream << "insert into" << *__name << endl;
	  if (__field_list)
	    {
	      __field_list->set_skip_space (true);
	      stream << "(" << *__field_list << ")" << endl;
	    }

	  (*i)->set_skip_space (true);
	  stream << "values (" << *(*i) << ")";
	  stream << endl << endl;
	}

    }

    chain *insert::
    get_condensed_table_list ()
    {
      return new chain (__name);
    }

  }

}
