
#include "introspection.hh"
#include "database.hh"

using namespace shield::introspection;

int main (int argc, char **argv)
{

  assert (argc>=4);
  
  shield::database::init (argv[1], argv[2], argv[3]);

  table &t = get_table ("test1");

  for (t::column_const_iterator i = t.column_begin (); i<t.column_end (); i++)
    {
      cout "lalala " << (*i).get_name () << endl;

    }
  

}
