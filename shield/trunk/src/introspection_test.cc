
#include "introspection.hh"
#include "database.hh"

int main (int argc, char **argv)
{
  using namespace shield::introspection;

  assert (argc>=4);

  shield::database::init (argv[1], argv[2], argv[3]);

  try
    {
      table &t = get_table ("test2");
      table &t2 = get_table ("i_dont_exist");
      for (table::column_const_iterator i = t.column_begin (); i<t.column_end (); i++)
	{
	  cout <<"name " << (*i).get_name () << endl;
	  cout << "is number :" << (*i).get_type ().is_number () << endl;
	  cout << "is char :" << (*i).get_type ().is_char () << endl;
	  cout << "is lob :" << (*i).get_type ().is_lob () << endl;
	}
      
      cout << t.exists() << endl;
      cout << t2.exists() << endl;
      
    }
  catch (exception &e)
    {
      cerr << e.what () << endl;
    }
  
}
