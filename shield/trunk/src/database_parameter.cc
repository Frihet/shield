/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/database.hh"
#include "include/util.hh"

namespace shield
{
  
  namespace database
  {

    using namespace util;

    parameter::
    parameter (const string &s)
      : __val (oracle_escape (s))
    {
    }


    parameter::
    parameter (const char *s)
      : __val (oracle_escape (s))
    {
    }

    
    parameter::
    parameter (int i)
      : __val (stringify (i))
    {
    }
    
    parameter::
    parameter (const parameter &p)
      : __val (p.__val)
    {
    }

    string parameter::
    str () const
    {
      return __val;
    }

  }

}
