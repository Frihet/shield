#include "include/introspection.hh"
#include "include/util.hh"

namespace shield
{
  
  namespace introspection
  {
    
    using namespace util;

    column_type::
    column_type (string t)
    {
      try
	{
	  __type = STRING_TO_ENUM (data_type, t);
	}
      catch (enum_char::bad_enum &e)
	{
	  throw shield::exception::syntax ("Unkown column type " + t);
	}
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
