
#include <assert.h>

#include "transform.hh"

namespace shield
{

namespace transform
{

insert::
insert()
  : __field_list (0), 
    __value_list (0), 
    __insert_update (0),
    __ignore (false),
    __name (0)
{
}

void 
insert::set_ignore (bool i)
{
  __ignore = i;
}

void 
insert::set_field_list (printable *l)
{
  __field_list = l;
}

void 
insert::set_value_list (chain *l)
{
  __value_list = l;
}

void 
insert::set_name (printable *l)
{
  __name = l;
}

void 
insert::set_insert_update (printable *l)
{
  __insert_update = l;
}

/**
   Convert an foo=bar, baz=qux style list to a standard sql (foo, baz)
   and (bar, qux) pair of lists.
*/
void 
insert::set_eq_list (chain *l)
{
  chain *col = new chain();
  chain *val = new chain();

  col->set_separator (",");
  val->set_separator (",");

  chain::const_iterator i;  

  for (i=l->begin (); i<l->end (); i++)
    {
      printable *p = *i;

      chain *c = dynamic_cast<chain *> (p);
      col->push ((*c)[0]);
      val->push ((*c)[1]);
    }  
  set_field_list (col);
  set_value_list (new chain (val));
}

void insert::
print (ostream &stream) const
{
  assert (__name);
  assert (__value_list);

  chain::const_iterator i;  

  for (i=__value_list->begin (); i<__value_list->end (); i++)
    {
      stream << "insert into" << *__name << endl;
      if (__field_list)
	{
	  __field_list->set_skip_space (true);
	  stream << "(" << *__field_list << ")" << endl;
	}

      (*i)->set_skip_space (true);
      stream << "values (" << *(*i) << ")";
      stream << endl << endl;
    }
}

}

}
