
#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    printable *identity_catalyst::
    operator () (printable *p)
    {

      //cerr << "Identity transform on element << " << p->str () << endl;

      text * t = dynamic_cast<text *> (p);

      if (!t)
	{
	  return p;
	}

      if (t->get_type () != IDENTIFIER &&
	  t->get_type () != IDENTIFIER_QUOTED)
	{
	  return p;
	}

      printable *parent = t->get_parent ();
      bool parent_is_id = dynamic_cast<identity *> (parent) != 0;

      if (parent_is_id)
	{
	  return p;
	}

      text *table = p->get_query ()->get_table (t);

      if (!table)
	{
	  cerr << "Could not find corresponding table for identity " << p->str () << "!!" << endl;
	  return p;
	}

      identity *res = new identity (0, table, t);
      res->set_parent (parent);

      return res;
    }

  }

}
