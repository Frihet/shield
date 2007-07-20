/**

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 3.

*/

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/util.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

    printable_alias::
    printable_alias (printable *item, text *alias, bool alias_implicit)
      : __alias_implicit (alias_implicit)
    {
      if (!item)
	{
	  throw exception::invalid_type ("Constructor for printable_alias", "non-null");
	}
      
      _set_child (CHILD_ITEM, item);
      _set_child (CHILD_ALIAS, alias);
    }
    void printable_alias::
    _print (ostream &stream)
    {
      
      if (!get_item ())
	{
	  throw exception::invalid_state ("Called print on printable_alias with null-item");
	}
      
      stream << *get_item ();
      __print_alias (stream);
    }

    void printable_alias::
    __print_alias (ostream &stream)
    {
      text *alias = get_alias ();
      if( alias && alias->length () )
	{
	  stream << " " << get_internal_alias ();
	}
      
    }

    string printable_alias::
    get_internal_alias ()
    {
      text *alias = get_alias ();
      if( alias && alias->length () )
	{
	  return alias->str () + (__alias_implicit ? "_" + util::stringify (this): "");
	}
      else 
	{
	  throw exception::invalid_state ("Tried to print null alias");
	}
    }
    
  }

}
