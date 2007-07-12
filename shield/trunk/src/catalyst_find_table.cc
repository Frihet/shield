#include "include/transform.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace catalyst
  {

    find_table::
    find_table (transform::query *q)
      : __query (q)
    {
    }

    transform::printable *find_table::
    catalyze (transform::printable *p)
    {
      transform::printable_alias *a = dynamic_cast<transform::printable_alias *> (p);

      if (a)
	{
	  if (a->get_alias ())
	    {
	      __res.push_back (a->get_alias ());
	    }
	  else
	    {
	      transform::printable *print = a->get_item ();
	      transform::text *txt = dynamic_cast<transform::text *> (print);
	      transform::identity *id = dynamic_cast<transform::identity *> (print);
	      if (txt || id)
		{
		  if (print->get_query () == __query)
		    {
		      __res.push_back (print);
		    }
		}
	    }
	}
      else
	{
	  transform::select *sel = dynamic_cast<transform::select *> (p);
	  if (sel)
	    {
	      find_table c2 (sel);
	      sel->get_table_list ()->transform (c2);
	      __res.insert(__res.end (), c2.__res.begin (), c2.__res.end ());
	    }
	}

      return p;
    }

  }

}

