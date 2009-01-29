/**
   @file transform_text.cc

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
	Limit is one less than actual oracle limit because we might
	need to add a trailing slash...
      */
      const unsigned int ORACLE_MAX_CHARS = 29u;
      
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
	
	if (in.size () > ORACLE_MAX_CHARS )
	  out += in.substr (0, ORACLE_MAX_CHARS);
	else
	  out += in;

	/*
	  The 'shield' prefix is used internally by Shield, we escape
	  all such instances with a trailing '_'. That also means we
	  must escape trailing slashes with another trailing slash...
	*/
	if (in.substr(0,6) == "shield" || in[in.size ()-1] == '_' || is_reserved (in))
	  out += "_";

	return out;
      }

      /**
	 The maximum length of the snapshot of the preview in get_node_name
      */
      const unsigned int MAX_PREVIEW_LEN = 20u;

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
	    __str = __val;
	    if (to_lower(__str) == "null") 
	      {
		__str = string("to_char (") + __str + ")";
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
	    
	    __str = to_lower (identifier_escape (val));

	    break;
	  }
	  
	  /**
	     This token is of literal type, i.e. it is literal data that is suppied by the user. It should be escaped.
	  */
	case LITERAL:
	  {
	    if (__val.size () && (__val[0] == ':' || __val[0] == '%'))
	      {
		__str = __val;//string("to_char (") + __val + ")";
	      }
	    else
	      {
		pair<string, bool> escaped = oracle_escape (mysql_unescape (__val));
		__str = escaped.first;
		__is_clob = escaped.second;
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
      
      for (size_t i=0; i<MAX_PREVIEW_LEN; i++)
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
	case EXACT:
	  return string("const_") +__val;

	case IDENTIFIER:
	  return __val;
	  
	case IDENTIFIER_QUOTED:
	  return __val.substr (1, __val.size ()-2);
	  
	case LITERAL:
	  if (__val.size () && (__val[0] == ':' || __val[0] == '%')) {
	    return __val.substr(1);
	  } 

	  return mysql_unescape (__val);
	  
	default:
	  throw exception::invalid_state (string ("Called unmangled_str () on ")+get_node_name () + ", of type " + ENUM_TO_STRING (text_type, get_type ()));
	}
    }

    data_type text::
    get_context (void)
    {
      if (get_type () == EXACT && to_lower(__str) == "null" )
	{
	  return DATA_TYPE_CHAR;
	}
      if (get_type () != LITERAL)
	{
	  return printable::get_context ();
	}

      switch (__val[0])
	{
	  //	case ':':
	  //	case '%':
	  //	  return DATA_TYPE_CHAR;

	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  if (__val.find ('.') != __val.npos)
	    {
	      return DATA_TYPE_FLOAT;
	    }
	  return DATA_TYPE_NUMBER;

	case '"':
	case '\'':
	  if (__is_clob)
	    {
	      return DATA_TYPE_CLOB;
	    }
	  return DATA_TYPE_CHAR;
	  
	default:
	  return printable::get_context ();

	}
      
    }
    
  }

}
