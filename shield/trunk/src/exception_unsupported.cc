
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
