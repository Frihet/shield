#include <assert.h>

#include "transform.hh"

namespace shield
{

  namespace transform
  {


    bool is_reserved (const string &in)
    {

      /**
	 Mind you, these are the _oracle_ reserved words, not MySQL reserved words.
      */
      string reserved[] = 
	{
	  "access", "else", "modify", "start",
	  "add", "exclusive", "noaudit", "select",
	  "all", "exists", "nocompress", "session",
	  "alter", "file", "not", "set",
	  "and", "float", "notfound", "share",
	  "any", "for", "nowait", "size",
	  "arraylen", "from", "null", "smallint",
	  "as", "grant", "number", "sqlbuf",
	  "asc", "group", "of", "successful",
	  "audit", "having", "offline", "synonym",
	  "between", "identified", "on", "sysdate",
	  "by", "immediate", "online", "table",
	  "char", "in", "option", "then",
	  "check", "increment", "or", "to",
	  "cluster", "index", "order", "trigger",
	  "column", "initial", "pctfree", "uid",
	  "comment", "insert", "prior", "union",
	  "compress", "integer", "privileges", "unique",
	  "connect", "intersect", "public", "update",
	  "create", "into", "raw", "user",
	  "current", "is", "rename", "validate",
	  "date", "level", "resource", "values",
	  "decimal", "like", "revoke", "varchar",
	  "default", "lock", "row", "varchar2",
	  "delete", "long", "rowid", "view",
	  "desc", "maxextents", "rowlabel", "whenever",
	  "distinct", "minus", "rownum", "where",
	  "drop", "mode", "rows", "with", "shield",
	}
      ;

      string lower = to_lower (in);

      for (int i = 0; i<(sizeof (reserved)/sizeof (string)); i++ )
	{
	  if (lower == reserved[i])
	    {
	      return true;
	    }
	}
      return false;
    }

    void drop_item (ostream &stream, const string &item_type_external, const string &item_name)
    {

      string item_type = to_lower (item_type_external);
      string plural = item_type + "s";

      if (item_type == "index")
	{
	  plural = "indexes";
	}
  
      string upcase_name = to_upper (item_name);
      stream << "declare" << endl;
      stream << "\t"+item_type+"_count integer;" << endl;
      stream << "\tcursor_handle integer;" << endl;
      stream << "\tfeedback integer;" << endl;
      stream << "begin" << endl;
      stream << "\tselect count ("+item_type+"_name) into "+item_type+"_count from user_"+plural+" where "+item_type+"_name=" << oracle_escape (upcase_name) << ";" << endl;
      stream << "\tif "+item_type+"_count = 1 then" << endl;
      stream << "\t\tcursor_handle := dbms_sql.open_cursor;" << endl;
      stream << "\t\tdbms_sql.parse (cursor_handle, "<< oracle_escape (string ("drop "+item_type+" ") + item_name) << ", dbms_sql.native);" << endl;
      stream << "\t\tfeedback := dbms_sql.execute (cursor_handle);" << endl;
      stream << "\t\tdbms_sql.close_cursor (cursor_handle);" << endl;
      stream << "\tend if;" << endl;
      stream << "end;" << endl;
      stream << "/" << endl << endl;
  
    }

    /* Called by yyparse on error.  */
    void yyerror (char const *s)
    {
      cerr << "shield: " << s << "\n";
    }


  }

}
