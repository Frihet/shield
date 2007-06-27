#include <set>
#include "transform.hh"

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
    throw exception::syntax_exception ("No item list for select");

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

  post << ";";

  /*
    Handle special case of limit+wildcards
  */
  if (__limit_clause && has_wild)
    {

      chain *item_list = new chain ();
      string table_list;

      item_list->set_separator (",");

      if (__group_clause)
	{
	  throw exception::syntax_exception ("Limit clauses, wildcards and group clauses in the same query is not supported");
	}

      for (i=__item_list->begin (); i<__item_list->end (); i++)
	{
	  printable *pr = *i;
	  select_item *it = dynamic_cast<select_item *> (pr);
	  assert (it);

	  select_item_wild * wi = dynamic_cast<select_item_wild *> (it);

	  if (wi)
	    {

	      string le = "";

	      if (wi->get_namespace ())
		{
		  throw exception::syntax_exception ("Table namespaces not supported in combination with wildcards and limit clauses. Yes, this may seem like a pretty arbitrary limitation. Sorry.");
		}

	      if (wi->get_table ())
		{
		  le = wi->get_table ()->str ();
		}
	      else
		{
		  le = __table_list-> str ();
		}
	      
	      le = oracle_escape (to_upper (le));

	      if (table_list.length () )
		table_list += ", ";
	      else
		table_list = "list (";
	      
	      table_list += le;

	    }
	  else
	    {
	      item_list->push (it);
	    }
	}

      table_list += ")";

      pre << *item_list;

      stream << "declare" << endl;
      stream << "\tres number;" << endl;
      stream << "begin" << endl;
      stream << "\tres := shield.make_select_limit_query (" << oracle_escape (pre.str ()) << "," << oracle_escape (post.str ()) << "," << table_list << ");"<< endl;
      stream << "end;" << endl << "/" << endl << endl;
    }
  else if (__group_clause && (has_wild || has_ungrouped))
    {

      string agg_list = "";
      string table_list;
      chain *item_list = new chain ();

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
	  
	  if (txt)
	    {
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

		      if (agg_list.length () )
			agg_list += ", ";
		      else
			agg_list = "list (";
	      
		      agg_list += le;
		      
		    }
		}
	    }
	  	  
	  
	  select_item_wild * wi = dynamic_cast<select_item_wild *> (it);
	  
	  if (wi)
	    {
	      
	      string le = "";

	      if (wi->get_namespace ())
		{
		  throw exception::syntax_exception ("Table namespaces not supported in combination with wildcards and group clauses. Yes, this may seem like a pretty arbitrary limitation. Sorry.");
		}
	      
	      if (wi->get_table ())
		{
		  le = wi->get_table ()->str ();
		}
	      else
		{
		  le = __table_list-> str ();
		}
	      
	      le = oracle_escape (to_upper (le));
	      
	      if (table_list.length () )
		table_list += ", ";
	      else
		table_list = "list (";
	      
	      table_list += le;
	      handled = true;
	    }
	  
	  
	  if (!handled)
	    {
	      item_list->push (txt);
	    }
	  
	}
      
      if (!table_list.length () )
	table_list = "list (";
      
      if (!agg_list.length () )
	agg_list = "list (";
	       
      agg_list += ")";
      table_list += ")";
      
      pre << *item_list;

      stream << "declare" << endl;
      stream << "\tres number;" << endl;
      stream << "begin" << endl;
      stream << "\tres := shield.make_select_group_query (" << oracle_escape (pre.str ()) << "," << oracle_escape (post.str ()) << "," << table_list << "," << agg_list << ");"<< endl;
      stream << "end;" << endl << "/" << endl << endl;
      
    }
  else
    {
      stream << pre.str () << *__item_list << "\n" << post.str () << endl << endl;      
    }

}

}

}
