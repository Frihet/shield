
#include "transform.hh"

void shield_list::print( ostream &stream ) const
{
  vector<shield_printable *>::const_iterator i;
  for( i = __arr.begin (); i < __arr.end (); i++ )
    {
      if (i != __arr.begin () )
	{
	  stream << ",";
	}
      shield_printable *it = *i;
      stream << *it;
    }
}
