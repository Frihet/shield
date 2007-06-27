#include <cctype>
#include <stdarg.h>

#include "util.hh"

namespace util
{

    string to_lower (const string &in)
    {
      string lower = in;
  
      for (size_t i = 0; i<lower.size(); i++)
	{
	  lower[i] = tolower (lower[i]);
	}

      return lower;
    }


    string to_upper (const string &in)
    {
      string upper = in;
  
      for (size_t i = 0; i<upper.size(); i++)
	{
	  upper[i] = toupper (upper[i]);
	}

      return upper;
    }


    string trim (const string &in)
    {
      string::const_iterator b, e;
      for (b = in.begin (); b<in.end (); b++ )
	{
	  if (!strchr( " \n\r", *b))
	    break;
	}

      for (e = in.end ()-1; e<=in.begin (); e-- )
	{
	  if (!strchr( " \n\r", *e))
	    break;
	}
      e++;

      string out (b, e);
      return out;
    }


    bool contains_str (const char *needle, ...)
    {
      char *arg;
      va_list va;
      bool res = false;
  
      va_start (va, needle);
      while ((arg=va_arg (va, char *) )!= 0)
	{
	  if (strcmp (needle, arg) == 0)
	    {
	      res = true;
	      break;
	    }

	}
      va_end (va);

      return res;
    }

    string oracle_escape (const string &in1)
    {
      string in = in1;
      string out = "'";
      int count = 0;
      bool is_clob = false;
      string::const_iterator i;

      if (in.length () == 0)
	in = " ";

      if (in.length () >= 4000)
	{
	  is_clob = true;
	}

      if (is_clob)
	{
	  out = "to_clob ('";
	}


      for (i=in.begin (); i < in.end (); i++)
	{
     
	  char c = *i;
      
	  if (c == '\'')
	    {
	      out += c;
	      out += c;
	    }
	  else if (abs(c) >= 32 )
	    {
	      count++;
	  
	      out += c;
	    }
	  else 
	    {
	      out += "'";

	      if (is_clob)
		out += ")";

	      out += " || chr (";
	      out += stringify ((int)c);
	      out += ") || ";

	      if (is_clob)
		out += "to_clob (";
	      out += "'";

	      count += 20;
	    }

	  if (count >= 60)
	    {
	      out += "' ||\n";
	      if (is_clob)
		out += "to_clob (";
	      out += "'";
	      count = 0;
	    }
	}

      out += "'";

      if (is_clob)
	out += ")";

      return out;
    }



}
