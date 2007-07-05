
#include "transform.hh"

namespace shield
{

  namespace transform
  {

    static vector<printable *>all_printable;

    void printable_delete()
    {
      vector<printable *>::const_iterator i;

      for (i=all_printable.begin (); i<all_printable.end (); i++ )
	{
	  delete( *i);
	}
      all_printable.clear();
    }


    printable::
    printable()
      :  __push_front (false), __context (CONTEXT_UNDEFINED), __skip_space (false), __parent (0)
    {
      all_printable.push_back (this);
    }


    void printable::
    _print (ostream &stream)
    {
      throw exception::syntax ("Tried to print object of type printable");
    }

    void printable::
    set_parent (printable *parent)
    {
      assert (parent);
      __parent = parent;
    }

    query *printable::
    get_query ()
    {
      if (__parent)
	return __parent->get_query ();

      return 0;
    }

    ostream &operator << (ostream &stream, printable &p)
    {
      p._print (stream);
      return stream;
    }

    void printable::
    _set_child (int id, printable *value)
    {
      __children[id] = value;
      if (value)
	value->set_parent (this);
    }
    
    printable *printable::
    _get_child (int id)
    {

      map<int, printable *>::const_iterator i;
      i = __children.find (id);

      if (i == __children.end ())
	return 0;

      return i->second;
      
    }

    printable *printable::
    transform (catalyst &catalyst)
    {
      
      map<int, printable *>::iterator i;
      
      for (i=__children.begin (); i!=__children.end (); ++i)
	{
	  printable *p = i->second;

	  if (p)
	    {	  
	      p = p->transform (catalyst);
	      p->set_parent (this);
	      i->second = p;
	    }
	}
      
      return catalyst (this);
    }
    
  }

}
