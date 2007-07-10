#include <ctype.h>
#include <map>
#include <exception>
#include <iostream>
#include <stdexcept>

#include "enum_char.hh"

using std::map;
using std::string;

namespace enum_char
{

  namespace
  {
    map<const char *, map<string, int> > char_to_enum;
    map<const char *, map<int, string> > enum_to_char;
  }

  void parse (const char *name, const char *def)
  {
    if (!name || !def)
      throw bad_enum (string ("Syntax error in enum"));
    
    int next_value = 0;

    const char *pos;
    map<const char *, map<string, int> >::iterator i (char_to_enum.find (def));
    if (i != char_to_enum.end ())
      return;

    pos = def;
    while (true)
      {
	const char *name_begin, *name_end;
	char *val_end;
	int value;
	
	while(isspace (*pos))
	  {
	    pos++;
	  }

	if( !*pos)
	  {
	    break;
	  }
	
	name_begin = pos;
	
	while (isalnum(*pos) || (*pos == '_'))
	  {
	    pos++;
	  }
	
	name_end = pos;
	
	if (name_begin == name_end)
	  {
	    throw bad_enum (string ("Syntax error in enum ")+name);
	  }
	
	while(isspace (*pos))
	  {
	    pos++;
	  }
	
	if (*pos == '=')
	  {
	    int base = 10;
	    pos++;
	    while(isspace (*pos))
	      {
		pos++;
	      }

	    if( *pos == '0')
	      {
		pos++;
		base=8;
		if (*pos == 'x'||*pos=='X')
		  {
		    base=16;
		    pos++;
		  }
	      }
	    errno=0;
	    value = strtol(pos, &val_end, base);
	    if (errno || pos==val_end)
	      throw bad_enum (string ("Invalid value for enum ")+name);

	    pos = val_end;
	  }
	else
	  {
	    value = next_value;
	  }

	while(isspace (*pos))
	  {
	    pos++;
	  }
	
	if (*pos == ',')
	  pos++;
	
	char_to_enum[def][string(name_begin, name_end)] = value;
	enum_to_char[def][value] = string(name_begin, name_end);
	
	next_value = value+1;
      }
  }
  
  string enum_to_string_internal (const char *name, const char *def, int idx)
  {
    parse (name, def);
    
    map<const char *, map<string, int> > char_to_enum;
    
    map<int, string> &m = enum_to_char[def];
    map<int, string>::iterator i (m.find (idx));
    
    if (i == m.end ())
      throw bad_enum (string("Enum ")+name+" does not have a member with specified value" );
    
    return i->second;
  }

  int string_to_enum_internal (const char *name, const char *def, const string &val)
  {    
    parse (name, def);
    
    map<string, int> &m = char_to_enum[def];
    map<string, int>::iterator i (m.find (val));
    
    if (i == m.end ())
      throw bad_enum (string("Enum ")+name+" does not have a member named " + val);
    
    return i->second;
  }

}
