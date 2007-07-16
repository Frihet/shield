/**

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

  namespace catalyst
  {

    transform::printable *validation::
    catalyze (transform::printable *p)
      {
	/*
	  Check that we have a valid node
	*/
	if (!p)
	  throw exception::not_found ("Main item in validation pass");
	
	transform::query *q = dynamic_cast<transform::query *> (p);
	
	if (!q)
	{
	  /*
	    Only queries may be a root
	  */
	  if (!p->get_parent ())
	    throw exception::not_found ("Parent item in validation pass");

	  /*
	    Check for short infinite loops. We should really check for
	    arbitrarily long loops as well, but in practice they don't
	    happen.
	  */
	  if (!p->get_parent ())
	    throw exception::not_found ("Parent item in validation pass");
	  
	  if (p == p->get_parent ())
	    throw exception::invalid_state ("Item is it's own parent in validation pass");
	  
	}
	

	/**
	   Check that the environment is healthy
	*/
	if (!p->get_query ())
	  {

	    transform::printable *i=p;
	    while (i)
	      {
		cerr << typeid(*i).name () << endl;
		//cerr << *i << endl;
		i=i->get_parent ();
	      }

	    throw exception::invalid_state ("query-less node found");
	  }
	
	return p;
      }

  }

}

