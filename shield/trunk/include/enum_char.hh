/**
   @file enum_char.hh

   Define enums with string representations. This file allows you to
   define enums and to map between a string representation of each
   enum value and the original enum value. It is implemented using
   preprocessor magic to give you a sane interface.

   When given bad input, all functions may throw a enum_char::bad_enum
   exception.

   If the string&lt;-&gt;enum conversion calls are never called, these
   should be exactly as fast as normal enums. On the first call to one
   of the conversion calls, the string is parsed, which takes time
   proportional to the length of the enum definition (sans
   comments). On subsequent calls, the lookup is done using std::map
   objects, which means lookup time should be proportional to the
   logarithm of the number of magic enums and the number of elements
   in the enum. Overall, this should be speedy, and virtually free
   when the feature is unused.

   Example usage:
   <pre>
namespace foo
{

  // Define enum named my_magic_enum with elements val0, val1, val2...
  ENUM_STRING (my_magic_enum, val0, val1, val2, val34=34, val35);

  void enum_test ()
  {
  // name2 becomes 'val2'
    string name2 = ENUM_TO_STRING (my_magic_enum, 1);
    // val35 becomes 35
    int val35 = STRING_TO_ENUM(my_magic_enum, "val35");
    
  }
}

</pre>

@remark package: shield
@author Axel Liljencrantz

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 3.


*/
#ifndef ENUM_CHAR_HH
#define ENUM_CHAR_HH

#include <string>

/**
   Magical enum definition function. This function will define an enum
   named by the first argument. It will also create a string named
   like the enum but with '_str' appended. This string should not be
   used directly, but it is used internally by the magic
   ENUM_TO_STRING and STRING_TO_ENUM macros.
*/
#define ENUM_STRING(name, ...) enum name {__VA_ARGS__}; const static char *name ## _str = #__VA_ARGS__

/**
   Get a string representation of the specified enum value

   @param enum_name the name of the enum
   @param idx the enum value

   May throw a enum_char::bad_enum
   exception.
*/
#define ENUM_TO_STRING(enum_name, idx) enum_char::enum_to_string_internal( #enum_name, enum_name ## _str, idx)

/**
   Get the value of an enum string representation

   @param enum_name the name of the enum
   @param string_name the name of the eval value

   May throw a enum_char::bad_enum
   exception.
*/
#define STRING_TO_ENUM(enum_name, string_name) (enum_name) enum_char::string_to_enum_internal( #enum_name, enum_name ## _str, string_name)

/**
   @namespace enum_char

   Functions and exception used by the magical enum macros.
*/

namespace enum_char
{
  
  /**
     Magical internal function. Don't ever call directly.
  */
  std::string enum_to_string_internal (const char *name, const char *def, int idx);
  
  /**
     Magical internal function. Don't ever call directly.
  */
  int string_to_enum_internal (const char *name, const char *def, const std::string &val);

  /**
     Exception thrown when any enum_char function encounters a bad enum definition string
  */
  class bad_enum
    : std::exception
  {
  public:
    bad_enum (const std::string &what)
      : __what (what)
    {
    }

    virtual const char *what (void) const throw ()
    {
      return __what.c_str ();
    }
    
    virtual ~bad_enum (void) throw ()
    {
    }

  private:
    std::string __what;

  }
  ;

}
#endif //#ifndef ENUM_CHAR_HH
