
#include <assert.h>
#include "transform.hh"

namespace shield
{

  namespace transform
  {

    create_table::create_table()
      : __field_list (0), 
	__create_options (0),
	__table_options (0),
	__initial_data (0), 
	__name (0),
	__check (false),
	__like_clause (0)
    {
      __prev_index_list = new chain ();
    }

    void 
    create_table::set_field_list (chain *l)
    {
      l->set_line_break (1);
      __field_list = l;
      if (l)
	l->set_parent (this);
    }

    void 
    create_table::set_key_list (chain *l)
    {
      l->set_line_break (1);
      __key_list = l;
      if (l)
	l->set_parent (this);
    }


    void 
    create_table::set_create_options (printable *l)
    {
      __create_options = l;
      if (l)
	l->set_parent (this);
    }

    void 
    create_table::set_table_options (printable *l)
    {
      __table_options = l;
      if (l)
	l->set_parent (this);
    }


    void 
    create_table::set_initial_data (printable *l)
    {
      __initial_data = l;
      if (l)
	l->set_parent (this);
    }


    void 
    create_table::set_name (printable *l)
    {
      __name = l;
      if (l)
	l->set_parent (this);
    }


    void 
    create_table::set_like_clause (printable *l)
    {
      __like_clause = l;
      if (l)
	l->set_parent (this);
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
      assert (__name);
      assert (__field_list || __like_clause);
  
      stream << "create table" << *__name << endl;

      if (__like_clause)
	{
	  stream << "like" << *__like_clause;
	}
      else
	{
	  stream << "(" << *__field_list << " )" ;
	}

      stream << endl << endl;

      if (__field_list)
	{
	  /*
	    Search through the field list for auto_increment columns.
	  */
	  map<string, printable *>prop;
	  prop["table_name"] = __name;

	  chain::const_iterator i;
	  for (i=__field_list->begin (); i<__field_list->end (); i++ )
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

      if (__key_list)
	{
	  /*
	    Search through the key list for auto_increment columns.
	  */
	  map<string, printable *>prop;
	  prop["table_name"] = __name;
	  prop["field_list"] = __field_list;

	  chain::const_iterator i;

	  for (i=__key_list->begin (); i<__key_list->end (); i++ )
	    {
	      attribute *attr = dynamic_cast<attribute *> (*i);
	      stream << sep;
	      post_render (stream, attr);
	      stream << endl << endl;
	    }
	}
    }


    printable *create_table::
    transform (catalyst &catalyst)
    {

      if (__field_list)
	__field_list = dynamic_cast<chain *> (__field_list->transform (catalyst));

      if (__key_list)
	__key_list = dynamic_cast<chain *> (__key_list->transform (catalyst));

      if (__create_options)
	__create_options = __create_options->transform (catalyst);

      if (__table_options)
	__table_options = __table_options->transform (catalyst);

      if (__initial_data)
	__initial_data = __initial_data->transform (catalyst);

      if (__name)
	__name = __name->transform (catalyst);

      if (__like_clause)
	__like_clause = __like_clause->transform (catalyst);

      return catalyst (this);
    }




  }

}
