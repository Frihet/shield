#include "introspection.hh"
#include "transform.hh"

namespace shield
{
  namespace introspection
  {
  
    using namespace shield::introspection;

    /*
      Map containing all previously introspected tables
    */
    namespace
    {
      map<string, table> table_map;
    }

    table &get_table (string table_name)
    {
      if (table_map.find (table_name) == table_map.end ())
	{
	  table_map.insert (std::make_pair (table_name, table (table_name)));
	}
      
      return (*table_map.find (table_name)).second;
    }
    
  }

}
