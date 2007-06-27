
#include <string>
#include "exception.hh"

namespace exception
{

not_found_exception::not_found_exception( const string &not_found )
  : __not_found (not_found)
{
}

ostream &operator << (ostream &stream, const not_found_exception &e)
{
  return stream << "Could not find item '" << e.__not_found << "'" << endl;
}

}

