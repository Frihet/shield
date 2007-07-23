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
#include "include/exception.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

      text::
      text (const string &text, text_type type, bool insert_whitespace)
	: __val (text), __type (type)
      {
	if (type == LITERAL)
	  set_context (DATA_TYPE_CHAR);
	
	set_skip_space (!insert_whitespace);
	__str_calc ();
      }


    text::
    text (unsigned long long val, text_type type, bool insert_whitespace)
      : __val (stringify (val)), __type (type)
    {
      set_context (DATA_TYPE_NUMBER);
      set_skip_space (!insert_whitespace);
      __str_calc ();
    }

    text::
    text (text *t)
    {
      if (!t)
	throw exception::invalid_state ("Tried to copy null text");

      __val = t->__val;
      __type = t->__type;
      __str = t->__str;

      set_context (t->get_context ());
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
	 Make an identifier name a valid oracle name
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
      if (!get_skip_space ())
	stream << " ";
      stream << __str;
    }

    string text::
    str (void)
    {
      return __str;
    }


    void text::
    __str_calc ()
    {

      switch (__type)
	{
      
	case EXACT:    
	  {
	    if (__val.length ())
	      {
		__str = __val;
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
	
	    __str = to_lower (val);
	    
	    break;
	  }

	  /**
	     This token is of literal type, i.e. it is literal data that is suppied by the user
	  */
	case LITERAL:
	  {
	    if (__val.size () && __val[0] == ':')
	      {
		__str = __val;
	      }
	    else
	      {
		__str = oracle_escape (mysql_unescape (__val));
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

    string text::
    unmangled_str ()
    {
      switch (get_type ())
	{
	case IDENTIFIER:
	  return __val;
	  
	case IDENTIFIER_QUOTED:
	  return __val.substr (1, __val.size ()-2);
	  
	case LITERAL:
	  return mysql_unescape (__val);
	  
	default:
	  throw exception::invalid_state (string ("Called unmangled_str () on ")+get_node_name () + ", of type " + ENUM_TO_STRING (text_type, get_type ()));
	}
    }


  }

}
