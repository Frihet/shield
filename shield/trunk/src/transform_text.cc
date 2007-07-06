#include "util.hh"


#include "transform.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

    text::
    text (unsigned long long val, text_type type, bool insert_whitespace)
      : __val (stringify (val)), __type (type)
    {
      set_context (CONTEXT_NUMBER);
      set_skip_space (!insert_whitespace);
    }

    namespace
    {

      /**
	 Remove quoting from quoted identifier
      */
      static string ident_unescape (const string &in)
      {
	return in.substr (1, in.length ()-2);
      }

      /**
	 Unescaped quoted mysql strings. Wildly incomplete, e.g. character
	 set specifications and numeric escapes are not yet supported.
      */
      static string mysql_unescape (const string &in)
      {
	string out = "";
  
	char end;
	string::const_iterator i;

	i=in.begin (); 
  
	end = *i;
	i++;

	while (true)
	  {
	    char c = *i;

	    if (c == end)
	      {
		break;
	      }
      
	    if (c == '\\')
	      {
		i++;
		if (i == in.end ())
		  {
		    throw exception::syntax ("Malformed string");
		  }

		char c = *i;
	  
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

	    i++;
	  }

	i++;

	if (i != in.end () )
	  {
	    throw exception::syntax ("Malformed string");
	  }

	return out;
      }

      /**
	 Make an identifier name a valid under oracle
      */
      static string identifier_escape (const string &in)
      { 
	string out = "";
	string::const_iterator i;
  
	for (i=in.begin (); i < in.end (); i++)
	  {
	    switch (*i)
	      {
	      case '_':
		out += *i;
		if ((i+1) == in.end ())
		  out += "_";
		break;

	      default:
		out += *i;
		break;
	      }
	  }

	return out;
      }

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

  }

}
