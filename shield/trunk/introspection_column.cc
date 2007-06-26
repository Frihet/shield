
#include "introspection.hh"

namespace introspection
{
  
  string column::
  get_name () const
  {
    return __name;
  }
  
  column_type column::
  get_type () const
  {
    return __type;
  }
  
}
