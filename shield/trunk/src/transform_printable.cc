/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <sstream>

#include "include/transform.hh"
#include "include/catalyst.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

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
      :  __push_front (false), __context (DATA_TYPE_UNDEFINED), __skip_space (false), __parent (0)
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
      /*
      if (dynamic_cast <insert *> (this))
	{
	  debug << (string ("Set child of type ") + ENUM_TO_STRING (child_type, id) + " to value " + stringify (value));
	}
      */
      //      if (!value)
  //	throw exception::invalid_param ("Called set_child with null value");
	__children[id] = value;
      if (value)
	value->set_parent (this);
    }
    
    printable *printable::
    _get_child (child_type id)
    {

      map<int, printable *>::const_iterator i;
      i = __children.find (id);
      /*
      if (dynamic_cast <insert *> (this))
	{
	  debug << (string ("Get child of type ") + ENUM_TO_STRING (child_type, id) + " with value " + stringify (i == __children.end ()?0:i->second));
	}
      */
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
	      if (p)
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

    string printable::
    get_node_name ()
    {
      string full = util::cxx_demangle (typeid(*this).name ());
      int pos = full.rfind("::");
      if (pos != full.npos)
	{
	  full = full.substr (pos+2);
	}
      return full;
    }

    string printable::
    get_path (bool is_first)
    {
      string res;

      if (get_parent ())
	{
	  res = get_parent ()->get_path(false);
	}

      res += get_node_name ();
      if (!is_first)
	res += "/";
      return res;
    }

    string printable::
    get_tree (int level)
    {
      string res(level*2, ' ');
      res += get_node_name () + "\n";
      map<int, printable *>::const_iterator it;

      for (it=__children.begin (); it!=__children.end (); ++it)
	{
	  if (it->second)
	    res += it->second->get_tree (level+1);
	}

      return res;
    }
    
    void printable::
    _add_query (query *q)
    {
      chain *qchain;

      printable *p = this;
      while (p->get_parent ()->get_parent ())
	p = p->get_parent ();

      /*
	The resulting node of the above loop should be the child of the root node. This should be a chain, since the base of the tree should always look like this:
	
	fake_query
           |
           \--chain
                |
                \--query1
                |
                \--query2
                |
                \--...
      */

      qchain = dynamic_cast<chain *> (p);
      
      if (!qchain)
	throw exception::invalid_state ("Root query chain could not be found");

      qchain->push (q);
      q->set_parent (qchain);
      
    }

  }

}
