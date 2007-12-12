/**
   @file catalyst_find_table.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

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
      transform::printable *print;

      if (a)
	{
	  if (a->get_alias ())
	    {
	      __res.push_back (new transform::text (a->get_alias ()));
	      debug << (string ("Found table alias ") + a->get_alias ()->str ());
	    }
	  else
	    {
	      print = a->get_item ();
	      if (transform::as_text (print, 1))
		{
		  if (print->get_query () == __query)
		    {
		      __res.push_back (print);
		      debug << (string ("Found table ") + print->str ());
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

