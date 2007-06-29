/** \file transform.hh

The transform namespace contains all code for parsing MySQL sql and
transforming it into Oracle sql.

*/
#ifndef TRANSFORM_HH
#define TRANSFORM_HH

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stdarg.h>
#include <assert.h>

#include "exception.hh"
#include "util.hh"

using namespace std;
using namespace util;

namespace shield
{

  namespace transform
  {

    extern const char sep;
    
    /**
       The various types of text that exist
    */
    enum text_type
      {
	/**
	   The text represents an exect sequence of characters and
	   should not be transformed
	*/
	EXACT, 
	/**
	   the text is an identifier, and reserved names should be avoided.
	*/
	IDENTIFIER,
	/**
	   the text is a quoted identifier, the quotes should be stripped.
	*/
	IDENTIFIER_QUOTED,
	/**
	   The text is a string literal, and should be converted to
	   an oracle style string literal
	*/
	LITERAL
      }
    ;

    /**
       The various possible contexts for a printable to be in
    */
    enum context
      {
	UNDEFINED, 
	NUMERIC, 
	STRING,
	DEFINED
      }
    ;

    /**
       Set of different types of keys
    */
    enum key_type
      {
	UNIQUE, PRIMARY_KEY, KEY
      }
    ;

    /**
       Different interval lengths
    */
    enum interval_type
      {
	INTERVAL_DAY,
	INTERVAL_HOUR,
	INTERVAL_SECOND,
	INTERVAL_YEAR,
	INTERVAL_MONTH,
	INTERVAL_MINUTE
      }
    ;

    class select_item;
    class limit;
    class chain;
    class printable;
    class type;
    class text;

    /**
       Check whether the specified string is a reserved word
    */
    bool is_reserved( const string &in );

    /**
       Delete all existing printable objects. This function is defined in printable.cc.
    */
    void printable_delete();

    /*
      This function emits the PL/SQL code needed to check if an item
      exists, and if so, drop it. An item can be a table, a sequence or an
      index.
    */
    void drop_item (ostream &stream, const string &item_type_external, const string &item_name);

    /**
       This is a class used to transform printables. It takes a
       printable and returns another one. It is used together with the
       transform function of printables.
    */
    class catalyst
    {
    public:
      virtual const printable *operator () (const printable *p) = 0;
    };

    /**
       A class representing a part of a sql query that can be printed using the << operator.

       This base class contains some infrastructure for printing, setting
       properties, and automatic memory deallocation.
    */
    class printable
    {
    public:
  
      friend ostream &operator << (ostream &stream, const printable &e);

      /**
	 Make empty virtual destructor since we use delete on the base
	 class to free derived classes.
      */
      virtual ~printable()
      {
      }

      /**
	 Return the transformed string representation of this printable.
      */
      string str () const
      {
	std::ostringstream out;
	if (!(out << *this))
	  throw shield::exception::syntax ("stringify called on invalid type");
	return trim (out.str());
      }

      /**
	 Set the list of properties. These are used to e.g. tell a
	 printable what table it belongs to.
      */
      void set_property (map<string,printable *> &prop)
      {
	__prop = prop;
      }

      /**
	 Returns true if this type of printable should be pushed to
	 the front rather than the back of a chain. This is needed
	 sometimes because MySQL allows mixing indices and fields in
	 e.g. create table queries, but oracle needs them ordered. 
      */
      bool get_push_front () const
      {
	return __push_front;
      }

      /**
	 Sets the value of push_front.
      */
      void set_push_front (bool pf)
      {
	__push_front = pf;
      }

      /**
	 Sets the context of this printable. 
      */
      void set_context (context c)
      {
	__context = c;
      }

      /**
	 Returns the context of this printable. The context needs to
	 be known because in mysql it is legal to compare values from
	 different contexts, like a date and a string, and type
	 conversion will be implicitly performed. This does not happen
	 in oracle.
      */
      context get_context () const
      {
	return __context;
      }

      /**
	 Sets whether the whitespace before this printable should be
         skipped.
      */
      virtual void set_skip_space (bool ss)
      {
	__skip_space = ss;
      }

      /**
	 Return true if the whitespace before this printable should be
	 skipped.	 
      */
      bool get_skip_space () const
      {
	return __skip_space;
      }

      /**
	 Transform this printable and all it's subelements using the specified catalyst.
      */
      virtual const printable *transform (catalyst &catalyst) const;

