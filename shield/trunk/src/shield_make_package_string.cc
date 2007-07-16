/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include <iostream>
#include <locale.h>

using namespace std;

#define LL 80

/**
   
*/
int main (int argc, char **argv)
{
  setlocale (LC_ALL, "");

  cout << "\"";
  int ll = 0;

  while (true)
    {
      unsigned char c;
      
      c = cin.get ();

      if (cin.eof ())
	break;

      if (ll >= LL)
	{
	  cout << "\"" << endl << "\"";
	  ll = 0;
	}

      if (c < 32 || c >= 128 || c == '"' || c == '\\')
	{
	  if (ll >= LL -6)
	  {
	    cout << "\"" << endl << "\"";
	    ll = 0;
	  }
	  cout << "\\x";
	  ios_base::fmtflags f = cout.flags (ios_base::hex);
	  cout << (int)c;
	  cout.flags (f);
	  cout << "\" \"";
	  ll += 7;
	}
      else
	{
	  cout << c;
	  ll++;
	}

    }

  cout << "\"";
  return 0;
}

