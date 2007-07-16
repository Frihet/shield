/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

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
