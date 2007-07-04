
#include "transform.hh"

namespace shield
{

  namespace transform
  {

    printable *attribute::
    transform (catalyst &catalyst)
    {
      if (__render)
	__render = __render->transform (catalyst);
      
      if (__post_render)
	__post_render = __post_render->transform (catalyst);

      return catalyst (this);
    }

  }

}
