/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

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
      debug << (string ("Get introspection data for table ") + table_name);
      
      if (table_map.find (table_name) == table_map.end ())
	{
	  table_map.insert (std::make_pair (table_name, table (table_name)));
	  debug << (string ("Create new introspection data for table ") + table_name);
	}
      
      return (*table_map.find (table_name)).second;
    }

    void clear_table (string table_name)
    {
      debug << (string ("Clear introspection data for table ") + table_name);
      table_map.erase (table_name);
    }
    
  }

}
