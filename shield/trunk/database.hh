#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>

using namespace std;

namespace shield
{

namespace database
{
  
  void init (string username, string password, string host);
  
  bool query (string q, ...);
  
  string error ();
  
}

}

#endif
