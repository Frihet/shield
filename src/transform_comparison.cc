/**
   @file transform_comparison.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

    typedef pair<data_type,data_type> pair_ctx;

    namespace 
    {

      const data_type mapping[][3] =
	{
	  /*
	    Clobs always need to be converted, since they can't be compared...
	  */
	  {DATA_TYPE_CLOB, DATA_TYPE_CLOB, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_NUMBER, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_FLOAT, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_CHAR, DATA_TYPE_CHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_VARCHAR, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_DATE, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_DATETIME, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_CLOB, DATA_TYPE_UNDEFINED, DATA_TYPE_VARCHAR },
	  
	  /*
	    Upconvert numbers to strings, except when comparing dates
	  */
	  {DATA_TYPE_NUMBER, DATA_TYPE_DATE, DATA_TYPE_NUMBER },
	  {DATA_TYPE_NUMBER, DATA_TYPE_DATETIME, DATA_TYPE_NUMBER },
	  {DATA_TYPE_NUMBER, DATA_TYPE_CHAR, DATA_TYPE_CHAR },
	  {DATA_TYPE_NUMBER, DATA_TYPE_VARCHAR, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_NUMBER, DATA_TYPE_UNDEFINED, DATA_TYPE_VARCHAR },
	  
	  {DATA_TYPE_FLOAT, DATA_TYPE_DATE, DATA_TYPE_NUMBER },
	  {DATA_TYPE_FLOAT, DATA_TYPE_DATETIME, DATA_TYPE_NUMBER },
	  {DATA_TYPE_FLOAT, DATA_TYPE_CHAR, DATA_TYPE_CHAR },
	  {DATA_TYPE_FLOAT, DATA_TYPE_VARCHAR, DATA_TYPE_VARCHAR },
	  {DATA_TYPE_FLOAT, DATA_TYPE_UNDEFINED, DATA_TYPE_VARCHAR },
	  
	  /*
	    Convert dates to strings when comparing
	  */
	  {DATA_TYPE_CHAR, DATA_TYPE_DATE, DATA_TYPE_CHAR },
	  {DATA_TYPE_CHAR, DATA_TYPE_DATETIME, DATA_TYPE_CHAR },
	  {DATA_TYPE_CHAR, DATA_TYPE_UNDEFINED, DATA_TYPE_CHAR },
	  
	  {DATA_TYPE_VARCHAR, DATA_TYPE_DATE, DATA_TYPE_CHAR },
	  {DATA_TYPE_VARCHAR, DATA_TYPE_DATETIME, DATA_TYPE_CHAR },
	  {DATA_TYPE_VARCHAR, DATA_TYPE_UNDEFINED, DATA_TYPE_VARCHAR },
	  
	  /*
	    Convert dates to datetimes when comapring. This should
	    really not be needed. Ok to remove?
	  */
	  {DATA_TYPE_DATE, DATA_TYPE_DATETIME, DATA_TYPE_DATE },
	  {DATA_TYPE_DATE, DATA_TYPE_UNDEFINED, DATA_TYPE_DATE },
	  
	  {DATA_TYPE_DATETIME, DATA_TYPE_UNDEFINED, DATA_TYPE_DATETIME }
	}
      ;

      map<pair_ctx,data_type> ctx_map;

    }



    comparison::
    comparison (printable *op, printable *arg1, printable *arg2)
    {
      _set_child (CHILD_OP, op);
      _set_child (CHILD_ARG1, arg1);
      _set_child (CHILD_ARG2, arg2);
    }

    void comparison::
    _print (ostream &stream)
    {

      /*
	The arguments to compare
      */
      printable *arg1 = _get_child (CHILD_ARG1);
      printable *arg2 = _get_child (CHILD_ARG2);

      /*
	Comparison operator
      */
      printable *op = _get_child (CHILD_OP);

      /*
	In and out types of both arguments
      */
      data_type c1_in;
      data_type c2_in;
      data_type c1_out;
      data_type c2_out;

      map<pair_ctx,data_type>::iterator p;

      /*
	The casting objects used to perform the actual cast
      */
      cast *ca1;
      cast *ca2;

      if (!arg1 || !arg2 || !op)
	throw shield::exception::syntax ("Comparison node with null children");
      
      c1_in = arg1->get_context ();
      c2_in = arg2->get_context ();
      c1_out = c1_in;
      c2_out = c2_in;
            
      if (c1_in != c2_in || c1_in == DATA_TYPE_CLOB)
	{
	  
	  if (!ctx_map.size ())
	    {
	      for (size_t i=0; i<(sizeof(mapping)/sizeof(mapping[0])); i++)
		{
		  ctx_map[pair_ctx (mapping[i][0], mapping[i][1])] = mapping[i][2];
		  ctx_map[pair_ctx (mapping[i][1], mapping[i][0])] = mapping[i][2];
		}
	    }

	  p = ctx_map.find (pair_ctx (c1_in,c2_in));
	  
	  if (p == ctx_map.end ())
	    {
	      /*
		If no casting is specified we don't need casting. Just
		print the plain old arguments.
	      */
	      stream << *arg1 << *op << *arg2;
	      return;
	    }
	  
	  c1_out = c2_out = p->second;
	}

      ca1 = new cast (arg1, c1_out);
      ca2 = new cast (arg2, c2_out);

      ca1->set_parent (this);
      ca2->set_parent (this);

      stream << *ca1 << *op << *ca2;
    }

  }

}
