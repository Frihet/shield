/**
   @file introspection_test.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/introspection.hh"
#include "include/database.hh"

int main (int argc, char **argv)
{
  using namespace shield::introspection;

  assert (argc>=4);

  shield::database::init (argv[1], argv[2], argv[3]);

  try
    {
      table &t = get_table ("test2");
      table &t2 = get_table ("i_dont_exist");
      for (table::column_const_iterator it = t.column_begin (); it != t.column_end (); ++it)
	{
	  cout <<"name " << (*it).get_name () << endl;
	  cout << "is number :" << (*it).get_type ().is_number () << endl;
	  cout << "is char :" << (*it).get_type ().is_char () << endl;
	  cout << "is lob :" << (*it).get_type ().is_lob () << endl;
	}
      
      cout << t.exists() << endl;
      cout << t2.exists() << endl;
      
    }
  catch (exception &e)
    {
      cerr << e.what () << endl;
    }
  
}
