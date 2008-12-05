/**
   @file cache.hh

   The transform namespace contains a very simple query cache. 

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.


*/

#include <string>

namespace shield
{
  
  namespace cache
  {
    
    /**
       Remove all items from the shield query cache. This needs to be
       called whenever the database structure changes.
    */
    void clear (void);
    
    /** 
	Perform a query cache lookup. 

	@return The cache lookup result. If the first element of the pair is true, then the cache lookup was ok, and the second value is the translated query. If the first element is false, then the cache lookup failed and the second element is undefined.
    */
    std::pair<bool, std::string> get (const std::string &key);
    
    /**
       Insert a new value into the cache

       @param key The untranslated query

       @param value The translated query
    */
    void set (const std::string &key, const std::string &value);
    
  } 

}
