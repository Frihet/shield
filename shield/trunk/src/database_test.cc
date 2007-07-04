#include <string>
#include <iostream>
#include <occi.h>

#include "database.hh"
#include "exception.hh"

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
	cout << "yo " << rs.get_string ("t") << endl;
    }
  catch (std::exception &e)
    {
      cerr << e;
    }
  
}
