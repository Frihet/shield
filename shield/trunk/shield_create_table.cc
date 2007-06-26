
#include <assert.h>
#include "shield.hh"
#include "shield_exception.hh"

namespace shield
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
}

void 
create_table::set_field_list (chain *l)
{
  l->set_line_break (1);
  __field_list = l;
}

void 
create_table::set_key_list (chain *l)
{
  l->set_line_break (1);
  __key_list = l;
}


void 
create_table::set_create_options (printable *l)
{
  __create_options = l;
}

void 
create_table::set_table_options (printable *l)
{
  __table_options = l;
}


void 
create_table::set_initial_data (printable *l)
{
  __initial_data = l;
}


void 
create_table::set_name (printable *l)
{
  __name = l;
}


void 
create_table::set_like_clause (printable *l)
{
  __like_clause = l;
}


void 
create_table::set_check_existance (bool check)
{
  __check = check; 
}

void create_table::
post_render (ostream &stream, attribute *attr, map<string, printable *> &prop) const
{
  /**
     'this2' is an ugly hack - this function is declared const, but it
     modifies the __prop variable. This should be ok, because __prop
     only contains info on the rest of the parse tree - rerunning
     print should yield the same result. We do this using a
     const_cast.
  */
  create_table *this2 = const_cast<create_table *> (this);

  printable *post = attr->get_post_render ();
  if (post)
    {
      post->set_property (prop);
      stream << *post;

      create_index *idx = dynamic_cast<create_index *> (post);
      if (idx)
	{
	  printable *idx_list = prop["index"];
	  if (!idx_list)
	    {
	      idx_list = new chain ();
	      prop["index"] = idx_list;
	    }
	  
	  dynamic_cast<chain *> (idx_list)->push (idx);
	}
    }
}

void create_table::
print (ostream &stream) const
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

  stream << ";" << endl << endl;

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
		      post_render (stream, attr, prop);
		    }
		}
	    }
	}
      stream << endl << endl;
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
	  post_render (stream, attr, prop);
	}
      stream << endl << endl;
    }
}

}
