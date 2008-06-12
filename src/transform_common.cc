/**
   @file transform_common.cc

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz
   
   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/


#include <set>

#include "include/transform.hh"
#include "include/util.hh"
#include "include/cache.hh"
#include "include/exception.hh"
#include "include/catalyst.hh"
#include "include/introspection.hh"
#include "include/database.hh"

namespace shield
{

  namespace transform
  {

    using namespace util;

    namespace
    {
      bool parse_error;
    }

    const string sep = string () + '\0';

    printable *root;

    logger::logger warning ("shield: transform warning");
    logger::logger debug ("shield: transform debug");

    bool is_reserved (const string &in)
    {

      static set<string> reserved;

      /*
	Mind you, these are the _oracle_ reserved words, not MySQL reserved words.
      */
      static string reserved_arr[] = 
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
	  "drop", "mode", "rows", "with",
	}
      ;
      
      string lower;

      if (reserved.size () == 0)
	{
	  for (size_t i = 0; i<(sizeof (reserved_arr)/sizeof (reserved_arr[0])); i++ )
	    {
	      reserved.insert (reserved_arr[i]);
	    }
	}
      
      lower = to_lower (in);
      
      return reserved.find (lower) != reserved.end ();
    }

    void drop_item (ostream &stream, const string &item_type_external, const string &item_name)
    {

      string item_type = to_lower (item_type_external);
      string plural = item_type + "s";
      string upcase_name = to_upper (item_name);

      if (item_type == "index")
	{
	  plural = "indexes";
	}
  
      stream << "declare" << endl;
      stream << "\t"+item_type+"_count integer;" << endl;
      stream << "\tcursor_handle integer;" << endl;
      stream << "\tfeedback integer;" << endl;
      stream << "begin" << endl;
      stream << "\tselect count ("+item_type+"_name) into "+item_type+"_count from user_"+plural+" where "+item_type+"_name=" << oracle_escape (upcase_name).first << ";" << endl;
      stream << "\tif "+item_type+"_count = 1 then" << endl;
      stream << "\t\tcursor_handle := dbms_sql.open_cursor;" << endl;
      stream << "\t\tdbms_sql.parse (cursor_handle, "<< oracle_escape (string ("drop "+item_type+" ") + item_name).first << ", dbms_sql.native);" << endl;
      stream << "\t\tfeedback := dbms_sql.execute (cursor_handle);" << endl;
      stream << "\t\tdbms_sql.close_cursor (cursor_handle);" << endl;
      stream << "\tend if;" << endl;
      stream << "end;" << endl;
      stream << "/" << endl << endl << sep;
  
    }

    /* Called by yyparse on error.  */
    void yyerror (char const *s)
    {
      parse_error = true;
      cerr << "shield: " << s << "\n";
    }

    string translate (const string &in) throw ()
    {
      try
	{
	  string out ("");
	  string::const_iterator it;
	  int sep_count=0;

	  if (in.empty ())
	    {
	      return shield::transform::sep+shield::transform::sep;
	    }


	  using namespace shield;

	  ostringstream output_stream;
	    
	  pair<bool, string> cache_res = cache::get (in);

	  if (cache_res.first)
	    {
	      out =cache_res.second;
	    }
	  else
	    {
	      using namespace shield::transform;

	      root = 0;
	      lex_set_string (in);
	      parse_error = false;
	      if (yyparse ())
		{
		  return shield::transform::sep+shield::transform::sep;
		}
	      
	      if (root)
		{
		  catalyst::validation validate1;
		  catalyst::internal intern;
		  catalyst::validation validate2;
		  root = root->transform (validate1);
		  debug << root->get_tree ();
		  root = root->transform (intern);	      
		  debug << root->get_tree ();
		  root = root->transform (validate2);
		  debug << root->get_tree ();
		  
		  output_stream << *root;
		}
	      
	      printable_delete ();
	      
	      out =  output_stream.str ();
	      
	      cache::set (in, out);
	      
	    }
	    
	  for (it=out.begin (); it != out.end (); ++it)
	    {
	      if (shield::transform::sep[0] == *it)
		{
		  if (!sep_count)
		    {
		      sep_count++;
		    }
		}
	      else
		{
		  sep_count = 0;
		}
	    }

	  for (int i=sep_count; i<2; i++)
	    {
	      out += shield::transform::sep;
	    }
	  return out;
	}
      catch (const shield::exception::traceback &e)
	{
	  error << "In query:";
	  error << in;
	  error << e.what ();
	  introspection::clear_tables ();
	  database::reset();
	  return string ()+shield::transform::sep+shield::transform::sep;
	}
      catch(const bad_alloc& x)
	{
	  printable_delete ();
	  error << "In query:";
	  error << in;
	  error << (string ("Out of memory: ") + x.what()) << "on query:" <<  in;
	  introspection::clear_tables ();
	  database::reset();
	  return string ()+shield::transform::sep+shield::transform::sep;
	}
      catch (const std::exception &e)
	{
	  error << "In query:";
	  error << in;
	  error << string("Non-shield exception thrown: ")+e.what ();
	  introspection::clear_tables ();
	  database::reset();
	  return string ()+shield::transform::sep+shield::transform::sep;
	}
      catch (...)
	{
	  error << "In query:";
	  error << in;
	  error << "Unknown error was thrown.";
	  introspection::clear_tables ();
	  database::reset();
	  return string ()+shield::transform::sep+shield::transform::sep;
	}
      
    }

    text *as_text(printable *p, int part)
    {
      transform::text *txt = dynamic_cast<text *> (p);
      transform::identity *id = dynamic_cast<identity *> (p);
      if (txt)
	{
	  return txt;
	}
      if (id)
	{
	  switch (part)
	    {
	    case 0:
	      return id->get_namespace ();
	    case 1:
	      return id->get_table ();
	    case 2:
	      return id->get_field ();
	    default:
	      throw exception::invalid_state ("Tried to acces invalid identity element");
	    }
	}
      return 0;
    }

    
  }

}