    protected:
      map<string,printable *> __prop;

      printable();
      
      /**
	 Print the query to the specified stream
      */
      virtual void print (ostream &stream) const;

    private:

      bool __skip_space;
      bool __push_front;
      context __context;
    };

    ostream &operator << (ostream &stream, const printable &e);

    /*
      A printable that represents an arbitrary piece of text, or a
      string literal, or an identifier.
    */
    class text 
      : public printable
    {
    public:

      text (const string &text, text_type type = EXACT, bool insert_whitespace=true)
	: __val (text), __type (type)
      {
	set_skip_space (!insert_whitespace);
      }

      text (unsigned long long val, text_type type = EXACT, bool insert_whitespace=true)
	: __val (stringify (val)), __type (type)
      {
	set_context (NUMERIC);
	set_skip_space (!insert_whitespace);
      }

      /**
	 Returns the unescaped length of this item
      */
      size_t length () const
      {
	return __val.length ();
      }

      /**
	 Returns the type of text
      */
      text_type get_type () const
      {
	return __type;
      }

    protected:
      virtual void print (ostream &stream) const;

    private:
      string __val;
      text_type __type;
    }
    ;

    /**
       A printable representing a select query.
    */
    class select
      : public printable
    {
    public:
      select();
  
      void set_item_list( chain *list )
      {
	__item_list = list;
      }

      void set_table_list( chain *list )
      {
	__table_list = list;
      }

      void set_where_clause( printable *clause )
      {
	__where_clause = clause;
      }

      void set_group_clause( chain *clause )
      {
	__group_clause = clause;
      }

      void set_having_clause( printable *clause )
      {
	__having_clause = clause;
      }

      void set_order_clause( printable *clause )
      {
	__order_clause = clause;
      }

      void set_limit_clause( limit *clause )
      {
	__limit_clause = clause;
      }

      void set_procedure_clause( printable *clause )
      {
	__procedure_clause = clause;
      }

      void set_into_clause( printable *clause )
      {
	__into_clause = clause;
      }

      void set_option_clause( printable *clause )
      {
	__option_clause = clause;
      }

    protected:
    
      virtual void print (ostream &stream) const;

    private:
      chain *__item_list;
      chain *__table_list;
      printable *__where_clause;
      chain *__group_clause;
      printable *__having_clause;
      printable *__order_clause;
      limit *__limit_clause;
      printable *__procedure_clause;
      printable *__into_clause;
      printable *__option_clause;
    };


    class chain 
      : public printable
    {
    public:
      typedef vector<printable *>::const_iterator const_iterator;
      typedef vector<printable *>::iterator iterator;

      chain (printable *a=0, 
	     printable *b=0, 
	     printable *c=0, 
	     printable *d=0, 
	     printable *e=0, 
	     printable *f=0, 
	     printable *g=0, 
	     printable *h=0,
	     printable *i=0,
	     printable *j=0,
	     printable *k=0,
	     printable *l=0);

      /**
	 Set what character should be printed between each printable
	 element. The default is nothing.
      */
      void set_separator (const string &sep)
      {
	__separator = sep;
      }

      /**
	 Push element onto this chain
      */
      void push (printable *p);

      /**
	 Returns true of this chain is empty
      */
      bool empty () const
      {
	return __chain.empty ();
      }

      /**
	 Returns an iterator for this chain
      */
      const_iterator begin () const
      {
	return __chain.begin ();
      }

      const_iterator end () const 
      {
	return __chain.end ();
      }

      printable *operator [] (int idx)
      {
	return __chain[idx];
      }

      /**
	 Returns the number of elements in this chain
      */
      size_t size () const
      {
	return __chain.size ();
      }

      /**
	 Set the number of items that should be printed before line breaking
      */
      void set_line_break (int do_line_break)
      {
	__do_line_break = do_line_break;
      }

      virtual void set_skip_space (bool ss)
      {
	printable::set_skip_space (ss);
	if (!__chain.empty ())
	  __chain[0]->set_skip_space (ss);
      }

      virtual const printable *transform (catalyst &catalyst) const;

    protected:

      virtual void print (ostream &stream) const;

      virtual chain *construct () const
      {
	return new chain ();
      }

    private:
      vector<printable *> __chain;
      string __separator;
      int __do_line_break;
    }
    ;

