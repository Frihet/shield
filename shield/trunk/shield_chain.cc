
#include "shield.hh"

namespace shield
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
  if (a)
    __chain.push_back (a);
    
  if (b)
    __chain.push_back (b);
    
  if (c)
    __chain.push_back (c);
    
  if (d)
    __chain.push_back (d);
    
  if (e)
    __chain.push_back (e);
    
  if (f)
    __chain.push_back (f);
    
  if (g)
    __chain.push_back (g);
    
  if (h)
    __chain.push_back (h);

  if (i)
    __chain.push_back (i);

  if (j)
    __chain.push_back (j);

  if (k)
    __chain.push_back (k);

  if (l)
    __chain.push_back (l);

}


void chain::
print (ostream &stream) const
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


const printable *chain::
transform (catalyst &catalyst) const
{
  chain *res = construct ();
  res->set_separator (__separator);
  const_iterator i;
  
  for (i=begin (); i<end (); i++)
    {
      res->push (const_cast<printable *> ((*i)->transform (catalyst)));
    }
  return catalyst (res);
}

void chain::
push (printable *p)
{
  if (p)
    {

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
