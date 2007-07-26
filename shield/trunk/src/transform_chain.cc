/**
   @file transform_chain.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/


#include "include/transform.hh"
#include "include/catalyst.hh"
#include "include/exception.hh"

namespace shield
{

  namespace transform
  {

    chain::
    chain (printable *a, 
	   printable *b, 
	   printable *c, 
	   printable *d, 
	   printable *e, 
	   printable *f, 
	   printable *g, 
	   printable *h,
	   printable *i,
	   printable *j,
	   printable *k,
	   printable *l)
      : __separator (""), __do_line_break (false)
    {
	push (a);
	push (b);
	push (c);
	push (d);
	push (e);
	push (f);
	push (g);
	push (h);
	push (i);
	push (j);
	push (k);
	push (l);
    }

    void chain::
    _print (ostream &stream)
    {
      vector<printable *>::const_iterator it;

      for( it = __chain.begin (); it != __chain.end (); ++it )
	{
	  if (it != __chain.begin () )
	    {
	      stream << __separator;
	      if (__do_line_break && ((it-__chain.begin ()) %__do_line_break == 0))
		{
		  stream << endl;
		}
	    }

	  printable *item = *it;

	  if (!item)
	    throw exception::invalid_state ("Null element in chain");
	  stream << *item;

	}
    }


    printable *chain::
    transform (catalyst::catalyst &catalyst)
    {
      for (int i=0; i<__chain.size (); i++)
	{
	  printable *p = __chain[i];
	  printable *p2 = p->transform (catalyst);
	  if (p2 != p)
	    {
	      __chain[i] = p2;
	      if (!__chain[i])
		{
		  __chain.erase(begin ()+i, begin()+i+1);
		  i--;
		}
	      else
		{
		  __chain[i]->set_parent (this);
		}
	    }
	}
 
      return catalyst (this);
   }

    void chain::
    push (printable *p)
    {
      if (p)
	{

	  p->set_parent (this);

	  if (p->get_push_front () || __chain.empty ())
	    {
	      p->set_skip_space (get_skip_space ());
	    }

	  if (p->get_push_front () )
	    {
	      __chain.insert (__chain.begin (), p);
	    }
	  else
	    {
	      __chain.push_back (p);
	    }
	}
    }

    string chain::
    get_tree (int level)
    {
      string res(level*2, ' ');
      res += get_node_name () + "\n";
      const_iterator it;

      for (it=__chain.begin (); it!=__chain.end (); ++it)
	{
	  res += (*it)->get_tree (level+1);
	}

      return res;
    }
    
    data_type chain::
    get_context (void)
    {
      if (!size ())
	return DATA_TYPE_UNDEFINED;

      return __chain[0]->get_context ();
    }

  }

}
