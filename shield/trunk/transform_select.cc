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
      : __limit_clause (0), 
	__item_list (0),
	__table_list (0), 
	__where_clause (0),
	__group_clause (0),
	__having_clause (0), 
	__order_clause (0), 
	__procedure_clause (0), 
	__into_clause (0)
    {
    }

    void print_having (ostream &stream, const printable *having, map<string, printable *> &mapping)
    {
      replace_identifier_catalyst cat (mapping);

      const printable *t = having->transform (cat);
      stream << *t;
    }

    static printable *aggregate (text *field, text *alias, string table)
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

      printable *val = new chain (new text (func_name), new paran (new identity (0, new text (table), field)));
      printable *res = new select_item (val, alias);

      return res;
    }

    void select::
    print (ostream &stream) const
    {

      std::ostringstream pre, post;
  
      chain::const_iterator i;

      bool has_wild=false;
      bool has_ungrouped=false;
  
      map<string, printable *> alias_lookup;
      set<string> group_field;

  
      if (!__item_list)
	{
	  throw exception::syntax ("No item list for select");
	}

      if (__limit_clause)
	{
	  __item_list->push (new select_item (new text ("rownum", EXACT), new text ("shield_rownum", EXACT)));
	}
      /*
	Find out if there are any wildcards among the select items.  When
	using group by or limit clauses, we need to special case
	wildcards.
      */
      for (i=__item_list->begin (); i<__item_list->end (); i++)
	{
	  printable *pr = *i;
	  select_item *it = dynamic_cast<select_item *> (pr);
	  assert (it);
      
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
  
      if (__group_clause)
	{

	  for (i=__group_clause->begin (); i<__group_clause->end (); i++)
	    {
	      printable *pr = *i;

	      chain *ch = dynamic_cast<chain *> (pr);
	  
	      assert (ch);
	      assert (ch->size ()>=1);

	      string id = (*ch)[0]->str ();
	      group_field.insert (id);
	    }

	  for (i=__item_list->begin (); i<__item_list->end (); i++)
	    {
	      printable *pr = *i;
	      select_item *it = dynamic_cast<select_item *> (pr);
	  
	      if (it->get_alias ())
		{
		  alias_lookup[it->get_alias ()->str ()] = new text (it->get_item ()->str ());
		}

	      /*
		Try and locate all used fields. There are lots of
		situataions where this is not good enough, e.g. when using
		non-cumulative functions or math operators in select
		items.

		This should be implemented as a transform instead.
	      */
	  
	      text *txt = dynamic_cast<text *> (it->get_item ());
	  
	      if (txt)
		{
		  if (txt->get_type () == IDENTIFIER || 
		      txt->get_type () == IDENTIFIER_QUOTED)
		    {
		      if (group_field.find (txt->str ()) == group_field.end ())
			{
			  has_ungrouped = true;
			}
		    }
		}
	    }
	}

      if (__limit_clause)
	pre << "select * from (";

      pre << "select";

      if (__option_clause)
	pre << *__option_clause;

      post << "from";

      if (__table_list && !__table_list->empty ())
	{
	  post << *__table_list;
	}
      else
	{
	  post << " dual";
	}

      if (__where_clause)
	{
	  post << "\nwhere" << *__where_clause;
	}

      if (__group_clause)
	{
	  post << "\ngroup by" << *__group_clause;
	}

      if (__having_clause)
	{
	  post << "\nhaving";
	  print_having (post, __having_clause, alias_lookup);
	}

      if (__order_clause)
	{
	  post << "\norder by" << *__order_clause;
	}

      if (__limit_clause )
	post << ") where" << *__limit_clause;

      /*
	Handle special case of limit+wildcards
      */

      chain *item_list;

      if (__limit_clause && has_wild)
	{

	  item_list = new chain ();
	  item_list->set_separator (",");

	  if (__group_clause)
	    {
	      throw exception::syntax ("Limit clauses, wildcards and group clauses in the same query is not supported");
	    }

	  for (i=__item_list->begin (); i<__item_list->end (); i++)
	    {
	      printable *pr = *i;
	      select_item *it = dynamic_cast<select_item *> (pr);
	      assert (it);

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
		      le = __table_list-> str ();
		    }

		  introspection::table &t = introspection::get_table (le);

		  introspection::table::column_const_iterator i;

		  for (i=t.column_begin (); i<t.column_end (); i++)
		    {
		      item_list->push (new select_item (new text ((*i).get_name (),IDENTIFIER), 0));
		    }
		}
	      else
		{
		  item_list->push (it);
		}
	    }

	}
      else if (__group_clause && (has_wild || has_ungrouped))
	{

	  item_list = new chain ();
	  item_list->set_separator (",");

	  for (i=__item_list->begin (); i<__item_list->end (); i++)
	    {
	      bool handled = false;

	      printable *pr = *i;
	      select_item *it = dynamic_cast<select_item *> (pr);
	  
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
		  string table_name = __table_list-> str ();

		  if (id->get_namespace ())
		    {
		      throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. Sorry.");
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
			  item_list->push (aggregate (txt, alias, table_name));
			  
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
		      throw exception::syntax ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. Sorry.");
		    }
	      
		  if (wi->get_table ())
		    {
		      le = wi->get_table ()->str ();
		    }
		  else
		    {
		      le = __table_list-> str ();
		    }


		  introspection::table &t = introspection::get_table (le);
		  
		  introspection::table::column_const_iterator i;

		  for (i=t.column_begin (); i<t.column_end (); i++)
		    {
		      string name = (*i).get_name ();
		      if (group_field.find (name) == group_field.end ())
			{
			  item_list->push (aggregate (new text (name), 0, le));
			}
		      else
			{
			  item_list->push (new select_item (new text (name), 0));
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
      else
	{
	  item_list = __item_list;
	}
      
      stream << pre.str () << *item_list << "\n" << post.str () << endl << endl;      
  
    }

  }

}
