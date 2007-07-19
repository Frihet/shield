/**
   @file transform_select.cc

   This is the file for writing out Oracle style sql code for a select
   query. It is probably the single most complicated query, since
   there are so many optional query parts, and so many of them work
   differently in Oracle. The code is a bit ugly in places and there
   are a few situations where valid but rare cases are not handeld
   correctly. 

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/
#include <set>
#include <sstream>

#include "include/transform.hh"
#include "include/introspection.hh"
#include "include/catalyst.hh"
#include "include/exception.hh"
#include "include/util.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

    using namespace util;

    select::
    select()
    {
    }

    /**
       This function creates a version of the having clause where all
       uses of aliases have been replaced with the original code, and
       then prints it.
    */
    void 
    print_having (ostream &stream, 
		  printable *having,
		  map<string, printable *> &mapping)
    {
      catalyst::replace_identifier cat (mapping);

      printable *t = having->transform (cat);
      stream << *t;
    }

      /**
	 Search the specified tree searching for printable_alias objects
	 and save their mapping
      */
      static void 
      get_table_alias (printable *it, map<string, printable *> &table_alias)
      {
	chain *ch = dynamic_cast<chain *> (it);
	printable_alias *al = dynamic_cast<printable_alias *> (it);

	if (al)
	  {
	    if (al->get_alias ())
	      {
		table_alias[al->get_alias ()->str ()] = al->get_item ();
	      }
	  }
	else if (ch)
	  {
	    chain::const_iterator it;
	    for (it=ch->begin (); it != ch->end (); ++it)
	      {
		get_table_alias (*it, table_alias);
	      }
	  }
      }

    void select::
    _print (ostream &stream)
    {
  
      chain::const_iterator it;

      bool is_subselect=false;
      printable *parent;
      query *parent_query;
      

      if (!get_item_list ())
	{
	  throw exception::syntax ("No item list for select");
	}

      stream << "/*\n";
      for (it=get_item_list ()->begin (); it!=get_item_list ()->end (); ++it)
	{
	  printable *pr = *it;

	  printable_alias *item = dynamic_cast<printable_alias *> (pr);
	  if (!item)
	    throw shield::exception::invalid_type ("Select query item list", "printable_alias");

	  if (item->get_alias ())
	    {
	      stream << item->get_alias ()->unmangled_str () << "\n";
	    }
	  else
	    {
	      printable *sub_item = item->get_item ();
	      identity *sub_item_identity = dynamic_cast<identity *> (sub_item);
	      if (sub_item_identity)
		{
		  sub_item = sub_item_identity->get_field ();
		}
	      
	      stream << util::identifier_unescape (sub_item->str ()) << "\n";
	    }
	}
      stream << "*/\n";


      if (get_limit_clause ())
	{
	  stream << "select";

	  for (it=get_item_list ()->begin (); it!=get_item_list ()->end (); ++it)
	    {
	      printable *pr = *it;
	      
	      printable_alias *item = dynamic_cast<printable_alias *> (pr);
	      if (!item)
		throw shield::exception::invalid_type ("Select query item list", "printable_alias");
	      
	      if (it != get_item_list ()->begin ())
		stream << ",";

	      if (item->get_alias ())
		{
		  stream << *item->get_alias ();
		}
	      else
		{
		  printable *sub_item = item->get_item ();
		  identity *sub_item_identity = dynamic_cast<identity *> (sub_item);
		  if (sub_item_identity)
		    {
		      sub_item = sub_item_identity->get_field ();
		    }
		  
		  stream << *sub_item;
		}
	    }
	  
	  stream <<" from (\n";
	}

      stream << "select";

      if (get_option_clause ())
	stream << *get_option_clause ();

      stream << *get_item_list ();

      /*
	We use the magic rownum field to simulate the limit clause.

	Since rownum is only increased on actually used rows, we need
	to assign rownum to a temporary field and perform the real
	limit in the outer select.
      */
      if (get_limit_clause ())
	{

	  if (get_group_clause ())
	    {
	      stream << "," << *(new printable_alias (new text ("shield_arb_agg_num (rownum)", EXACT),
						      new text ("shield_rownum", EXACT)));
	    }
	  else
	    {
	      stream << "," << *(new printable_alias (new text ("rownum", EXACT),
						      new text ("shield_rownum", EXACT)));
	    }
	}

      stream << "\nfrom ";

      if (get_table_list () && !get_table_list ()->empty ())
	{
	  stream << *get_table_list ();
	}
      else
	{
	  stream << " dual";
	}

      if (get_where_clause ())
	{
	  stream << "\nwhere" << *get_where_clause ();
	}

      if (get_group_clause ())
	{
	  stream << "\ngroup by" << *get_group_clause ();
	}

      if (get_having_clause ())
	{
	  stream << "\nhaving";
	  print_having (stream, get_having_clause (), __field_alias);
	}

      if (get_order_clause ())
	{
	  stream << "\norder by" << *get_order_clause ();
	}

      if (get_limit_clause () )
	stream << ") where" << *get_limit_clause ();

      parent = get_parent ();
      if (parent)
	{
	  parent_query = parent->get_query ();
	  if( dynamic_cast<select *> (parent_query))
	    is_subselect = true;
	}

      if (!is_subselect)
	{
	  stream << endl << endl << sep;
	}
    }

    void select::
    _make_condensed_table_list ()
    {
      _condensed_table_list.clear ();
      catalyst::find_table cat (this);      
      get_table_list ()->transform (cat);
      _condensed_table_list = cat.get_table_list ();
    }

    text *select::
    unalias_table (text *alias)
    {
      map<string, printable *> table_alias;
      get_table_alias (get_table_list (), table_alias);

      if (table_alias.find (alias->str ()) != table_alias.end ())
	{
	  printable *res = table_alias[alias->str ()];
	  
	  text *txt = dynamic_cast<text *> (res);
	  identity *id = dynamic_cast<identity *> (res);
	  
	  if (id)
	    {
	      txt = id->get_table ();
	    }

	  if (!id)
	    throw shield::exception::invalid_type ("Unaliased table name", "text");

	  return txt;
	}
      return alias;
    }

    printable *select::
    internal_transform ()
    {
      __pre_calculate ();
      __resolve_item_list ();

      catalyst::create_identity id_catalyst (this);
      printable *res= this->transform (id_catalyst);

      if (get_group_clause ())
	{
	  catalyst::aggregate agg_catalyst (this, __group_field);
	  
	  set_item_list ( dynamic_cast<chain *> (get_item_list ()->transform (agg_catalyst)));
	  if (get_order_clause ())
	    set_order_clause ( get_order_clause ()->transform (agg_catalyst));
	}

      return res;
    }



    void select::
    __resolve_item_wildcard (select_item_wild * wi, chain *item_list)
    {
      text *table = 0;
      
      if (wi->get_namespace ())
	{
	  throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. I'm lazy. Sorry.");
	}
      
      if (wi->get_table ())
	{
	  table = wi->get_table ();
	}
      else
	{
	  vector<printable *>::const_iterator it;
	  _make_condensed_table_list ();

	  for (it = _condensed_table_list.begin (); it != _condensed_table_list.end (); ++it)
	    {
	      text *txt = dynamic_cast<text *> (*it);
	      identity *id = dynamic_cast<identity *> (*it);

	      if (txt)
		{
		  id = new identity (0, txt);
		}
	      if (!id)
		{
		  throw exception::invalid_state (string ("Table not of type identity or text:") + _condensed_table_list[0]->get_node_name ());
		}

	      select_item_wild *wi2 = new select_item_wild (0, id->get_table ());
	      __resolve_item_wildcard (wi2, item_list);
	    }

	  if (_condensed_table_list.size () == 0)
	    {
	      throw exception::invalid_state (string ("Used table-less wildcard in select query with") + util::stringify (_condensed_table_list.size ()) + " items");
	    }
	  
	  return;

	}
      
      introspection::table &t = introspection::get_table (unalias_table (table)->unmangled_str ());
      introspection::table::column_const_iterator it;
      
      for (it=t.column_begin (); it != t.column_end (); it++)
	{
	  string name = it->get_name ();
	  item_list->push (new printable_alias (new cast (new identity (0, table, new text (name, IDENTIFIER)), DATA_TYPE_SELECTABLE), new text (name, IDENTIFIER) ) );
	}
    }

    void select::
    __resolve_item_list ()
    {

      /*
	The new item list
      */
      chain *item_list;
      chain::const_iterator it;
      
      item_list = new chain ();
      item_list->set_separator (",");
      item_list->set_line_break (2);
      
      for (it=get_item_list ()->begin (); it != get_item_list ()->end (); ++it)
	{
	  bool handled = false;
	  
	  printable *pr = *it;
	  printable_alias *item = dynamic_cast<printable_alias *> (pr);
	  
	  if (!item)
	    {
	      throw exception::invalid_state ("select item was not of expected type printable_alias");
	    }
	  
	  cast *cast_item = dynamic_cast<cast *>(item->get_item ());
	  select_item_wild * wi = dynamic_cast<select_item_wild *> (item->get_item ());
	  
	  if (cast_item)
	    {
	      item_list->push (pr);
	    }
	  else if (wi)
	    {
	      __resolve_item_wildcard (wi, item_list);
	    }
	  else
	    {
	      throw exception::invalid_state ("Expected child of type cast or type select_item_wild");
	    }

	}
      _set_child (CHILD_ITEM_LIST, item_list);
    }

    void select::
    __pre_calculate ()
    {

      chain::const_iterator it;

      /*
	There are various ways in which Oracle is stricter than MySQL when
	it comes to group by/having clauses that we need to work
	around. If we are using a group by clause, we need to find out
	some information about the query in order to properly transform
	it.
      */  
      if (get_group_clause ())
	{

	  for (it=get_group_clause ()->begin (); it != get_group_clause ()->end (); ++it)
	    {
	      printable *pr = *it;

	      chain *ch = dynamic_cast<chain *> (pr);
	  
	      if (!ch)
		throw shield::exception::invalid_type ("Select query group by-item", "chain");

	      if (!ch->size ())
		throw shield::exception::not_found ("Select query group by-item");
	      
	      identity *id = dynamic_cast<identity *> ((*ch)[0]);
	      
	      __group_field.push_back (id);
	    }

	  for (it=get_item_list ()->begin (); it != get_item_list ()->end (); ++it)
	    {
	      printable *pr = *it;
	      printable_alias *item = dynamic_cast<printable_alias *> (pr);
	      
	      if (item->get_alias ())
		{
		  __field_alias[item->get_alias ()->str ()] = new text (item->get_item ()->str ());
		}

	    }
	}

    }

  }

}

  
