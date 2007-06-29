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

      if (s=="char")
	__type = CHAR;
      else if (s=="varchar")
	__type = VARCHAR;
      else if (s=="varchar2")
	__type = VARCHAR2;
      else if (s=="number")
	__type = NUMBER;
      else if (s=="float")
	__type = FLOAT;
      else if (s=="clob")
	__type = CLOB;
      else
	throw shield::exception::syntax ("Unkown column type " + s);
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

    column_type_enum column_type::
    get_type () const
    {
      return __type;
    }

  }

}
