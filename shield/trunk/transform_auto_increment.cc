#include "transform.hh"

namespace shield
{

namespace transform
{

/**
   Print 
*/
void auto_increment::
print (ostream &stream)
{

  create_table *table_query = dynamic_cast<create_table *> (get_query ());
  assert (table_query);

  string t_name = table_query->get_name ()->str ();
  string f_name = __field_name->str ();

  string name = t_name + "_" + f_name;

  /*
    Truncate index name at 27 characters since oracle has a 30
    character maximum on field names. This can lead to clashes!
  */
  if (name.length () > 27)
    {
      name = t_name.substr(0, 15) + "_" + f_name;
      name = name.substr(0, 27);
    }

  drop_item (stream, "sequence", name + "_s_");

  stream << sep;

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
  stream << "/" << endl << endl;
  
}

}

}

