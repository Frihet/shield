#include "include/transform.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

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
      printable *arg1 = _get_child (CHILD_ARG1);
      printable *arg2 = _get_child (CHILD_ARG2);
      printable *op = _get_child (CHILD_OP);

      if (!arg1 || !arg2 || !op)
	throw shield::exception::syntax ("Comparison node with null children");

      data_type c1_in = arg1->get_context ();
      data_type c2_in = arg2->get_context ();
      data_type c1_out = c1_in;
      data_type c2_out = c2_in;
      
      if (c1_in == DATA_TYPE_UNDEFINED || c1_in == DATA_TYPE_UNDEFINED)
	throw shield::exception::syntax ("Comparison node with undefined context");

      if (c1_in != c2_in)
	{
	  typedef pair<data_type,data_type> pair_ctx;
	  
	  map<pair_ctx,data_type> ctx_map;

	  data_type mapping[][3] =
	    {
	      {DATA_TYPE_CLOB, DATA_TYPE_NUMBER, DATA_TYPE_VARCHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_FLOAT, DATA_TYPE_VARCHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_CHAR, DATA_TYPE_CHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_VARCHAR, DATA_TYPE_VARCHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_DATE, DATA_TYPE_VARCHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_DATETIME, DATA_TYPE_VARCHAR },
	      {DATA_TYPE_CLOB, DATA_TYPE_UNDEFINED, DATA_TYPE_VARCHAR },

	      {DATA_TYPE_NUMBER, DATA_TYPE_DATE, DATA_TYPE_NUMBER },
	      {DATA_TYPE_NUMBER, DATA_TYPE_DATETIME, DATA_TYPE_NUMBER },

	      {DATA_TYPE_FLOAT, DATA_TYPE_DATE, DATA_TYPE_NUMBER },
	      {DATA_TYPE_FLOAT, DATA_TYPE_DATETIME, DATA_TYPE_NUMBER },

	      {DATA_TYPE_CHAR, DATA_TYPE_DATE, DATA_TYPE_CHAR },
	      {DATA_TYPE_CHAR, DATA_TYPE_DATETIME, DATA_TYPE_CHAR },

	      {DATA_TYPE_VARCHAR, DATA_TYPE_DATE, DATA_TYPE_CHAR },
	      {DATA_TYPE_VARCHAR, DATA_TYPE_DATETIME, DATA_TYPE_CHAR },

	      {DATA_TYPE_DATE, DATA_TYPE_DATETIME, DATA_TYPE_DATE },
	      {DATA_TYPE_DATE, DATA_TYPE_UNDEFINED, DATA_TYPE_DATE },

	      {DATA_TYPE_DATETIME, DATA_TYPE_UNDEFINED, DATA_TYPE_DATETIME }
	    }
	  ;

	  for (int i=0; i<(sizeof(mapping)/sizeof(mapping[0])); i++)
	    {
	      ctx_map[pair_ctx (mapping[1][0], mapping[i][1])] = mapping[i][2];
	      ctx_map[pair_ctx (mapping[1][1], mapping[i][0])] = mapping[i][2];
	    }
	  
	  map<pair_ctx,data_type>::iterator p = ctx_map.find (pair_ctx (c1_in,c2_in));
	  
	  if (p == ctx_map.end ())
	    {
	      /*
		If no casting is specified we don't need casting. Just
		print the plain old arguments.
	      */
	      stream << *arg1 << *op << *arg2;
	      return;
	    }
	  
	  data_type res = (*p).second;
	  
	  c1_out = res;
	  c2_out = res;
	  
	}

      cast *ca1 = new cast (arg1, c1_out);
      cast *ca2 = new cast (arg2, c2_out);

      ca1->set_parent (this);
      ca2->set_parent (this);

      stream << *ca1 << *op << *ca2;
    }

  }

}
