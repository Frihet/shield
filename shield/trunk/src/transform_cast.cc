
#include "transform.hh"
#include "exception.hh"
#include "introspection.hh"

namespace shield
{

  namespace transform
  {

    cast::
    cast (printable *p, int contexts)
      : __contexts (contexts)
    {
      if (!contexts)
	throw shield::exception::syntax ("Cast called with no legal type");

      _set_child (CHILD_INNER, p);
    }

    data_type cast::
    get_context (void)
    {
      printable *inner = _get_child (CHILD_INNER);

      if (!inner)
	throw shield::exception::syntax ("Cast of null node");

      data_type real = inner->get_context ();
      
      /*
	Desired is a bitmask of all types that are ok. 
      */
      int desired = __contexts;

      /*
	First check if the actual type is one of the allowed types. If so, do no casting.
      */
      if (real & desired)
	return real;

      data_type order[] = 
	{
	  DATA_TYPE_CLOB,
	  DATA_TYPE_VARCHAR,
	  DATA_TYPE_CHAR,
	  DATA_TYPE_NUMBER,
	  DATA_TYPE_FLOAT,
	  DATA_TYPE_DATETIME,
	  DATA_TYPE_DATE
	}
      ;

      for (int i=0; i<(sizeof(order)/sizeof(data_type)); i++)
	{
	  if (desired & order[i])
	    {
	      return order[i];
	    }
	}
      
      throw shield::exception::syntax ("Unknown desired context " + desired);
    }
    
    void cast::
    _print (ostream &stream)
    {
      printable *inner = _get_child (CHILD_INNER);

      if (!inner)
	throw shield::exception::syntax ("Cast of null node");

      data_type real = inner->get_context ();
      
      /*
	Desired is a bitmask of all types that are ok. 
      */
      int desired = __contexts;

      /*
	First check if the actual type is one of the allowed types. If so, do no casting.
      */
      if (real & desired)
	{
	  stream << *inner;
	}
      else
	{
	  stream << " ";

	  if (desired & DATA_TYPE_CLOB)
	    {  
	      inner->set_skip_space (true);

	      if (real & DATA_TYPE_DATETIME)
		{
		  stream << "to_clob (to_char (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss'))";
		}
	      else if (real & DATA_TYPE_DATE)
		{
		  stream << "to_clob (to_char (" << *inner << ", 'yyyy-mm-dd'))";
		}
	      else 
		{
		  stream << "to_clob (" << *inner << ")";
		}
	    }
	  else if (desired & (DATA_TYPE_CHAR | DATA_TYPE_VARCHAR))
	    {
	      if (real & (DATA_TYPE_CHAR | DATA_TYPE_VARCHAR))
		{
		  stream << *inner;
		}
	      else if (real & DATA_TYPE_DATETIME)
		{
		  inner->set_skip_space (true);
		  stream << "to_char (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss')";
		}
	      else if (real & DATA_TYPE_DATE)
		{
		  inner->set_skip_space (true);
		  stream << "to_char (" << *inner << ", 'yyyy-mm-dd')";
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "to_char (" << *inner << ")";
		}
	    }
	  else if (desired & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
	    {
	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  stream << *inner;
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "to_number (" << *inner << ")";
		}
	    }
	  else if (desired & DATA_TYPE_DATETIME)
	    {
	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  inner->set_skip_space (true);
		  stream << "to_date (" << *inner << ", 'yyyymmddhh24miss')";
		}
	      else if (real & (DATA_TYPE_DATE | DATA_TYPE_DATETIME))
		{
		  stream << *inner;
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "to_date (" << *inner << ", 'yyyy-mm-dd hh24:mi:ss')";
		}
	    }
	  else if (desired & DATA_TYPE_DATE)
	    {

	      if (real & (DATA_TYPE_NUMBER | DATA_TYPE_FLOAT))
		{
		  inner->set_skip_space (true);
		  stream << "to_date (" << *inner << ", 'yyyymmdd')";
		}
	      else if (real & (DATA_TYPE_DATE | DATA_TYPE_DATETIME))
		{
		  stream << *inner;
		}
	      else
		{
		  inner->set_skip_space (true);
		  stream << "to_date (" << *inner << ", 'yyyy-mm-dd')";
		}
	    }
	  else
	    {
	      throw shield::exception::syntax ("Unknown desired context " + desired);
	    }	      
	}
      
    }

  }

}
