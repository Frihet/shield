
#include <assert.h>

#include "transform.hh"
#include "catalyst.hh"
#include "exception.hh"
#include "introspection.hh"

namespace shield
{

  namespace transform
  {

    create_table::create_table()
      : __check (false)
    {
      __prev_index_list = new chain ();
    }

    void 
    create_table::set_check_existance (bool check)
    {
      __check = check; 
    }

    void create_table::
    post_render (ostream &stream, attribute *attr)
    {

      printable *post = attr->get_post_render ();
      if (post)
	{
	  stream << *post;

	  create_index *idx = dynamic_cast<create_index *> (post);
	  if (idx)
	    {
	      __prev_index_list->push (idx);
	    }
	}
    }

    void create_table::
    _print (ostream &stream)
    {
      if (!get_name () || (!get_field_list () && !get_like_clause ()))
	throw exception::invalid_state ("Required child nodes unset in create table node");
  
      if (__check)
	{
	  introspection::table &t = introspection::get_table (get_name ()->str ());
	  if (t.exists ())
	    return;
	}

      stream << "create table" << *get_name () << endl;

      if (get_like_clause ())
	{
	  stream << "like" << *get_like_clause ();
	}
      else
	{
	  stream << "(" << *get_field_list () << " )" ;
	}

      stream << endl << endl;

      if (get_field_list ())
	{
	  /*
	    Search through the field list for auto_increment columns.
	  */
	  map<string, printable *>prop;
	  prop["table_name"] = get_name ();

	  chain::const_iterator i;
	  for (i=get_field_list ()->begin (); i<get_field_list ()->end (); i++ )
	    {
	      field_spec *fs = dynamic_cast<field_spec *> (*i);
	      assert (fs);
	      if (fs->get_attribute ())
		{
		  chain *attr_list = dynamic_cast<chain *> (fs->get_attribute ());
		  prop["field_name"] = fs->get_name ();
	      
		  if (attr_list)
		    {
		      chain::const_iterator j;
		  
		      for (j=attr_list->begin (); j<attr_list->end (); j++ )
			{
			  attribute *attr = dynamic_cast<attribute *> (*j);
			  stream << sep;
			  post_render (stream, attr);
			  stream << endl << endl;
			}
		    }
		}
	    }
	}

      if (get_key_list ())
	{
	  /*
	    Search through the key list for auto_increment columns.
	  */
	  map<string, printable *>prop;
	  prop["table_name"] = get_name ();
	  prop["field_list"] = get_field_list ();

	  chain::const_iterator i;

	  for (i=get_key_list ()->begin (); i<get_key_list ()->end (); i++ )
	    {
	      attribute *attr = dynamic_cast<attribute *> (*i);
	      stream << sep;
	      post_render (stream, attr);
	      stream << endl << endl;
	    }
	}
    }

  }

}
