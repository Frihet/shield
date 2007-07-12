#include "include/introspection.hh"

namespace shield
{
  
  namespace introspection
  {
    
    column::
    column (string name, string type)
      : __name (name), __type (type)
    {
    }
    
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

}
