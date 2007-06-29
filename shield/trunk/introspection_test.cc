
#include "introspection.hh"
#include "database.hh"

using namespace shield::introspection;

int main (int argc, char **argv)
{
  assert (argc>=4);

  shield::database::init (argv[1], argv[2], argv[3]);

  try
    {
  table &t = get_table ("test2");
    table &t2 = get_table ("i_dont_exist");
  for (table::column_const_iterator i = t.column_begin (); i<t.column_end (); i++)
    {
      cout <<"lalala " << (*i).get_name () << endl;
      cout << "number :" << (*i).get_type ().is_number () << endl;
      cout << "char :" << (*i).get_type ().is_char () << endl;
      cout << "lob :" << (*i).get_type ().is_lob () << endl;
    }

  //cerr << "FDS" << endl;
  cout << t.exists() << endl;
  cout << t2.exists() << endl;

    }
  catch (exception &e)
    {
      cerr << e.what () << endl;
    }
  
}
