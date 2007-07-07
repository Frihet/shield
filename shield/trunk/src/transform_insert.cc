
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
      : __ignore (false)
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
      _set_child (CHILD_FIELD_LIST, l);
    }

    void 
    insert::set_value_list (chain *l)
    {
      _set_child (CHILD_VALUE_LIST, l);
    }

    void 
    insert::set_name (printable *l)
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

    printable *insert::
    get_field_list ()
    {
      return _get_child (CHILD_FIELD_LIST);
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

    printable *insert::
    get_name ()
    {
      return _get_child (CHILD_NAME);
    }

    void insert::
    _print (ostream &stream)
    {
      assert (get_name ());
      assert (get_value_list ());

      /*
	This code fills out the field list if not given. For a while,
	I though this might be needed for some types of inserts, but
	it seems I was wrong. I have not removed the code though,
	since it may be useful in the future.
      */
      /*
      if (!get_field_list ())
	{
	  chain *fl = new chain ();
	  fl->set_separator (",");

	  using namespace introspection;
	  
	  table &t = introspection::get_table (get_name ()->str ());
	  table::column_const_iterator i;
	  for (i=t.column_begin (); i<t.column_end (); i++)
	    {
	      string n = (*i).get_name ();
	      text *fld = new text (n);
	      identity *id = new identity (0, 0, fld);
	      fl->push (id);
	    }
	  
	    set_field_list (fl);
	  }
      */

      chain::const_iterator i;  

      for (i=get_value_list ()->begin (); i<get_value_list ()->end (); i++)
	{

	  if (i!=get_value_list ()->begin ())
	    stream << sep;

	  stream << "insert into" << *get_name () << endl;
	  if (get_field_list ())
	    {
	      get_field_list ()->set_skip_space (true);
	      stream << "(" << *get_field_list () << ")" << endl;
	    }

	  (*i)->set_skip_space (true);
	  stream << "values (" << *(*i) << ")";
	  stream << endl << endl;
	}

    }

    chain *insert::
    _get_condensed_table_list ()
    {
      return new chain (get_name ());
    }

  }

}
