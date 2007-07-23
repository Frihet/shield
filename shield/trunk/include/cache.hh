
#include <string>

namespace shield
{
  
  namespace cache
  {
    
    /**
       Remove all items from the shield query cache
    */
    void clear (void);
    
    /** 
	Perform a query cache lookup. Returns null on failiure.
    */
    std::pair<bool, std::string> get (const std::string &key);
    
    /**
       Insert a new value into the cache
    */
    void set (const std::string &key, const std::string &value);
    
  }
  

}
