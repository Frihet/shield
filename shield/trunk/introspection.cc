
#include "introspection.hh"
#include "shield.hh"

using namespace shield;

namespace introspection
{
  
  static map<string, table> table_map;
  
  table &get_table (string table_name)
  {
    if (table_map.find (table_name) == table_map.end ())
      {
	//	table_map[table_name] = table (table_name);
	table_map.insert (std::make_pair (table_name, table (table_name)));
      }
    
    return (*table_map.find (table_name)).second;
    
  }

}


