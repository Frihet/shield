
#include "include/transform.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"

namespace shield
{

  namespace catalyst
  {

    transform::printable *replace_identifier::
    catalyze (transform::printable *node)
    {
      transform::text *t = dynamic_cast<transform::text *> (node);
      if (!t)
	{
	  return node;
	}

      if (t->get_type () != transform::IDENTIFIER &&
	  t->get_type () != transform::IDENTIFIER_QUOTED)
	{
	  return node;
	}

      string st = t->str ();

      if (__mapping.find (st) == __mapping.end ())
	{
	  return node;
	}
      return __mapping[st];
    }
  }
}
