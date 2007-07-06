
#include <string>
#include "exception.hh"
#include "util.hh"

namespace shield
{

  namespace exception
  {

    unsupported::unsupported( const string &file, int line )
    {
      _set_what ("Unsupported feature. File: " + file+ ", line: " + util::stringify (line));
    }

}

}
