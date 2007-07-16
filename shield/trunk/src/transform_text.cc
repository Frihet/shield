/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

#include "include/util.hh"


#include "include/transform.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

    text::
    text (unsigned long long val, text_type type, bool insert_whitespace)
      : __val (stringify (val)), __type (type)
    {
      set_context (DATA_TYPE_NUMBER);
      set_skip_space (!insert_whitespace);
    }

    namespace
    {

      /**
	 Remove quoting from quoted identifier
      */
      string ident_unescape (const string &in)
      {
	return in.substr (1, in.length ()-2);
      }

      /**
	 Unescaped quoted mysql strings. Wildly incomplete, e.g. character
	 set specifications and numeric escapes are not yet supported.
      */
      string mysql_unescape (const string &in)
      {
	string out = "";
  
	char end;
	string::const_iterator it;

	it=in.begin (); 
  
	end = *it;
	++it;

	while (true)
	  {
	    char c = *it;

	    if (c == end)
	      {
		break;
	      }
      
	    if (c == '\\')
	      {
		++it;
		if (it == in.end ())
		  {
		    throw exception::syntax ("Malformed string");
		  }

		char c = *it;
	  
		switch (c)
		  {
		  case 'n':
		    out += '\n';
		    break;
	      
		  case 'r':
		    out += '\r';
		    break;
	      
		  case 'f':
		    out += '\f';
		    break;
	      
		  case 'v':
		    out += '\v';
		    break;
	      
		  case 'b':
		    out += '\b';
		    break;
	      
		  case 'e':
		    out += '\e';
		    break;
	      
		  case 'a':
		    out += '\a';
		    break;
	      
		  case 't':
		    out += '\t';
		    break;

		  default:
		    out += c;
	      
		  }
	      }
	    else
	      {
		out += c;
	      }

	    ++it;
	  }

	++it;

	if (it != in.end () )
	  {
	    throw exception::syntax ("Malformed string");
	  }

	return out;
      }

      /**
	 Make an identifier name a valid under oracle
      */
      string identifier_escape (const string &in)
      { 
	string out = "";
	string::const_iterator it;
  
	for (it=in.begin (); it != in.end (); ++it)
	  {
	    switch (*it)
	      {
	      case '_':
		out += *it;
		if ((it+1) == in.end ())
		  out += "_";
		break;

	      default:
		out += *it;
		break;
	      }
	  }

	return out;
      }

      /**
	 The maximum length of the snapshot of the preview in get_node_name
      */
      const int MAX_PREVIEW_LEN = 20;

    }

    void text::
    _print (ostream &stream)
    {
      switch (__type)
	{
      
	case EXACT:    
	  {
	    if (__val.length ())
	      {
		if (!get_skip_space ())
		  stream << " ";

		stream << __val;
	      }
	    break;
	  }

	case IDENTIFIER:    
	case IDENTIFIER_QUOTED:    
	  {
	    string val;
	    if (__type==IDENTIFIER_QUOTED)
	      {
		val = ident_unescape (__val);
	      }
	    else
	      {
		val = __val;
	      }

	    if (is_reserved (val))
	      {
		val = val+"_";
	      }
	    else
	      {
		val = identifier_escape (val);
	      }
	
	    if (val.length ())
	      {
		if (!get_skip_space ())
		  stream << " ";

		stream << to_lower (val);
	      }

	    break;
	  }

	  /**
	     This token is of literal type, i.e. it is literal data that is suppied by the user
	  */
	case LITERAL:
	  {
	    if (__val.size () && __val[0] == ':')
	      {
		if (!get_skip_space ())
		  stream << " ";

		stream << __val;
	      }
	    else
	      {
		string unescaped = mysql_unescape( __val );
		string escaped = oracle_escape( unescaped );
		
		if (!get_skip_space ())
		  stream << " ";
		
		stream << escaped;
	      }
	    break;
	  }
      
	}

    }
    
    string text::
    get_node_name ()
    {
      string res = printable::get_node_name ();
      string content = str ();
      string suffix = "...";
      res += ": ";
      
      for (int i=0; i<MAX_PREVIEW_LEN; i++)
	{
	  if (i == content.size ())
	    {
	      suffix = "";
	      break;
	    }

	  /*
	    Below 32 are non-printables, skip them and print
	    whitespace
	  */
	  if (content[i]<32)
	    {
	      res += ' ';
	    }
	  else
	    {
	      res += content[i];
	    }
	}
      
      res += suffix;
      return res;
    }

  }

}
