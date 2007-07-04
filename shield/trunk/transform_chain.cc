
#include "transform.hh"

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
    print (ostream &stream)
    {
      vector<printable *>::const_iterator i;

      for( i = __chain.begin (); i < __chain.end (); i++ )
	{
	  if (i != __chain.begin () )
	    {
	      stream << __separator;
	      if (__do_line_break && ((i-__chain.begin ()) %__do_line_break == 0))
		{
		  stream << endl;
		}
	    }

	  printable *it = *i;

	  assert (it);
	  stream << *it;

	}
    }


    printable *chain::
    transform (catalyst &catalyst)
    {

      for (int i=0; i<__chain.size (); i++)
	{
	  __chain[i] = __chain[i]->transform (catalyst);
	  assert (__chain[i]);
	  __chain[i]->set_parent (this);
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

  }

}
