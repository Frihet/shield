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
      : __table_alias_init (false)
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
    get_table_alias (chain *ch, map<string, printable *> &table_alias)
    {
      chain::const_iterator it;
      printable_alias *al;      
      chain *sub_ch;

      if (ch)
	{
	  for (it=ch->begin (); it != ch->end (); ++it)
	    {
	      al = dynamic_cast<printable_alias *> (*it);
	      sub_ch = dynamic_cast<chain *> (*it);

	      if (al && al->get_alias ())
		{
		  table_alias[al->get_alias ()->str ()] = al->get_item ();
		}
	      else if (sub_ch)
		{
		  get_table_alias (sub_ch, table_alias);
		}
	    }
	}
    }

    void select::
    __print_comment (ostream &stream)
    {
      chain::const_iterator it;
      bool is_sub= is_subselect ();
      printable *pr;
      printable_alias *item;

      printable *sub_item;
      identity *sub_item_identity;

      if (!is_sub)
	{
	  stream << "/*\n";
	  for (it=get_item_list ()->begin (); it!=get_item_list ()->end (); ++it)
	    {
	      pr = *it;
	      item = dynamic_cast<printable_alias *> (pr);
	      
	      if (!item)
		throw shield::exception::invalid_type ("Select query item list", "printable_alias");

	      if (item->get_alias ())
		{
		  stream << item->get_alias ()->unmangled_str () << "\n";
		}
	      else
		{
		  sub_item = item->get_item ();
		  sub_item_identity = dynamic_cast<identity *> (sub_item);
		  if (sub_item_identity)
		    {
		      sub_item = sub_item_identity->get_field ();
		    }
		  
		  stream << util::identifier_unescape (sub_item->str ()) << "\n";
		}
	    }
	  stream << "*/\n";
	}

    }

    void select::
    _print (ostream &stream)
    {
      chain::const_iterator it;
      bool is_sub= is_subselect ();
      printable *pr;
      printable_alias *item;

      if (!get_item_list ())
	{
	  throw exception::syntax ("No item list for select");
	}

      __print_comment (stream);

      if (get_limit_clause ())
	{

	  /*
	    Ouch, we're facing a limit clause. These are insanely complex to implement in Oracle, because of how row numbers work w.r.t. ordering, filetering, etc.

	    Basically, according to ask tom and other Oracle experts, this is the proper way to do a 'limit' in Oracle:

	    select alias1, alias2, alias3... from (
	    select foo.*, rownum r from (
	    select ...
	    ) where rownum < MAX)
	    where r > MIN
	  */

	  stream << "select";

	  for (it=get_item_list ()->begin (); it!=get_item_list ()->end (); ++it)
	    {
	      pr = *it;	      
	      item = dynamic_cast<printable_alias *> (pr);

	      if (!item)
		throw shield::exception::invalid_type ("Select query item list", "printable_alias");
	      
	      if (it != get_item_list ()->begin ())
		stream << ",";

	      stream << " " << item->get_internal_alias ();

	    }
	  
	  stream << " from (\n";
	  /*
	    We use the magic rownum field to simulate the limit clause.
	    
	    Since rownum is only increased on actually used rows, we need
	    to assign rownum to a temporary field and perform the real
	    limit in the outer select.

	    The weird first_rows comment is an Oracle hint that is
	    supposed to make thid type of query much faster. (Never
	    verified it)
	  */
	  stream << "select /*+ first_rows(" << get_limit_clause ()->get_upper_limit () << ") */ shield_limit.*, rownum shield_rownum from (\n";
	}
      
      stream << "select";
      
      if (get_option_clause ())
	stream << *get_option_clause ();

      stream << *get_item_list ();

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
	{
	  stream << ")\nshield_limit where rownum <= " <<get_limit_clause ()->get_upper_limit () << ")\n";
	  stream << " where shield_rownum > " << get_limit_clause ()->get_lower_limit ();

	}

      if (!is_sub)
	{
	  stream << endl << endl << sep;
	}
    }

    void select::
    _make_condensed_table_list ()
    {
      catalyst::find_table cat (this);      

      if (_condensed_table_list.size ())
	return;

      get_table_list ()->transform (cat);
      _condensed_table_list = cat.get_table_list ();
    }    

    text *select::
    unalias_table (text *alias)
    {
      map<string, printable *>::const_iterator it;

      printable *res;   
      text *txt;
      identity *id;

      if (!__table_alias_init)
	{
	  __table_alias_init = true;
	  get_table_alias (get_table_list (), __table_alias);
	}

      it = __table_alias.find (alias->str ());
      
      if (it != __table_alias.end ())
	{
	  res = it->second;
	  txt = dynamic_cast<text *> (res);
	  id = dynamic_cast<identity *> (res);
	  
	  if (id)
	    {
	      txt = id->get_table ();
	    }

	  if (!id)
	    throw shield::exception::invalid_type ("Unaliased table name", "text");
	  
	  return new text (txt);
	}
      return alias;
    }

    printable *select::
    internal_transform ()
    {

      catalyst::create_identity id_catalyst (this);
      catalyst::set_selectable sel_catalyst (true);
      printable *res;

      __pre_calculate ();
      
      __resolve_item_list ();
      
      res= this->transform (id_catalyst);
      
      if (get_group_clause ())
	{
	  catalyst::aggregate agg_catalyst (this, __group_field);
	  
	  set_item_list ( dynamic_cast<chain *> (get_item_list ()->transform (agg_catalyst)));
	  if (get_order_clause ())
	    set_order_clause (get_order_clause ()->transform (agg_catalyst));
	}

      set_item_list ( dynamic_cast<chain *> (get_item_list ()->transform (sel_catalyst)));

      return res;
    }

    void select::
    __resolve_all_table_fields (chain *item_list)
    {
      text *txt;
      identity *id;
      vector<printable *>::const_iterator it;

      _make_condensed_table_list ();

      for (it = _condensed_table_list.begin (); it != _condensed_table_list.end (); ++it)
	{
	  txt = dynamic_cast<text *> (*it);
	  id = dynamic_cast<identity *> (*it);

	  if (txt)
	    {
	      id = new identity (0, txt, 0);
	    }
	  
	  if (!id << !id->get_table ())
	    {
	      throw exception::invalid_state (string ("Table not of type identity or text:") + _condensed_table_list[0]->get_node_name ());
	    }
	  
	  select_item_wild *wi2 = new select_item_wild (0, new text (id->get_table ()));
	  __resolve_item_wildcard (wi2, item_list);
	}
      
      if (_condensed_table_list.size () == 0)
	{
	  throw exception::invalid_state (string ("Used table-less wildcard in select query with") + util::stringify (_condensed_table_list.size ()) + " items, got zero fields");
	}
    }


    void select::
    __resolve_item_wildcard (select_item_wild * wi, chain *item_list)
    {
      text *table = 0;
      introspection::table::column_const_iterator meta_it;
      
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
	  __resolve_all_table_fields (item_list);
	  return;
	}
      
      introspection::table &meta_table = introspection::get_table (unalias_table (table)->unmangled_str ());
      
      for (meta_it=meta_table.column_begin (); meta_it != meta_table.column_end (); meta_it++)
	{
	  string name = meta_it->get_name ();
	  text *table_copy = 0;

	  if (table)
	    table_copy = new text (table);

	  identity *new_id = new identity (0, table_copy, new text (name, IDENTIFIER));
	  text *new_alias = new text (name, IDENTIFIER);
	  cast *new_cast = new cast (new_id, DATA_TYPE_SELECTABLE);
	  item_list->push (new printable_alias (new_cast, new_alias, true ));
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
      
      printable *pr;
      printable_alias *item;
	  
      cast *cast_item;
      select_item_wild * wi;

      item_list = new chain ();
      item_list->set_separator (",");
      item_list->set_line_break (2);
      
      for (it=get_item_list ()->begin (); it != get_item_list ()->end (); ++it)
	{
	  
	  pr = *it;
	  item = dynamic_cast<printable_alias *> (pr);
	  
	  if (!item)
	    {
	      throw exception::invalid_state ("select item was not of expected type printable_alias");
	    }
	  
	  cast_item = dynamic_cast<cast *>(item->get_item ());
	  wi = dynamic_cast<select_item_wild *> (item->get_item ());
	  
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

      printable *pr;
      chain *ch;
      identity *id;

      printable_alias *item;

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
	      pr = *it;
	      ch = dynamic_cast<chain *> (pr);
	  
	      if (!ch)
		throw shield::exception::invalid_type ("Select query group by-item", "chain");

	      if (!ch->size ())
		throw shield::exception::not_found ("Select query group by-item");
	      
	      id = dynamic_cast<identity *> ((*ch)[0]);
	      
	      if (id)
		{
		  __group_field.push_back (id);
		}
	    }

	  for (it=get_item_list ()->begin (); it != get_item_list ()->end (); ++it)
	    {
	      pr = *it;
	      item = dynamic_cast<printable_alias *> (pr);
	      
	      if (item->get_alias ())
		{
		  __field_alias[item->get_alias ()->str ()] = new text (item->get_item ()->str ());
		}

	    }
	}

    }

    bool select::
    is_subselect ()
    {
      query *parent_query;
      printable *parent = get_parent ();
      
      if (parent)
	{
	  parent_query = parent->get_query ();
	  if( dynamic_cast<select *> (parent_query))
	    {
	      return true;
	    }
	}
      return false;
    }

  }

}

  
  
