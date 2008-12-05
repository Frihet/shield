/**
   @file util.hh
 
   A small set of commun utility functions.

   @package Shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 2 or later.

*/
#ifndef UTIL_HH
#define UTIL_HH

#include <sstream>
#include <string>
#include <set>

/**
   @namespace util
   
   Common utility functions.
*/
namespace util
{

  using namespace std;

  /**
     Semi-ulgy but very useful hack for checking if an element is in a list
   */

#define contains( str,... ) util::contains_str( str, __VA_ARGS__, (void *)0 )

  /**
     Make the specified item into a string. Warning - this function does
     not work with the clever 'virtual <<' hack used by
     e.g. printable. C++ has some pretty lame limitiations.
  */
  template<typename T> inline std::string stringify (const T& x)
  {
    std::ostringstream out;
    if (!(out << x))
      return "";
    return out.str ();
  }

  /**
     Like std::pair but more...
  */

  template<typename T1, typename T2, typename T3> class triplet
  {
  public:
    typedef T1 first_type;
    typedef T2 second_type;
    typedef T3 third_type;

    triplet (const T1 &a, const T2 &b, const T3 &c)
      : first (a), second (b), third (c)
    {
    }

    triplet ()
      : first (T1 ()), second (T2 ()), third (T3 ())
    {
    }
    
    template<typename U1, typename U2, typename U3>
    triplet (const triplet<U1, U2, U3> &t)
      : first (t.first), second (t.second), third (t.third)
    {
    }

  public:
    T1 first;
    T2 second;
    T3 third;
  }
  ;

  /**
     Like std::make_pair but more...
  */
  template<typename T1, typename T2, typename T3> 
  inline triplet<T1, T2, T3> make_triplet(T1 a, T2 b, T3 c)
  {
    return triplet<T1, T2, T3> (a, b, c);
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
     Unescaped quoted mysql strings. Wildly incomplete, e.g. character
     set specifications and numeric escapes are not yet supported.
  */
  string mysql_unescape (const string &in);

  /**
     Escape strings in an oracle compatible fashion.
     
     Non-printables use the chr function, and long lines are
     automatically broken into multiple lines to avoid the hardcoded
     maximum line lengths in oracle.

     @return A pair, the first element of which is the resulting string, the second is a boolean flag which is set to true if this string is too long to fit into a char-style string literal and has been converted to a clob.
  */
  pair<string,bool> oracle_escape (const string &in);

  /**
     Attempt to demangle the supplied C++ class/function name. If this
     fails, return the original string.

     This function is just a wrapper aound the cplus_demangle function
     from the libiberty library. The relevant c files have been
     lightly modified to avoid having to include nerly the entire
     libiberty library, but are otherwise unchanged.

     Including the source code of a library directly in a project
     instead of installing and linking to the library is ugly, silly
     and a very bad idea, but unfortunatly, this is for some reason
     the method of using libiberty that the developers themseves
     recommend. This in combination with the fact that no modern
     distributions package libiberty leavs fairly little choice. :-(
  */
  string cxx_demangle (const string &in);

  string identifier_unescape (const string &in);

}

#endif //#ifndef UTIL_HH

