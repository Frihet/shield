
#include "include/transform.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"
#include "include/util.hh"

namespace shield
{

  namespace catalyst
  {
    
    logger::logger warning ("shield: catalyst warning");
    logger::logger debug ("shield: catalyst debug");

    transform::printable *catalyst::
    operator () (transform::printable *node)
    {
      if (__done.find (node) != __done.end ())
	{
	  return node;
	}

      __done.insert (node);

      try
	{
	  return catalyze (node);
	}
      catch (shield::exception::exception &e)
	{
	  warning << (string ("Error in catalyst of type " + util::cxx_demangle(typeid(*this).name ())+"node: ") + node->get_path ());
	  throw;
	}
    }

  }

}
