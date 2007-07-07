
#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    cast::
    cast (printable *p, context c)
      : __context (c)
    {
      _set_child (CHILD_INNER, p);
    }
    
    void cast::
    _print (ostream &stream)
    {
      printable *inner = _get_child (CHILD_INNER);

      if (!inner)
	throw shield::exception::syntax ("Cast of null node");

      context real = inner->get_context ();
      context desired = __context;

      if (real == desired)
	{
	  stream << *inner;
	}
      else
	{
	  stream << " ";

	  switch (desired)
	    {

	    case CONTEXT_LOB:
	      stream << "to_clob (" << *inner << ")";
	      break;

	    case CONTEXT_NUMBER:
	      stream << "to_number (" << *inner << ")";
	      break;

	    case CONTEXT_STRING:
	      stream << "to_char (" << *inner << ")";
	      break;

	    case CONTEXT_DATE:
	      stream << "to_date (" << *inner << ", 'yyyy-mm-dd')";
	      break;
	      
	    case CONTEXT_DATETIME:
	      stream << "to_date (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss')";
	      break;
	      
	    case CONTEXT_SORTABLE:

	      switch (real)
		{
		case CONTEXT_NUMBER:
		case CONTEXT_STRING:
		  break;

		case CONTEXT_LOB:
		case CONTEXT_DATE:
		case CONTEXT_DATETIME:
		  stream << "to_char (" << *inner << ")";
		  break;
		}

	      break;

	    default:
	      throw shield::exception::syntax ("Unknown desired context " + desired);
	      
	    }	      
	}
      
    }

  }

}
