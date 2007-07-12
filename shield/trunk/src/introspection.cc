#include "include/introspection.hh"
#include "include/transform.hh"

namespace shield
{
  namespace introspection
  {
  
    using namespace shield::introspection;

    logger::logger warning ("shield: introspection warning");
    logger::logger debug ("shield: introspection debug");

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
