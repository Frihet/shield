/**
   @file transform_auto_increment.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/

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
      
      string t_name = __table_query->get_name ()->unmangled_str ();
      string f_name = __field_name->unmangled_str ();

      string name = t_name + "_" + f_name;
      string sequence_name;
      string trigger_name;

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

      sequence_name = name + "_s_";
      trigger_name = name + "_t_";
      

      drop_item (stream, "sequence", sequence_name);

      stream << "create sequence " << sequence_name << endl;
      stream << "start with 1" << endl;
      stream << "increment by 1" << endl;
      stream << "nomaxvalue" << endl << endl;
  
      stream << sep;
  
      stream << "create or replace trigger " << trigger_name << endl;
      stream << "before insert on " << __table_query->get_name ()->str () << endl;
      stream << "for each row" << endl;
      stream << "begin" << endl;
      stream << "\tselect " << sequence_name << ".nextval into shield.last_insert_id from dual;" << endl;
      stream << "\tif :new." <<  __field_name->str () << " is null or :new." << __field_name->str () << " = 0 then" << endl;
      stream << "\t\tselect " << sequence_name << ".currval into :new." << __field_name->str () << " from dual;" << endl;
      stream << "\telse" << endl; 
      stream << "\t\twhile shield.last_insert_id < :new." << __field_name->str () << endl;
      stream << "\t\tloop" << endl;
      stream << "\t\t\tselect " << sequence_name << ".nextval into shield.last_insert_id from dual;" << endl;
      stream << "\t\tend loop;" <<endl;
      stream << "\tend if;" <<endl;
      stream << "end;" <<endl;
      stream << "/" << endl << endl << sep;
  
    }

  }

}

