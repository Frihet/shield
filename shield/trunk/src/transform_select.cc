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

    namespace
    {
      /**
	 This function uses introspection to find out what type a table
	 column is of, and use the correct version of the arbitrary
	 aggregation functions defined toghether with the shield
	 package.
      */
      static printable *
      aggregate (text *field, 
		 text *alias,
		 const string &table_alias,
		 const string &table)
      {
	string func_name;

	introspection::table &t = introspection::get_table (table);
	const introspection::column &c = t.get_column (field->str ());
	introspection::column_type y = c.get_type ();

	if (y.is_char ())
	  {
	    func_name = "shield_arb_agg_char";
	  }
	else if (y.is_lob ())
	  {
	    func_name = "shield_arb_agg_clob";
	  } 
	else if (y.is_number ())
	  {
	    func_name = "shield_arb_agg_num";
	  }
	paran *p = new paran (new identity (0, new text (table_alias), field));
	printable *val = new chain (new text (func_name), p);
	printable *res = new printable_alias (val, alias);

	return res;
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
		// cerr << "alias " << al->get_alias ()->str () << " -> " << al->get_item ()->str () << endl;
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
	      stream << item->get_alias ()->str () << "\n";
	    }
	  else
	    {
	      stream << item->get_item ()->str () << "\n";
	    }
	}
      stream << "*/\n";


      if (get_limit_clause ())
	stream << "select"<< *get_item_list () <<" from (";

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
	  stream << "," << *(new printable_alias (new text ("rownum", EXACT),
						  new text ("shield_rownum", EXACT)));
	}

      stream << "\nfrom";

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
      return this->transform (id_catalyst);
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

      if (get_group_clause ())
	{

	  for (it=get_item_list ()->begin (); it != get_item_list ()->end (); ++it)
	    {
	      bool handled = false;

	      printable *pr = *it;
	      printable_alias *item = dynamic_cast<printable_alias *> (pr);
	  
	      /*
		Try and locate all used fields. There are lots of
		situataions where this is not good enough, e.g. when using
		non-cumulative functions or math operators in select
		items.
	      */
	  
	      text *txt = dynamic_cast<text *> (item->get_item ());
	      identity *id = dynamic_cast<identity *> (item->get_item ());

	      if (txt)
		{
		  id = new identity (0, 0, txt);
		}

	      if (id)
		{
		  string table_name = get_table_list ()-> str ();

		  if (id->get_namespace ())
		    {
		      throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. I'm lazy. Sorry.");
		    }

		  if (id->get_table ())
		    {
		      table_name = id->get_table ()->str ();
		    }

		  txt = id->get_field ();

		  if (txt->get_type () == IDENTIFIER || 
		      txt->get_type () == IDENTIFIER_QUOTED)
		    {
		      if (__group_field.find (txt->str ()) == __group_field.end ())
			{
			  string le = oracle_escape (to_upper (txt->str ()));
			  /*		      
					     if (extra_group_list.length () )
					     extra_group_list += ", ";
					     else
					     extra_group_list = "list (";
		      
					     extra_group_list += le;
			  */
			  handled = true;
			  
			  text *alias = item->get_alias ();

			  string unaliased_table_name = unalias_table(new text (table_name))->str ();

			  item_list->push (aggregate (txt, alias, table_name, unaliased_table_name));
			  
			}
		      else
			{
			  item_list->push (item);
			  handled = true;
			}
		    }
		}
	      
	      select_item_wild * wi = dynamic_cast<select_item_wild *> (item);
	      
	      if (wi)
		{
	      
		  string le = "";

		  if (wi->get_namespace ())
		    {
		      throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. I'm lazy. Sorry.");
		    }
	      
		  if (wi->get_table ())
		    {
		      le = wi->get_table ()->str ();
		    }
		  else
		    {
		      le = get_table_list ()-> str ();
		    }


		  introspection::table &t = introspection::get_table (le);
		  
		  introspection::table::column_const_iterator i;

		  for (i=t.column_begin (); i<t.column_end (); i++)
		    {
		      string name = (*i).get_name ();
		      if (__group_field.find (name) == __group_field.end ())
			{

			  string unaliased_table_name = unalias_table(new text (le))->str ();

			  item_list->push (aggregate (new text (name), 0, le, unaliased_table_name));
			}
		      else
			{
			  item_list->push (new printable_alias (new text (name)));
			}
		    }

		  handled = true;
		  
		}

	      if (!handled)
		{
		  item_list->push (*it);
		}
	      
	    }
	  
	}
      else 
	{


	  for (it=get_item_list ()->begin (); it!=get_item_list ()->end (); ++it)
	    {
	      printable *pr = *it;
	      printable_alias *item = dynamic_cast<printable_alias *> (pr);

	      if (!item)
		throw shield::exception::invalid_type ("Select query item list", "printable_alias");
	      

	      select_item_wild * wi = dynamic_cast<select_item_wild *> (item);
	      
	      if (wi)
		{

		  string le;

		  if (wi->get_namespace ())
		    {
		      throw exception::syntax ("Table namespaces not supported in combination with wildcards and limit clauses. Yes, this may seem like a pretty arbitrary limitation. Sorry.");
		    }

		  if (wi->get_table ())
		    {
		      le = wi->get_table ()->str ();
		    }
		  else
		    {
		      catalyst::find_table cat (this);
		      get_table_list ()->transform (cat);
		      if (!cat.get_table_list ().size ())
			{
			  throw shield::exception::not_found ("Select query table");
			}
		      le = (*cat.get_table_list ().begin ())->str ();
		    }

		  introspection::table &t = introspection::get_table (le);

		  introspection::table::column_const_iterator it;

		  for (it=t.column_begin (); it!=t.column_end (); ++it)
		    {
		      item_list->push (new printable_alias (new text ((*it).get_name (),IDENTIFIER), 0));
		    }
		}
	      else
		{
		  item_list->push (item);
		}
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

	      string id = (*ch)[0]->str ();
	      __group_field.insert (id);
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

