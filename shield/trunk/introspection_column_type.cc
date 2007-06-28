
#include "introspection.hh"
#include "util.hh"

using namespace util;

namespace shield
{

  namespace introspection
  {
  
    column_type::
    column_type (string t)
    {
      string s = to_lower (t);
    }

    bool column_type::
    is_char () const
    {
      return (__type == CHAR) || (__type == VARCHAR) || (__type == VARCHAR2) ;
    }
    
    bool column_type::
    is_number () const
    {
      return (__type == NUMBER) || (__type == FLOAT);
    }
    
    bool column_type::
    is_lob () const
    {
      return __type == CLOB;
    }
  
  }

}
