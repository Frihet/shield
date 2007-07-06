
#include "transform.hh"
#include "exception.hh"
#include "catalyst.hh"

namespace shield
{

  namespace catalyst
  {

    transform::printable *identity::
    operator () (transform::printable *p)
    {
      
      transform::text * t = dynamic_cast<transform::text *> (p);

      if (!t)
	{
	  return p;
	}

      if (t->get_type () != transform::IDENTIFIER &&
	  t->get_type () != transform::IDENTIFIER_QUOTED)
	{
	  return p;
	}


      transform::printable *parent = t->get_parent ();
      bool parent_is_id = dynamic_cast<identity *> (parent) != 0;

      if (parent_is_id)
	{
	  return p;
	}

      if (p->get_query () != __query)
	return p;

      transform::text *table = p->get_query ()->get_table (t);

      if (!table)
	{
	  debug << ("Could not find corresponding table for identity " + p->str () + "!!");
	  return p;
	}
      
      transform::identity *res = new transform::identity (0, 0, t);
      res->set_parent (parent);
      
      return res;
    }

  }

}
