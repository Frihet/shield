
#include <string>
#include "shield_exception.hh"

namespace shield
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

