
#include <string>
#include "exception.hh"

namespace exception
{

unsupported_exception::unsupported_exception( const string &file, int line )
  : __file (file), __line (line)
{  
}

ostream &operator << (ostream &stream, const unsupported_exception &e)
{
  return stream << "Unsupported feature.\nFile: " <<e.__file <<", line: " << e.__line << endl;
}

}

