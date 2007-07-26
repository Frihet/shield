/**
   @file cache.cc


   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/
#include <map>
#include <vector>
#include <iostream>

#include "include/cache.hh"
#include "include/exception.hh"

using std::string;
using std::map;
using std::vector;
using std::pair;
using std::cerr;
using std::endl;

namespace shield
{

  namespace cache
  {


    namespace 
    {

      map<const string, pair<long long, string> > shield_cache;
      long long shield_cache_id = 0;
      long long int shield_cache_hit = 0;
      long long shield_cache_miss = 0;
      
      const int SHIELD_CACHE_MAX_AGE = 192;
      const int SHIELD_CACHE_OVERFLOW = 256;
      
    }    
    
    void 
    clear ()
    {
      shield_cache.erase (shield_cache.begin (), shield_cache.end ());
    }

    /*
    string strip_mysql (const string &in, vector<string> &item)
    {
      return "";
    }

    string strip_oracle (const string &in, vector<string> &item)
    {
      string::const_iterator item_begin;
      string::const_iterator item_end;
      string::const_iterator it;

      string result = "";

      for (it=in.begin(); it != in.end (); ++it)
	{
	  if (*it == '\'')
	    {
	      item_begin = it;
	      ++it;
	      while (true)
		{
		  if (it == in.end ())
		    throw exception::invalid_state (string ("Could not strip Oracle query ")+ in);

		  if (*it == '\'')
		    {
		      ++it;
		      if (it == in.end ())
			throw exception::invalid_state (string ("Could not strip Oracle query ")+ in);

		      if (*it != '\'')
			{
			  --it;
			  break;
			}
		    }
		  ++it;
		}
	      result += "''";
	      item_end = it+1;
	      item.push_back (string (item_begin, item_end));
	      cerr << "Got item " << (string (item_begin, item_end)) << endl;
	    }
	  else
	    {
	      result += *it;
	    }
	}

      return result;
    }

    string unstrip_oracle (const string &in, vector<string> &item)
    {
      return "";
    }
    
    */

    std::pair<bool, string> 
    get (const std::string &key)
    {
      
      map<const string,pair<long long, string> >::iterator mit = shield_cache.find (key);
      
      if (mit != shield_cache.end ())
	{
	  shield_cache_hit++;
	  mit->second.first = shield_cache_id++;
	  return make_pair (true, mit->second.second);
	}

      shield_cache_miss++;
      return make_pair (false,string (""));	  
    }
	  
	  
    void
    set (const std::string &key, const std::string &value)
    {

      shield_cache[key] = make_pair (shield_cache_id++,value);
      
      if (shield_cache.size () > SHIELD_CACHE_OVERFLOW)
	{
	  map<const string, pair<long long, string> > new_cache;
	  map<const string, pair<long long, string> >::const_iterator oit;
	  
	  //	  cerr << "CACHE OVERFULL!!! " << shield_cache.size () << " items" << endl; 
	  //cerr << "Hits/misses: " << shield_cache_hit << "/" << shield_cache_miss << endl;
	  
	  for (oit = shield_cache.begin (); oit != shield_cache.end (); ++oit)
	    {
	      long long age = shield_cache_id - oit->second.first;
	      if (age < SHIELD_CACHE_MAX_AGE)
		{
		  new_cache[oit->first] = oit->second;
		}
	    }
	  
	  swap (new_cache, shield_cache);
	  //cerr << "CACHE SIZE NOW  " << shield_cache.size () << " items" << endl; 
	}
    }
      
  }

}
