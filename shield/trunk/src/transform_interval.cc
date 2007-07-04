#include "transform.hh"

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
  stream << " interval" << *__expr << " " << type_string (__type);
}

}

}
