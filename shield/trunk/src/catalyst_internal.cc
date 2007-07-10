
#include "transform.hh"
#include "exception.hh"
#include "catalyst.hh"

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
