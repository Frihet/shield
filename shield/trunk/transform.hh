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

    
    /**
       The various possible contexts for a printable to be in
    */
    enum context
      {
	/**
	   Unknown context.
	*/
	CONTEXT_UNDEFINED,
	/**
	   Any lob context, like lob, clob, blob
	*/
	CONTEXT_LOB,
	/**
	   Any numeric context
	*/
	CONTEXT_NUMBER, 
	/**
	   Any string context, like char, varchar, etc.
	*/
	CONTEXT_STRING,
	/**
	   Any type that can be sorted, compared, etc. I.e. _not_ lob.
	*/
	CONTEXT_SORTABLE,
	/**
	   A date type
	*/
	CONTEXT_DATE,
	/**
	   A datetime object
	*/
	CONTEXT_DATETIME,
      }
    ;

    class printable_alias;
    class limit;
    class chain;
    class printable;
    class type;
    class text;
    class environment;

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
      virtual printable *operator () (printable *p) = 0;
    };

    /**
       A class representing a part of a sql query that can be printed using the << operator.

       This base class contains some infrastructure for printing, setting
       properties, and automatic memory deallocation.
    */
    class printable
    {
    public:

      /**
	 Make empty virtual destructor since otherwise one cannot use
	 delete on a derived class to delete memory allocated in the
	 base. Oh C++, how I love thee.
      */
      virtual ~printable()
      {
      }

      /**
	 Return the transformed string representation of this printable.
      */
      string str ()
      {
	std::ostringstream out;
	if (!(out << *this))
	  throw shield::exception::syntax ("stringify called on invalid type");
	return trim (out.str());
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
	 Set the context of this printable to be the desired
	 state. This function does not work on all subclasses - many
	 of them, such as field names, know what context they belong
	 to and ignore this value.
      */
      void set_context (context c)
      {
	__context = c;
      }

      /**
	 Returns the context of this printable. 
      */
      virtual context get_context ()
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
      virtual printable *transform (catalyst &catalyst);

      printable *get_parent ()
      {
	return __parent;
      }
      
      void set_parent (printable *parent);

      virtual environment *get_environment ();

      virtual printable *internal_transform ()
      {
	return this;
      }

    protected:
      //      map<string,printable *> __prop;

      printable();
      
      /**
	 Print the query to the specified stream
      */
      virtual void print (ostream &stream);

      void set_child (int id, printable *value);
      printable *get_child (int id);


    private:

      friend ostream &operator << (ostream &stream, printable &e);

      bool __skip_space;
      bool __push_front;
      context __context;
      printable *__parent;
      map<int, printable *> __children;
    };

    class query
      : public printable
    {

    public:
      query ();

      text *get_table (text *field);
      virtual text *unalias_table (text *table);
      environment *get_environment ();

    protected:

      virtual chain *get_condensed_table_list ();
      
    private:
      environment *__environment;

    }
    ;

    ostream &operator << (ostream &stream, printable &e);

    class environment
      : public printable
    {

    public:
      
      environment (query *q)
	: __query (q)
      {
      }

      query *get_query ()
      {
	return __query;
      }
      
    private:
      query *__query;
      
    }
    ;


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
	if (type == LITERAL)
	  set_context (CONTEXT_STRING);

	set_skip_space (!insert_whitespace);
      }

      text (unsigned long long val, text_type type = EXACT, bool insert_whitespace=true)
	: __val (stringify (val)), __type (type)
      {
	set_context (CONTEXT_NUMBER);
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
      virtual void print (ostream &stream);

    private:
      string __val;
      text_type __type;
    }
    ;

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
	 Push element onto this chain. Most of the chain interface
	 mimics the stl vector interface, but this method is named
	 push, not push_back. This is because this method can push to
	 the front of the list if the item returns true on
	 get_push_front (), though that feature seems to be disused.
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

      virtual printable *transform (catalyst &catalyst);

    protected:

      virtual void print (ostream &stream);

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

    class limit 
      : public printable
    {
    public:

      limit (unsigned long long from, unsigned long long to)
	: __from (from), __to(to)
      {
      }

    protected:

      virtual void print (ostream &stream)
      {
	stream << " shield_rownum >" << __from << " and shield_rownum <= " << __to;
      }  

    private:
      unsigned long long __from;
      unsigned long long __to;

    }
    ;

    /**
       A printable representing a select query.
    */
    class select
      : public query
    {
    public:
      select();
  
      void set_item_list( chain *list )
      {
	set_child (__ITEM_LIST, list);
      }

      chain *get_item_list ()
      {
	return dynamic_cast<chain *> (get_child (__ITEM_LIST));
      }

      void set_table_list( chain *list )
      {
	set_child (__TABLE_LIST, list);
      }

      chain *get_table_list ()
      {
	return dynamic_cast<chain *> (get_child (__TABLE_LIST));
      }

      void set_where_clause( printable *clause )
      {
	set_child (__WHERE_CLAUSE, clause);
      }

      printable *get_where_clause ()
      {
	return get_child (__WHERE_CLAUSE);
      }

      void set_group_clause( chain *clause )
      {
	set_child (__GROUP_CLAUSE, clause);
      }

      chain *get_group_clause ()
      {
	return dynamic_cast<chain *> (get_child (__GROUP_CLAUSE));
      }

      void set_having_clause( printable *clause )
      {
	set_child (__HAVING_CLAUSE, clause);
      }

      printable *get_having_clause ()
      {
	return get_child (__HAVING_CLAUSE);
      }

      void set_order_clause( printable *clause )
      {
	set_child (__HAVING_CLAUSE, clause);
      }

      printable *get_order_clause ()
      {
	return get_child (__ORDER_CLAUSE);
      }

      void set_limit_clause( limit *clause )
      {
	set_child (__LIMIT_CLAUSE, clause);
      }

      limit *get_limit_clause ()
      {
	return dynamic_cast<limit *> (get_child (__LIMIT_CLAUSE));
      }

      void set_procedure_clause( printable *clause )
      {
	set_child (__PROCEDURE_CLAUSE, clause);
      }

      printable *get_procedure_clause ()
      {
	return get_child (__PROCEDURE_CLAUSE);
      }

      void set_into_clause( printable *clause )
      {
	set_child (__INTO_CLAUSE, clause);
      }

      printable *get_into ()
      {
	return get_child (__INTO_CLAUSE);
      }

      void set_option_clause( printable *clause )
      {
	set_child (__OPTION_CLAUSE, clause);
      }

      printable *get_option_clause ()
      {
	return get_child (__OPTION_CLAUSE);
      }

      string get_table (text *id);

      virtual text *unalias_table (text *alias);

      virtual chain *get_condensed_table_list ();

      virtual printable *internal_transform ();

    protected:
    
      virtual void print (ostream &stream);

    private:
      enum 
	{
	  __ITEM_LIST,
	  __TABLE_LIST,
	  __WHERE_CLAUSE,
	  __GROUP_CLAUSE,
	  __HAVING_CLAUSE,
	  __ORDER_CLAUSE,
	  __LIMIT_CLAUSE,
	  __PROCEDURE_CLAUSE,
	  __INTO_CLAUSE,
	  __OPTION_CLAUSE
	};
    };


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
      virtual void print (ostream &stream)
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
	if (__render)
	  __render->set_parent (this);

	if (__post_render)
	  __post_render->set_parent (this);

      }
  
      printable *get_post_render ()
      {
	return __post_render;
      }

      virtual printable *transform (catalyst &catalyst);

    protected:

      virtual void print (ostream &stream)
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
      : public query
    {
    public:

      void set_key_type (key_type type);
      void set_name (printable *name);
      void set_key_list (chain *key_list);

      /*
	Filter the __key_list to remove any columns which are known to be
	of non-indexable type (i.e. clob) and return the result.
      */
      chain *get_filtered_key_list (chain *field_list);
      
      virtual printable *transform (catalyst &catalyst);

    protected:
      
      virtual void print (ostream &stream);
      
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
  
      void set_field_name (printable *field_name)
      {
	__field_name = field_name;
      }

    protected:
  
      virtual void print (ostream &stream);
    private:
      
      printable *__field_name;
    }
    ;

    class identity 
      : public printable
    {
    public:

      identity (text *ns=0, text *table=0, text *field=0)
	: __namespace (ns), __table (table), __field (field)
      {
	if (ns)
	  ns->set_parent (this);
	if (table)
	  table->set_parent (this);
	if (field)
	  field->set_parent (this);
      }

      text *get_namespace ();
      text *get_table ();
      text *get_field ();

      virtual printable *transform (catalyst &catalyst);
      virtual context get_context ();

    protected:

      virtual void print (ostream &stream);

    private:

      text *__namespace;
      text *__table;
      text *__field;

    }
    ;

    class create_table 
      : public query
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
  
      printable *transform (catalyst &catalyst);
  
      printable *get_name ()
      {
	return __name;
      }

      chain *get_prev_index_list ()
      {
	__prev_index_list;
      }

      chain *get_field_list ()
      {
	return __field_list;
      }

    protected:
      virtual void print (ostream &stream);
  
    private:
      chain *__field_list;
      chain *__key_list;
      printable *__create_options;
      printable *__table_options;
      printable *__initial_data;
      printable *__name;
      printable *__like_clause;
      chain *__prev_index_list;
      bool __check;
      
      void post_render (ostream &stream, attribute *attr);
    }
    ;
    
    
    class insert
      : public query
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
      virtual void print (ostream &stream);
      virtual chain *get_condensed_table_list ();
      
    private:
      printable *__field_list;
      chain *__value_list;
      printable *__insert_update;
      printable *__name;
      bool __ignore;
    }
    ;

    class drop_table
      : public query
    {
    public:
      
      drop_table( printable *name, bool if_exists);
      
    protected:
      
      virtual void print (ostream &stream);
      
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
    
      virtual void print (ostream &stream);

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
    
      virtual void print (ostream &stream);
  
    private:

      interval_type __type;
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
    
      virtual void print (ostream &stream)
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

    /**
       A printable with an alias
    */
    class printable_alias
      : public printable
    {
    public:
      printable_alias (printable *item=0, text *alias=0)
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
    
      virtual void print (ostream &stream)
      {
	stream << *__item;

	print_alias (stream);
      }

      void print_alias (ostream &stream) const
      {
	if( __alias && __alias->length () )
	  stream << *__alias;
      }
  
  
    private:
      printable *__item;
      text *__alias;

    }
    ;


    class select_item_wild 
      : public printable_alias
    {
    public:
      select_item_wild (text *ns=0, text *table=0)
	: printable_alias (0, 0), __namespace (ns), __table (table)
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

      virtual void print (ostream &stream)
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

      virtual printable *operator () (printable *p)
      {
	text *t = dynamic_cast<text *> (p);
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

    /**
       Simple class to validate that the tree is sane. Never changes any nodes.
    */
    class validation_catalyst
      : public catalyst
    {
    public:

      virtual printable *operator () (printable *p)
      {
	/*
	  Check that we have a valid node
	*/
	assert (p);
	
	query *q = dynamic_cast<query *> (p);
	
	if (!q)
	{
	  /*
	    Only queries may be a root
	  */
	  assert (p->get_parent ());

	  /*
	    Check for short infinite loops. We should really check for
	    arbitrarily long loops as well, but in practice they don't
	    happen.
	  */
	  assert (p != p->get_parent ());
	}
	

	/**
	   Check that the environment is healthy
	*/
	assert (p->get_environment ());
	assert (p->get_environment ()->get_query ());
	
	return p;
      }
    }
    ;

    /**
       This catalyst transforms any identifiers that are represented
       by a text node into one represented by an identity node.
    */
    class identity_catalyst
      : public catalyst
    {
    public:
      virtual printable *operator () (printable *p);
    }
    ;

    class internal_catalyst 
      : public catalyst
    {
    public:
      virtual printable *operator () (printable *p)
      {
	return p->internal_transform ();
      }
    }
    ;

    /**
       This catalyst locates any table names in a from clause. If
       there is a table alias, it is returned in preference over the
       actual table name. 
    */
    class find_table_catalyst
      : public catalyst
    {
    public:
      find_table_catalyst ()
      {
	__res = new chain ();
      }

      /**
	 The catalyst function. Detects printable_alias nodes and
	 handles them.
      */
      virtual printable *operator () (printable *p)
      {
	printable_alias *a = dynamic_cast<printable_alias *> (p);
	if (a)
	  {
	    if (a->get_alias ())
	      __res->push (a->get_alias ());
	    else
	      __res->push (a->get_item ());	      
	  }
	return p;
      }

      /**
	 Return the accumulated list of tables
      */
      chain *get_table_list ()
      {
	return __res;
      }

    private:
      chain *__res;
    }
    ;

    /**
       A function representing a comparison operation. This nodes
       exists to handle implicit conversions. At print time, a cast
       node may be emitted for either argument.
    */
    class comparison
      : public printable
    {
    public:
      comparison (printable *op, printable *arg1, printable *arg2);

    protected:

      virtual void print (ostream &stream);

    private:
      enum {
	OP,
	ARG1,
	ARG2
      }
      ;

    }
    ;

    /**
       A node representing a type cast. 
    */
    class cast
      : public printable
    {
    public:
      cast (printable *p, context c);

    protected:
      virtual void print (ostream &stream);

    private:
      context __context;

    }
    ;

    /**
       This is not a real query - it is used to make a chain of real
       queries be a legal return value - this class wraps the chain up
       so that when the chain calls get_parent it will not die.
    */
    class fake_query
      : public query
    {
    public:

      /**
	 Constructor. 

	 No validation possible - acepts any argument.
      */
      fake_query (printable *p)
	: __transform_identity (false)
      {
	set_child (0, p);
      } 

      virtual printable *internal_transform ()
      {
	if (!__transform_identity)
	  return this;
	identity_catalyst i;
	return this->transform (i);
      }

      /**
	 This function can be called to set whether the
	 identity_catalyst transformation should be performed. The
	 default is false (no).
      */
      void set_transform_identity (bool v)
      {
	__transform_identity = v;
      }
      
    protected:

      virtual void print (ostream &stream)
      {
	printable *inner = get_child (0);

	if (!inner)
	  throw shield::exception::syntax ("Tried to print null fake_query node");

	stream << *inner;
      }

    private:

      /**
	 Whether identity transformation should be performed
      */
      bool __transform_identity;
      
    }
    ;

    class update
      : public query
    {
    public:

      void set_table_list (chain *name)
      {
	set_child (__NAME, name);
      }

      chain *get_table_list ()
      {
	return dynamic_cast<chain *> (get_child (__NAME));
      }

      void set_update_list (printable *name)
      {
	set_child (__UPDATE_LIST, name);
      }

      printable *get_update_list ()
      {
	return get_child (__UPDATE_LIST);
      }

      void set_where_clause (printable *name)
      {
	set_child (__WHERE_CLAUSE, name);
      }

      printable *get_where_clause ()
      {
	return get_child (__WHERE_CLAUSE);
      }

      void set_order_clause (printable *name)
      {
	set_child (__ORDER_CLAUSE, name);
      }

      printable *get_order_clause ()
      {
	return get_child (__ORDER_CLAUSE);
      }

      void set_delete_limit_clause (printable *name)
      {
	set_child (__DELETE_LIMIT_CLAUSE, name);
      }

      printable *get_delete_limit_clause ()
      {
	return get_child (__DELETE_LIMIT_CLAUSE);
      }

      virtual chain *get_condensed_table_list ()
      {
	find_table_catalyst c;
	transform (c);
	return c.get_table_list ();
      }

      printable *internal_transform ()
      {
	identity_catalyst i;
	return this->transform (i);
      }

    protected:

      virtual void print (ostream &stream);

    private:

      enum 
	{
	  __NAME,
	  __UPDATE_LIST,
	  __WHERE_CLAUSE,
	  __ORDER_CLAUSE,
	  __DELETE_LIMIT_CLAUSE
	}
      ;
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
