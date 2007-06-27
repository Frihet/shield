#include "transform.hh"

namespace shield
{

namespace transform
{

void 
natural::print (ostream &stream) const
{
  bool do_wrap = true;
  text *txt = dynamic_cast<text *> (__expr);
  
  if (txt)
    {
      if (txt->get_type () == LITERAL)
	do_wrap = false;
    }

  if (__expr->get_context () != UNDEFINED)
    do_wrap = false;

  if (do_wrap)
    {
      if (!get_skip_space ())
	stream << " ";
      stream << "shield.natural (";
      __expr->set_skip_space (true);
      stream << *__expr;
      stream << ")";
    }
  else
    {
      __expr->set_skip_space (get_skip_space ());
      stream << *__expr;
    }
}


}

}
