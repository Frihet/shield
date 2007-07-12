#include "include/transform.hh"
#include "include/exception.hh"

namespace shield
{

  namespace transform
  {

    /*
      Reparent this node as a new query, but first find out a bit about the table, like what field we operate on
    */
    printable *auto_increment::
    internal_transform (void)
    {
      printable *node;
      field_spec *spec;
      __table_query = dynamic_cast<create_table *> (get_parent ()->get_query ());

      
      if (!__table_query)
	{
	  throw exception::invalid_state ("auto_increment query found with non-create table-query");
	}
     
      node = this;
      while (true)
	{
	  
	  if (!node)
	    {
	      throw exception::invalid_state ("Could not locate field for auto_increment column");
	    }
	  spec = dynamic_cast<field_spec *> (node);
	  if (spec)
	    break;

	  node = node->get_parent ();
	}

      __field_name = spec->get_name ();

      _add_query (this);

      return 0;
    }

    /**
       Print 
    */
    void auto_increment::
    _print (ostream &stream)
    {

      if (!__table_query)
	throw exception::invalid_state ("auto_increment column with no table information");

      if (!__field_name)
	throw exception::invalid_state ("auto_increment column has no field name");
      
      string t_name = __table_query->get_name ()->str ();
      string f_name = __field_name->str ();

      string name = t_name + "_" + f_name;

      /*
	FIXME:

	Truncate index name at 27 characters since oracle has a 30
	character maximum on field names. This can lead to clashes!
      */
      if (name.length () > 27)
	{
	  name = t_name.substr(0, 15) + "_" + f_name;
	  name = name.substr(0, 27);
	}

      drop_item (stream, "sequence", name + "_s_");

      stream << "create sequence " << name << "_s_" << endl;
      stream << "start with 1" << endl;
      stream << "increment by 1" << endl;
      stream << "nomaxvalue" << endl << endl;
  
      stream << sep;
  
      stream << "create or replace trigger " << name << "_t_" << endl;
      stream << "before insert on " << t_name << endl;
      stream << "for each row" << endl;
      stream << "begin" << endl;
      stream << "\tselect " << name << "_s_.nextval into :new." << f_name << " from dual;" << endl;
      stream << "\tselect " << name << "_s_.currval into shield.last_insert_id from dual;" << endl;
      stream << "end;" <<endl;
      stream << "/" << endl << endl << sep;
  
    }

  }

}

