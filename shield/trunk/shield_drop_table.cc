#include "shield.hh"

namespace shield
{


drop_table::drop_table( printable *name, bool if_exists)
  : __name (name), __if_exists (if_exists)
{
}

void 
drop_table::print (ostream &stream) const
{
  if (__if_exists)
    {
      drop_item (stream, "table", __name->str ());
    }
  else
    {
      stream << "drop table" << *__name << ";" << endl << endl;
    }

}


}
