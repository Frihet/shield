
#include "transform.hh"

namespace shield
{

namespace transform
{

static vector<printable *>all_printable;

void printable_delete()
{
  vector<printable *>::const_iterator i;

  for (i=all_printable.begin (); i<all_printable.end (); i++ )
    {
      delete( *i);
    }
  all_printable.clear();
}


printable::
printable()
  : __push_front (false), __context (UNDEFINED), __skip_space (false)
{
  all_printable.push_back (this);
}


void printable::
print (ostream &stream) const
{
  //int *aaa = 0;
  // *aaa = 3;
  throw exception::syntax ("Tried to print object of type printable");
}

ostream &operator << (ostream &stream, const printable &e)
{
  e.print( stream );
  return stream;
}

const printable *printable::
transform (catalyst &catalyst) const
{
  return catalyst (this);
}

}

}
