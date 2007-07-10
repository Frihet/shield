#include <cctype>
#include <stdarg.h>

#include "util.hh"
#include "demangle.h"


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

  string cxx_demangle (const string &in)
  {
    string res = in;

    /*
      Start by trying to do a plain demangle
    */
    char *demangled = cplus_demangle (in.c_str (), DMGL_PARAMS | DMGL_ANSI);
    char *begin = demangled;
    char *end;

    if (!demangled)
      {
	/*
	  Ok, that didn't work. The argument might not be a complete
	  function call, but in fact only a mangled class name. If so,
	  we need prepend '_ZN1aE' to the mangled text, which should
	  hopefully mean a call to the function 'a' with the class as
	  the parameter. 

	  This is _obviously_ completely dependant on the mangling
	  scheme, but we don't really have that much to lose, since if
	  this isn't valid, we'll just return the mangeld string. Keep
	  in mind that the above function call is able to
	  automatically classify the mangling scheme of the mangled
	  name - it shouldn't be able to missclassify a 'mixed'
	  scheme and return something invalid.
	*/
	demangled = cplus_demangle ((string ("_ZN1aE")+in).c_str (), DMGL_PARAMS | DMGL_ANSI);
	if (demangled)
	  {
	    /*
	      We did it! Victory dance!

	      Yes! Yes! Yay! Yay! 
	      
	      Yes! Yes! Yay! Yay! 
	      
	      Yes! Yes! Yay! Yay! 
	      
	      All we have to do now is strip away the 'a' function
	      call to get only the class definition.
	    */
	    begin = strchr( demangled, '(');
	    if (!begin)
	      begin = demangled;
	    else
	      begin++;
	    
	    end = strrchr (demangled, ')');
	    if (end)
	      *end = 0;
	  }
      }
    
    //    cerr << in << " -> " << (foo?foo:in) << endl;

    if (demangled)
      {
	res = begin;
	free (demangled);
      }

    return res;
  }

}
