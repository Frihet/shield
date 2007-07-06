/**
   @file transform_select.cc

   This is the file for writing out Oracle style sql code for a select
   query. It is probably the single most complicated query, since
   there are so many optional query parts, and so many of them work
   differently in Oracle. The code is a bit ugly in places and there
   are a few situations where valid but rare cases are not handeld
   correctly. Needs a bit more work.
*/
#include <set>

#include "transform.hh"
#include "introspection.hh"

using namespace shield;

namespace shield
{

  namespace transform
  {

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
      replace_identifier_catalyst cat (mapping);

      printable *t = having->transform (cat);
      stream << *t;
    }

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
	  chain::const_iterator i;
	  for (i=ch->begin (); i<ch->end (); i++)
	    {
	      get_table_alias (*i, table_alias);
	    }
	}
    }

    void select::
    _print (ostream &stream)
    {

      std::ostringstream pre, post;
  
      chain::const_iterator i;

      /**
	 This is set to true if the field list contains wildcards
      */
      bool has_wild=false;
      
      /**
	 This is set to true of there are fields that are not grouped on
      */
      bool has_ungrouped=false;
  
      /*
	Mappings from field aliases to the underlying field
	definition. The field definition may be an expression.
      */
      map<string, printable *> field_alias;

      /**
	 Mapping between table aliases and the actual table names.
      */
      map<string, printable *> table_alias;

      /**
	 Set of fields on which grouping is performed
      */
      set<string> group_field;

      if (!get_item_list ())
	{
	  throw exception::syntax ("No item list for select");
	}

      /*
	We use the magic rownum field to simulate the limit clause.

	Since rownum is only increased on actually used rows, we need
	to assign rownum to a temporary field and perform the real
	limit in the outer select.
      */
      if (get_limit_clause ())
	{
	  get_item_list ()->push (new printable_alias (new text ("rownum", EXACT),
						  new text ("shield_rownum", EXACT)));
	}

      /*
	Find out if there are any wildcards among the select items.  When
	using group by or limit clauses, we need to special case
	wildcards.
      */
      for (i=get_item_list ()->begin (); i<get_item_list ()->end (); i++)
	{
	  printable *pr = *i;
	  printable_alias *it = dynamic_cast<printable_alias *> (pr);
	  if (!it)
	    throw shield::exception::invalid_type ("Select query item list", "printable_alias");

      
	  if (dynamic_cast<select_item_wild *> (it))
	    {
	      has_wild = true;
	      break;
	    }
	}

      /*
	There are various ways in which Oracle is stricter than MySQL when
	it comes to group by/having clauses that we need to work
	around. If we are using a group by clause, we need to find out
	some information about the query in order to properly transform
	it.
      */  
      if (get_group_clause ())
	{

	  for (i=get_group_clause ()->begin (); i<get_group_clause ()->end (); i++)
	    {
	      printable *pr = *i;

	      chain *ch = dynamic_cast<chain *> (pr);
	  
	      if (!ch)
		throw shield::exception::invalid_type ("Select query group by-item", "chain");

	      if (!ch->size ())
		throw shield::exception::not_found ("Select query group by-item");

	      string id = (*ch)[0]->str ();
	      group_field.insert (id);
	    }

	  get_table_alias (get_table_list (), table_alias);
	  
	  for (i=get_item_list ()->begin (); i<get_item_list ()->end (); i++)
	    {
	      printable *pr = *i;
	      printable_alias *it = dynamic_cast<printable_alias *> (pr);
	  
	      if (it->get_alias ())
		{
		  field_alias[it->get_alias ()->str ()] = new text (it->get_item ()->str ());
		}

	      /*
		Try and locate all used fields. There are lots of
		situataions where this is not good enough, e.g. when using
		non-cumulative functions or math operators in select
		items.

		This should be implemented as a transform instead.
	      */
	  
	      text *txt = dynamic_cast<text *> (it->get_item ());
	      identity *id = dynamic_cast<identity *> (it->get_item ());
	  
	      if (txt)
		{
		  id = new identity (0, 0, txt);
		}

	      if (id)
		{

		  txt = id->get_field ();

		  if (txt->get_type () == IDENTIFIER || 
		      txt->get_type () == IDENTIFIER_QUOTED)
		    {
		      if (group_field.find (id->str ()) == group_field.end ())
			{
			  has_ungrouped = true;
			}
		    }
		}
	    }
	}

      if (get_limit_clause ())
	pre << "select * from (";

      pre << "select";

      if (get_option_clause ())
	pre << *get_option_clause ();

      post << "from";

      if (get_table_list () && !get_table_list ()->empty ())
	{
	  post << *get_table_list ();
	}
      else
	{
	  post << " dual";
	}

      if (get_where_clause ())
	{
	  post << "\nwhere" << *get_where_clause ();
	}

      if (get_group_clause ())
	{
	  post << "\ngroup by" << *get_group_clause ();
	}

      if (get_having_clause ())
	{
	  post << "\nhaving";
	  print_having (post, get_having_clause (), field_alias);
	}

      if (get_order_clause ())
	{
	  post << "\norder by" << *get_order_clause ();
	}

      if (get_limit_clause () )
	post << ") where" << *get_limit_clause ();

      chain *item_list;
      
      if (get_group_clause () && (has_wild || has_ungrouped))
	{

	  item_list = new chain ();
	  item_list->set_separator (",");

	  for (i=get_item_list ()->begin (); i<get_item_list ()->end (); i++)
	    {
	      bool handled = false;

	      printable *pr = *i;
	      printable_alias *it = dynamic_cast<printable_alias *> (pr);
	  
	      /*
		Try and locate all used fields. There are lots of
		situataions where this is not good enough, e.g. when using
		non-cumulative functions or math operators in select
		items.
	      */
	  
	      text *txt = dynamic_cast<text *> (it->get_item ());
	      identity *id = dynamic_cast<identity *> (it->get_item ());

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
		      if (group_field.find (txt->str ()) == group_field.end ())
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
			  
			  text *alias = it->get_alias ();
			  
			  string unaliased_table_name = table_name;
			  if (table_alias.find (table_name) != table_alias.end ())
			    unaliased_table_name = table_alias[table_name]->str ();

			  item_list->push (aggregate (txt, alias, table_name, unaliased_table_name));
			  
			}
		      else
			{
			  item_list->push (it);
			  handled = true;
			}
		    }
		}
	      
	      select_item_wild * wi = dynamic_cast<select_item_wild *> (it);
	      
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
		      if (group_field.find (name) == group_field.end ())
			{

			  string unaliased_table_name = le;
			  if (table_alias.find (le) != table_alias.end ())
			    unaliased_table_name = table_alias[le]->str ();

			  item_list->push (aggregate (new text (name), 0, le, unaliased_table_name));
			}
		      else
			{
			  item_list->push (new printable_alias (new text (name), 0));
			}
		    }

		  handled = true;
		  
		}

	      if (!handled)
		{
		  item_list->push (*i);
		}
	      
	    }
	  
	}
      else if (true)//get_limit_clause () && has_wild)
	{

	  /**
	     We shouldn't really expand all wildcards, but it is far
	     easier to always do it than to figure out all the
	     situations when it should happen.
	  */

	  item_list = new chain ();
	  item_list->set_separator (",");

	  for (i=get_item_list ()->begin (); i<get_item_list ()->end (); i++)
	    {
	      printable *pr = *i;
	      printable_alias *it = dynamic_cast<printable_alias *> (pr);
	      if (!it)
		throw shield::exception::invalid_type ("Select query item list", "printable_alias");
	      

	      select_item_wild * wi = dynamic_cast<select_item_wild *> (it);
	      
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
		      find_table_catalyst cat (this);
		      get_table_list ()->transform (cat);
		      le = (*cat.get_table_list ()->begin ())->str ();
		    }

		  introspection::table &t = introspection::get_table (le);

		  introspection::table::column_const_iterator i;

		  for (i=t.column_begin (); i<t.column_end (); i++)
		    {
		      item_list->push (new printable_alias (new text ((*i).get_name (),IDENTIFIER), 0));
		    }
		}
	      else
		{
		  item_list->push (it);
		}
	    }
	}
      else
	{
	  item_list = get_item_list ();
	}
      
      stream << pre.str () << *item_list << "\n" << post.str ();

      if (!get_parent ())
	stream << endl << endl;      
  
    }

    chain *select::
    _get_condensed_table_list ()
    {
      find_table_catalyst cat (this);
      get_table_list ()->transform (cat);
      return cat.get_table_list ();
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
      identity_catalyst i (this);
      return this->transform (i);
    }

  }

}
