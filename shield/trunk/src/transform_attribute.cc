
#include "transform.hh"

namespace shield
{

  namespace transform
  {

    void attribute::
    _print (std::ostream &stream)
    {
      if (get_render ())
	{
	  stream << *get_render ();
	}
    }
  }

}
