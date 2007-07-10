#include "introspection.hh"
#include "util.hh"

namespace shield
{
  
  namespace introspection
  {
    
    using namespace util;

    column_type::
    column_type (string t)
    {
      string s = to_lower (t);

      if (s=="char")
	__type = DATA_TYPE_CHAR;
      else if (s=="varchar")
	__type = DATA_TYPE_VARCHAR;
      else if (s=="varchar2")
	__type = DATA_TYPE_VARCHAR;
      else if (s=="number")
	__type = DATA_TYPE_NUMBER;
      else if (s=="float")
	__type = DATA_TYPE_FLOAT;
      else if (s=="clob")
	__type = DATA_TYPE_CLOB;
      else if (s=="date")
	__type = DATA_TYPE_DATETIME;
      else
	throw shield::exception::syntax ("Unkown column type " + s);
    }

    bool column_type::
    is_char () const
    {
      return (__type == DATA_TYPE_CHAR) || (__type == DATA_TYPE_VARCHAR);
    }
    
    bool column_type::
    is_number () const
    {
      return (__type == DATA_TYPE_NUMBER) || (__type == DATA_TYPE_FLOAT);
    }
    
    bool column_type::
    is_lob () const
    {
      return __type == DATA_TYPE_CLOB;
    }

    data_type column_type::
    get_type () const
    {
      return __type;
    }

  }

}
