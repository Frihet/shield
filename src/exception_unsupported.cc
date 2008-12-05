/**
   @file exception_unsupported.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <string>
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace exception
  {

    unsupported::unsupported (const string &file, int line)
    {
      _set_what ("Unsupported feature. File: " + file+ ", line: " + util::stringify (line));
    }

    unsupported::unsupported (const string &message)
    {
      _set_what ("Unsupported feature: " + message);
    }

}

}