    class paran 
      : public chain
    {
    public:
      paran (printable *a=0, 
	     printable *b=0, 
	     printable *c=0, 
	     printable *d=0, 
	     printable *e=0, 
	     printable *f=0, 
	     printable *g=0, 
	     printable *h=0)
	: chain (a, b, c, d, e, f, g, h)
      {
	set_skip_space (true);
      }

    protected:
      virtual void print (ostream &stream) const
      {
	stream << " (";
	chain::print (stream);
	stream << ")";
      }
  
      virtual chain *construct () const
      {
	return new paran ();
      }  

    }
    ;
    
    
    class limit : public printable
    {
    public:

      limit (unsigned long long from, unsigned long long to)
	: __from (from), __to(to)
      {
      }

    protected:

      virtual void print (ostream &stream) const
      {
	stream << " shield_rownum >" << __from << " and shield_rownum <= " << __to;
      }  

    private:
      unsigned long long __from;
      unsigned long long __to;

    }
    ;

    /**
       A class for defining a table attribute. The point of this class
       is that a lot of things can be defined inline in a mysql create
       table query that need to be defined as a separate query in
       Oracle. This class has a get_post_render () method that allows
       you to return a printable that is used _after_ the first query.
    */
    class attribute
      : public printable
    {
    public:

      attribute (printable *render=0, printable *post_render=0)
	: __render (render), __post_render (post_render)
      {
      }
  
      printable *get_post_render ()
      {
	return __post_render;
      }

    protected:

      virtual void print (ostream &stream) const
      {
	if (__render)
	  {
	    stream << *__render;
	  }
      }

    private:

      printable *__render;
      printable *__post_render;
    }
    ;

    /**
       A class for creating an index
    */
    class create_index
      : public printable
    {
    public:

      void set_key_type (key_type type)
      {
	__type = type;
      }

      void set_name (printable *name)
      {
	__name = name;
      }

      void set_key_list (chain *key_list)
      {
	__key_list = key_list;
      }
      
      /*
	Filter the __key_list to remove any columns which are known to be
	of non-indexable type (i.e. clob) and return the result.
      */
      chain *get_filtered_key_list () const;
      
    protected:
      
      virtual void print (ostream &stream) const;
      
    private:
      
      printable *__name;
      chain *__key_list;
      key_type __type;
    }
    ;

    class auto_increment
      : public printable
    {
    public:
  
    protected:
  
      virtual void print (ostream &stream) const;
  
    }
    ;

    class identity 
      : public printable
    {
    public:

      identity (text *ns=0, text *table=0, text *field=0)
	: __namespace (ns), __table (table), __field (field)
      {
      }

      text *get_namespace ();
      text *get_table ();
      text *get_field ();

    protected:

      virtual void print (ostream &stream) const;

    private:

      text *__namespace;
      text *__table;
      text *__field;

    }
    ;

    class create_table 
      : public printable
    {
    public:

      create_table();
    
      void set_field_list (chain *l);
      void set_key_list (chain *l);
      void set_create_options (printable *l);
      void set_table_options (printable *l);
      void set_initial_data (printable *l);
      void set_name (printable *l);
      void set_like_clause (printable *l);
      void set_check_existance (bool check);
  
  
    protected:
      virtual void print (ostream &stream) const;
  
    private:
      chain *__field_list;
      chain *__key_list;
      printable *__create_options;
      printable *__table_options;
      printable *__initial_data;
      printable *__name;
      printable *__like_clause;
      bool __check;
      
      void post_render (ostream &stream, attribute *attr, map<string, printable *> &prop) const;
    }
    ;
    
    
    class insert
      : public printable
    {
    public:

      insert();
      
      
      void set_ignore (bool i);
      void set_field_list (printable *l);
      void set_value_list (chain *l);
      void set_insert_update (printable *l);
      void set_name (printable *l);
      void set_eq_list (chain *l);
      
    protected:
      virtual void print (ostream &stream) const;
      
    private:
      printable *__field_list;
      chain *__value_list;
      printable *__insert_update;
      printable *__name;
      bool __ignore;
    }
    ;

    class drop_table
      : public printable
    {
    public:
      
      drop_table( printable *name, bool if_exists);
      
    protected:
      
      virtual void print (ostream &stream) const;
      
    private:
      
      printable *__name;
      bool __if_exists;

    }
    ;

    class date_function
      : public printable
    {

    public:

      date_function (printable *call)
	: __call (call)
      {
      }

    protected:
    
      virtual void print (ostream &stream) const;

    private:
      printable *__call;
    }
    ;

