
#include <string>
#include "exception.hh"

namespace shield
{

namespace exception
{

syntax_exception::syntax_exception( const string &err )
  : __err (err)
{
}

ostream &operator << (ostream &stream, const syntax_exception &e)
{
  return stream << "Syntax error: " << e.__err << endl;
}

}

}
