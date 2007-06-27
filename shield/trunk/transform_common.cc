#include <cctype>
#include <stdarg.h>
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

    string to_lower (const string &in)
    {
      string lower = in;
  
      for (size_t i = 0; i<lower.size(); i++)
	{
	  lower[i] = tolower (lower[i]);
	}

      return lower;
    }


    string to_upper (const string &in)
    {
      string upper = in;
  
      for (size_t i = 0; i<upper.size(); i++)
	{
	  upper[i] = toupper (upper[i]);
	}

      return upper;
    }


    string trim (const string &in)
    {
      string::const_iterator b, e;
      for (b = in.begin (); b<in.end (); b++ )
	{
	  if (!strchr( " \n\r", *b))
	    break;
	}

      for (e = in.end ()-1; e<=in.begin (); e-- )
	{
	  if (!strchr( " \n\r", *e))
	    break;
	}
      e++;

      string out (b, e);
      return out;
    }

    string oracle_escape (const string &in1)
    {
      string in = in1;
      string out = "'";
      int count = 0;
      bool is_clob = false;
      string::const_iterator i;

      if (in.length () == 0)
	in = " ";

      if (in.length () >= 4000)
	{
	  is_clob = true;
	}

      if (is_clob)
	{
	  out = "to_clob ('";
	}


      for (i=in.begin (); i < in.end (); i++)
	{
     
	  char c = *i;
      
	  if (c == '\'')
	    {
	      out += c;
	      out += c;
	    }
	  else if (abs(c) >= 32 )
	    {
	      count++;
	  
	      out += c;
	    }
	  else 
	    {
	      out += "'";

	      if (is_clob)
		out += ")";

	      out += " || chr (";
	      out += stringify ((int)c);
	      out += ") || ";

	      if (is_clob)
		out += "to_clob (";
	      out += "'";

	      count += 20;
	    }

	  if (count >= 60)
	    {
	      out += "' ||\n";
	      if (is_clob)
		out += "to_clob (";
	      out += "'";
	      count = 0;
	    }
	}

      out += "'";

      if (is_clob)
	out += ")";

      return out;
    }


    bool contains_str (const char *needle, ...)
    {
      char *arg;
      va_list va;
      bool res = false;
  
      va_start (va, needle);
      while ((arg=va_arg (va, char *) )!= 0)
	{
	  if (strcmp (needle, arg) == 0)
	    {
	      res = true;
	      break;
	    }

	}
      va_end (va);

      return res;
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
