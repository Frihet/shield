
#include "include/transform.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace catalyst
  {
    
    transform::printable *internal::
    catalyze (transform::printable *node)
    {
      return node->internal_transform ();
    }

  }

}
