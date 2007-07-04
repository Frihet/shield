#ifndef UTIL_HH
#define UTIL_HH

#include <sstream>
#include <string>

#include "exception.hh"

using namespace std;

namespace util
{

#define contains( str,... ) contains_str( str, __VA_ARGS__, (void *)0 )

  /**
     Make the specified item into a string. Warning - this function does
     not work with the clever 'virtual <<' hack used by
     e.g. printable. C++ has some pretty lame limitiations.
  */
  template<typename T> inline std::string stringify (const T& x)
  {
    std::ostringstream out;
    if (!(out << x))
      throw shield::exception::syntax ("stringify called on invalid type");
    return out.str ();
  }

  bool contains_str( const char *, ... );
  
  /**
     Remove leading and trailing whitespace from string
  */
  string trim (const string &in);
  
  /**
     Turn string into lower case
  */
  string to_lower (const string &in);
  
  /**
     Turn string into upper case
  */
  string to_upper (const string &in);

    /**
       Escape strings in an oracle compatible fashion.

       Non-printables use the chr function, and long lines are
       automatically broken into multiple lines to avoid the hardcoded
       maximum line lengths in oracle.
    */
    string oracle_escape (const string &in);


}

#endif
