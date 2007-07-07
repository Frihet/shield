
#include <sstream>

#include "transform.hh"
#include "catalyst.hh"
#include "exception.hh"
#include "util.hh"

namespace shield
{

  namespace transform
  {

    namespace
    {
      vector<printable *>all_printable;
    }

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
      throw exception::syntax ("Tried to print node with no _print method");
    }

    void printable::
    set_parent (printable *parent)
    {
      if (!parent)
	throw exception::invalid_state ("Tried to reparent a node to a null pointer");

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
    _set_child (child_type id, printable *value)
    {
      __children[id] = value;
      if (value)
	value->set_parent (this);
    }
    
    printable *printable::
    _get_child (child_type id)
    {

      map<int, printable *>::const_iterator i;
      i = __children.find (id);

      if (i == __children.end ())
	return 0;

      return i->second;
      
    }

    printable *printable::
    transform (catalyst::catalyst &catalyst)
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

    string printable::
    str (void)
    {
      std::ostringstream out;
      if (!(out << *this))
	throw shield::exception::syntax ("stringify called on invalid type");
      return util::trim (out.str());
    }

    void printable::invalid_type (const string &what, const string &type)
    {
      throw exception::invalid_type (what, type);
    }

    
  }

}
