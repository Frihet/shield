#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    comparison::
    comparison (printable *op, printable *arg1, printable *arg2)
    {
      set_child (OP, op);
      set_child (ARG1, arg1);
      set_child (ARG2, arg2);
    }

    void comparison::
    print (ostream &stream)
    {
      printable *arg1 = get_child (ARG1);
      printable *arg2 = get_child (ARG2);
      printable *op = get_child (OP);

      if (!arg1 || !arg2 || !op)
	throw shield::exception::syntax ("Comparison node with null children");

      context c1_in = arg1->get_context ();
      context c2_in = arg2->get_context ();
      context c1_out = c1_in;
      context c2_out = c2_in;
      
      if (c1_in == CONTEXT_UNDEFINED || c1_in == CONTEXT_UNDEFINED)
	throw shield::exception::syntax ("Comparison node with undefined context");

      if (c1_in != c2_in)
	{
	  typedef pair<context,context> pair_ctx;

	  map<pair_ctx,context> ctx_map;

	  ctx_map[pair_ctx (CONTEXT_NUMBER,CONTEXT_STRING)] = CONTEXT_STRING;
	  ctx_map[pair_ctx (CONTEXT_STRING,CONTEXT_NUMBER)] = CONTEXT_STRING;
	  
	  ctx_map[pair_ctx (CONTEXT_LOB,CONTEXT_STRING)] = CONTEXT_STRING;
	  ctx_map[pair_ctx (CONTEXT_STRING,CONTEXT_LOB)] = CONTEXT_STRING;
	  
	  ctx_map[pair_ctx (CONTEXT_NUMBER,CONTEXT_LOB)] = CONTEXT_STRING;
	  ctx_map[pair_ctx (CONTEXT_LOB,CONTEXT_NUMBER)] = CONTEXT_STRING;
	  
	  ctx_map[pair_ctx (CONTEXT_STRING,CONTEXT_DATE)] = CONTEXT_DATE;
	  ctx_map[pair_ctx (CONTEXT_DATE,CONTEXT_STRING)] = CONTEXT_DATE;

	  ctx_map[pair_ctx (CONTEXT_STRING,CONTEXT_DATETIME)] = CONTEXT_DATETIME;
	  ctx_map[pair_ctx (CONTEXT_DATETIME,CONTEXT_STRING)] = CONTEXT_DATETIME;
	  
	  map<pair_ctx,context>::iterator p = ctx_map.find (pair_ctx (c1_in,c2_in));
	  
	  if (p == ctx_map.end ())
	    throw shield::exception::syntax (string ("Don't know how to compare types ") + stringify (c1_in) + " and " + stringify (c2_in));
	  
	  context res = (*p).second;
	  
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
