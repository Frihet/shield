#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    void fake_query::
    _print (ostream &stream)
    {
      printable *inner = _get_child (CHILD_INNER);
      
      if (!inner)
	throw shield::exception::syntax ("Tried to print null fake_query node");
      
      stream << *inner;
    }

  }

}

