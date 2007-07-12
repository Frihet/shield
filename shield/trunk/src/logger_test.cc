b


#include "include/logger.hh"

using namespace std;

int main ()
{
  logger::logger l1("first"), l2("second"), l3("third");

  l1.enable ();
  l3.enable ();

  l1 << "print me";
  l2 << "not me";
  l3 << "but do print me";

  
}
