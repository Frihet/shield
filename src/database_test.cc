/**
   @file database_test.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <string>
#include <iostream>

#include "include/database.hh"
#include "include/exception.hh"

using namespace std;
using namespace oracle::occi;
using namespace shield::database;

int main (int argc, char **argv)
{
  assert (argc>=4);
  
  init (argv[1], argv[2], argv[3]);
  
  result_set &rs = query ("select * from test2 where t2 <> %") << "g";
  
  try
    {
      while (rs.next ())
	cout << "line: " << rs.get_string ("t") << endl;
    }
  catch (std::exception &e)
    {
      cerr << e.what () << endl;
    }
  
}
