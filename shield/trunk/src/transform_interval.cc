#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {
    
    string type_string (interval_type t)
    {
      switch (t)
	{
	case INTERVAL_DAY:
	  return "day";
      
	case INTERVAL_HOUR:
	  return "hour";
      
	case INTERVAL_SECOND:
	  return "second";
      
	case INTERVAL_YEAR:
	  return "year";
      
	case INTERVAL_MONTH:
	  return "month";
      
	case INTERVAL_MINUTE:
	  return "minute";

	default:
	  throw exception::syntax ("Interval of unknown type " + t);
	}
    }

    void interval::
    _print (ostream &stream)
    {
      stream << " interval" << *_get_child (__EXPR) << " " << type_string (__type);
    }

  }

}