    class interval 
      : public printable
    {
    public:
      interval (interval_type type, printable *expr=0)
	: __type (type), __expr(expr)
      {
      }

      void set_expr (printable *expr)
      {
	__expr = expr;
      }

    protected:
    
      virtual void print (ostream &stream) const;
  
    private:

      interval_type __type;
      printable *__expr;
    }
    ;

    class natural
      : public printable
    {
    public:

      natural (printable *expr)
	: __expr(expr)
      {
	set_context (DEFINED);
      }

    protected:
    
      virtual void print (ostream &stream) const;
  
    private:

      printable *__expr;
    }
    ;


    class type
      : public chain
    {
    public:
      type (printable *a=0, 
	    printable *b=0, 
	    printable *c=0, 
	    printable *d=0, 
	    printable *e=0, 
	    printable *f=0, 
	    printable *g=0, 
	    printable *h=0)
	: chain (a, b, c, d, e, f, g, h), __indexable (true)
      {
      }

      bool get_indexable ()
      {
	return __indexable;
      }

      void set_indexable (bool i)
      {
	__indexable = i;
      }

      string get_name ()
      {
	return (*this)[0]->str ();
      }

    private:

      bool __indexable;

    }
    ;

    class field_spec
      : public printable
    {
    public:
      field_spec (printable *name, type *type, printable *attribute)
	: __name (name), __type (type), __attrib (attribute)
      {
	__name->set_skip_space (true);
      }

      type *get_type ()
      {
	return __type;
      }

      printable *get_attribute ()
      {
	return __attrib;
      }

      printable *get_name ()
      {
	return __name;
      }

    protected:
    
      virtual void print (ostream &stream) const
      {
	stream << "\t" << *__name << *__type;
	if (__attrib)
	  stream << *__attrib;
      }

    private:

      printable *__name;
      type *__type;
      printable *__attrib;
  
    }
    ;

    class select_item
      : public printable
    {
    public:
      select_item (printable *item=0, text *alias=0)
	: __item (item), __alias (alias)
      {
      }

      printable *get_item ()
      {
	return __item;
      }

      void set_alias (text *alias)
      {
	__alias = alias;
      }

      text *get_alias ()
      {
	return __alias;
      }

      bool has_alias ()
      {
	return get_alias () != 0;
      }

    protected:
    
      virtual void print (ostream &stream) const
      {
	stream << *__item;

	print_alias (stream);
      }

      void print_alias (ostream &stream) const
      {
	if( __alias && __alias->length () )
	  stream << " as" << *__alias;
      }
  
  
    private:
      printable *__item;
      text *__alias;

    }
    ;


    class select_item_wild 
      : public select_item
    {
    public:
      select_item_wild (text *ns=0, text *table=0)
	: select_item (0, 0), __namespace (ns), __table (table)
      {
      }

      text *get_namespace ()
      {
	return __namespace;
      }

      text *get_table ()
      {
	return __table;
      }

  

    protected:

      virtual void print (ostream &stream) const
      {
	stream << " ";

	if (__namespace && __namespace->length ())
	  {
	    stream << *__namespace << ".";
	  }
	if (__table && __table->length ())
	  {
	    stream << *__table << ".";
	  }
	stream << "*";

	print_alias (stream);
      }

    private:

      text *__namespace;
      text *__table;

    };


    /**
       A catalyst fo replacing a specific shield_text with another
       printable. This is used in the having clause of selects,
       because oracle forbids the use of aliases there for some odd
       reason.
    */
    class replace_identifier_catalyst
      : public catalyst
    {
    public:

      replace_identifier_catalyst (map<string, printable *> &mapping)
	: __mapping (mapping)
      {
      }

      virtual const printable *operator () (const printable *p)
      {
	const text *t = dynamic_cast<const text *> (p);
	if (!t)
	  {
	    return p;
	  }

	if (t->get_type () != IDENTIFIER &&
	    t->get_type () != IDENTIFIER_QUOTED)
	  {
	    return p;
	  }

	string st = t->str ();

	if (__mapping.find (st) == __mapping.end ())
	  {
	    return p;
	  }

	return __mapping[st];

      }

    private:
      map<string, printable *> __mapping;
    }
    ;

    void yyerror (char const *s);
    int yyparse ();

    /**
       Print the shield package definition
    */
    void print_package ();

    void *lex_set_string (const string &str);

  }

}
int yylex ();

#endif
