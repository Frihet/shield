/**
   @file transform.hh

   The transform namespace contains all code for parsing MySQL sql and
   transforming it into Oracle sql.

   @package shield
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 or later.


*/

/**
   @mainpage Shield API documentation

   @section Introduction

   Shield, or SQL Helper Interfacce Enabling Legacy Databases, is a
   wrapper program, that is intended to make it possible to use the
   Oracle database program with program that where written for the
   MySQL database.

   @section tech Technical description
   
   At it's heart, Shield is a compiler. Shield translates from one
   language, MySQL SQL, into another language which may seem similar
   but only on the surface, namely Oracle SQL. This translation is
   divided into three distinct phases, parsing, transformation and
   output.

   @subsection parsing Parsing

   This first pass has the goal of creating a tree structure representing the query to 
   translate. This tree is called the Abstract Syntax Tree or AST. Parsing is performed by two 
   modules. 

   - The first is the lexer, also called the tokenizer. This module divides the input into logical tokens and classifies them. The lexer is written in the flex language, and is defined in transform_lex.y. Flex files consist of a list of regular expressions, and an action associated with each expression. The longest matching regular expression will isrepeatedly chosen and its action is run.

   - The second is the actual parser. This module actually constructs the AST from the tokens. The lexer is written as a bison grammar and is defined in transform_yacc.y. A bison grammar consists of a series of rules. Each rule tells how the compiler how to transform a set of zero or more nodes into a single node. When parsing a file, these rules are repeatedly applied on the input until only a singe node remains.

   For more information about flex and bison, see the respective
   manuals.

   @subsection transformation Transformation

   This consists of various transformations of the AST. It is
   implemented using the transform method and functors deriving from
   the catalyst base class. The catalyst base class defines a functor
   that takes a AST node as argument and returns a node to replace it
   with. 

   @subsection output Output

   The AST is written into a output stream. Each node has a _print
   method that prints that specific node and all its children. The
   _print method is overloaded by most nodes to perform node type
   specific formating. 

   Some nodes, like the cast node, which is used to convert an
   expression to a specific type, do all their actual work at print
   time.

   @subsection memory-management Memory managment

   The non-parser part of shield is written using a simple modern C++
   memory managment style where memory managment is handled implicityl
   and very few objects are dynamically allocated. Unfortunatly, bison
   requires all AST nodes to be inserted into a union, and C++ does
   not permit putting classes in union, so this is not possible in the
   parser. Instead, Shield has an extremly simply allocation strategy:
   The entire AST is allocated using a non-oveloaded new operator, and
   each node is inserted into a vector. Once the tree has been
   written, every element of the vector is deleted. This has a few
   drawbacks:

   - Multiple inheritance is not allowed

   - Multithreading in the parser is not allowed without some clever coding. In order to have multithreading, one would need to use Thread Local Storage for the allocation vector.

   The benefit of this memory model is that memory leaks in the parser
   are extremely rare. The above limitations are not a serious
   limitations, and both can be worked around with some work.
   
   @subsection style Coding style

   Shield relies heavily on dynamic casting to perform its task. Most
   catalysts only work on one specific type of nodes, and test each
   node using dynamic casting.

*/


#ifndef TRANSFORM_HH
#define TRANSFORM_HH

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stdarg.h>
#include <assert.h>

//#include "util.hh"
#include "logger.hh"

namespace shield
{

  using namespace shield;

  namespace catalyst
  {
    class catalyst;
  }

  namespace transform
  {

    using namespace std;

    extern const char sep;

    /**
       Logger object for transform warnings
    */
    extern logger::logger warning;
    /**
       Logger object for transform debug output
    */
    extern logger::logger debug;
    
    /**
       The various types thet the text node can be of
    */
    enum text_type
      {
	/**
	   The text node represents an exect sequence of characters and
	   should not be transformed
	*/
	EXACT, 
	
	/**
	   The text node is an identifier, and reserved names should be avoided.
	*/
	IDENTIFIER,
	
	/**
	   The text node is a quoted identifier, the quotes should be stripped.
	*/
	IDENTIFIER_QUOTED,
	
	/**
	   The text node is a string literal, and should be converted to
	   an oracle style string literal
	*/
	LITERAL
      }
    ;

    /**
       Set of different types of keys (indices)
    */
    enum key_type
      {
	/** Unique key */
	UNIQUE, 
	/** Primary key */
	PRIMARY_KEY, 
	/** Synonym for unique key */
	KEY
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
    class query;
    class type;
    class text;

    /**
       Check whether the specified string is a reserved word

       @param in The string to check
    */
    bool is_reserved( const string &in );

    /**
       Delete all existing printables and subclasses, e.g. all existing nodes.
    */
    void printable_delete(void);

    /*
      This function emits the PL/SQL code needed to check if an item
      exists, and if so, drop it. An item can be a table, a sequence or an
      index.

      @param stream The stream to output the code to
      @param item_type_external The type of the item in Oracle sql. This can be for example index or table. 
      @param item_name the name of the item to delete
    */
    void drop_item (ostream &stream, const string &item_type_external, const string &item_name);

    /**
       A class representing a node in the tree created by parsing a sql query. 

       This base class contains some infrastructure for printing, setting
       properties, handling children and automatic memory deallocation.
    */
    class printable
    {
    public:

      /**
	 Make empty virtual destructor since otherwise one cannot use
	 delete on a derived class to delete memory allocated in the
	 base. Oh C++, how I love thee.
      */
      virtual ~printable(void)
      {
      }

      /**
	 Return the transformed string representation of this
	 printable. This is done by using the << operator and then
	 calling trim on the resulting string result.
      */
      string str (void);

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
	 Returns the \c context of this printable. 
      */
      virtual context get_context (void)
      {
	return __context;
      }

      /**
	 Sets whether the whitespace before this printable can be
         safely skipped.
      */
      virtual void set_skip_space (bool ss)
      {
	__skip_space = ss;
      }

      /**
	 Return true if the whitespace before this printable should be
	 skipped.	 
      */
      bool get_skip_space (void) const
      {
	return __skip_space;
      }

      /**
	 Transform this node and all it's children in arbitrary order
	 using the specified catalyst.
      */
      virtual printable *transform (shield::catalyst::catalyst &catalyst);
      
      /**
	 Return this nodes parent, or null if this is the root.
      */
      printable *get_parent (void)
      {
	return __parent;
      }
      
      /**
	 Reparent this node.
      */
      void set_parent (printable *parent);

      /**
	 Return the query that this node is a part of
      */
      virtual query *get_query (void);

      /**
	 This method is called for every node by the \c
	 internal_catalyst as the first transformation of the syntax
	 tree. This baseline implementation does nothing, but various
	 subclasses of the printable node overload this method to
	 perform node specific transformations.
      */
      virtual printable *internal_transform (void)
      {
	return this;
      }

      /**
	 Sets whether this node should be pushed to the front of its
	 parent if its parent is a chain.
      */
      void set_push_front (bool push_front)
      {
	__push_front = push_front;
      }

      /**
	 Returns true of this node should be pushed to the front of
	 its parent if its parent is a chain.
      */
      bool get_push_front (void)
      {
	return __push_front;
      }

    protected:

      /**
	 The constructor. It is protected to make sure that only
	 subclasses are instantiated.
      */
      printable(void);
      
      /**
	 Print the query to the specified stream. This is only a
	 virtual base method, all real action is performed by
	 subclasses. If this method is not overloaded, a \c syntax
	 exception will be thrown.
      */
      virtual void _print (ostream &stream);

      /**
	 Add a new child node to this node, and identify it using the
	 specified integer.

	 @param id the integer to identify the child by
	 @param value the child. If value is null, then the specified child is erased.
      */
      void _set_child (int id, printable *value);

      /**
	 Return the child with the specified id, or null if no child
	 with that id exists.
      */
      printable *_get_child (int id);


    private:

      friend ostream &operator << (ostream &stream, printable &e);

      /**
	 If set to true, this node should be pushed to the front not
	 of the back of its parent if the parent is a chain.
      */
      bool __push_front;

      /**
	 Flag used for determening wheter the space before this
	 printable can be safely skipped. Since this only exists to
	 make the formating nice, there are nodes that will ignore
	 this information.
      */
      bool __skip_space;

      /**
	 The type context of this node. This is only valid for nodes that
	 represent an expression.
      */
      context __context;

      /**
	 The parent of this node
      */
      printable *__parent;

      /**
	 A map of all children
      */
      map<int, printable *> __children;
    };

    /**
       This is the base class of all nodes that represent a complete
       query. Note that since a single MySQL query can result in
       multiple Oracle queries, this does not need to be the root of
       the tree. 
    */
    class query
      : public printable
    {

    public:
      query (void);

      /**
	 This is a lookup method that returns the table that the
	 specified field belongs to.
      */
      text *get_table (text *field);

      /**
	 This method takes a possibly aliased table name and returns
	 the unaliased name.
      */
      virtual text *unalias_table (text *table);

      /**
	 Returns this node
      */
      virtual query *get_query (void);

    protected:

      /**
	 Returns a chain object whare each child is a table. The
	 tables may be aliased.
      */
      virtual chain *_get_condensed_table_list (void);
      
    private:

    }
    ;

    /**
       The output operator for a printable node. This operator calls
       the virtual print method of the node in order to print the
       whole tree.
    */
    ostream &operator << (ostream &stream, printable &e);

    /*
      A printable that represents an arbitrary piece of text, a
      string literal, or an identifier.

      When type is LITERAL, this class will automatically take care of
      unescaping any mysql escape sequences and escape using oracle
      syntax.

      When type is EXACT, not transformation will be performed.

      When type is IDENTIFIER, reserved words will have an underscore appended to them.

      When type is IDENTIFIER_QUOTED, the quoting is stripped and reserved words will have an underscore appended to them.
    */
    class text 
      : public printable
    {
    public:

      /**
	 Construct a new text object using a string as the initial definition.

	 @param text the original text message this object should represent
	 @param type is the type of message
	 @param insert_whitespace whether whitespace is needed before this token
      */
      text (const string &text, text_type type = EXACT, bool insert_whitespace=true)
	: __val (text), __type (type)
      {
	if (type == LITERAL)
	  set_context (CONTEXT_STRING);

	set_skip_space (!insert_whitespace);
      }

      /**
	 Construct a new text object using a number as the initial definition.

	 @param val the original value this object should represent
	 @param type is the type of message
	 @param insert_whitespace whether whitespace is needed before this token
      */
      text (unsigned long long val, text_type type = EXACT, bool insert_whitespace=true);

      /**
	 Returns the raw (unformated) length of this item. Use str
	 ().size () to determine the formated length.
      */
      size_t length (void) const
      {
	return __val.length ();
      }

      /**
	 Returns the type of text
      */
      text_type get_type (void) const
      {
	return __type;
      }

    protected:
      
      /**
	 Print this node.
      */
      virtual void _print (ostream &stream);

    private:

      /**
	 The value this object should represent
      */
      string __val;
      
      /**
	 the type of message
      */
      text_type __type;
    }
    ;

    /**
       This class represents a sequence of child nodes with no known
       internal relation. 
    */
    class chain 
      : public printable
    {
    public:
      /**
	 Typedef for the const_iterator type, defined to fir in with stl.
      */
      typedef vector<printable *>::const_iterator const_iterator;
      /**
	 Typedef for the iterator type, defined to fir in with stl.
      */
      typedef vector<printable *>::iterator iterator;

      /**
	 Construct a new chain with the specifgied contents.
      */
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
	 push, not push_back for historic reasons. This should
	 probably be fixed.
      */
      void push (printable *p);

      /**
	 Returns true of this chain is empty
      */
      bool empty (void) const
      {
	return __chain.empty ();
      }

      /**
	 Returns an iterator for this chain
      */
      const_iterator begin (void) const
      {
	return __chain.begin ();
      }

      const_iterator end (void) const 
      {
	return __chain.end ();
      }

      /**
	 Returns an iterator for this chain
      */
      iterator begin (void) 
      {
	return __chain.begin ();
      }

      iterator end (void)  
      {
	return __chain.end ();
      }

      /**
	 Returns the child with the specified index
      */
      printable *operator [] (int idx)
      {
	return __chain[idx];
      }

      /**
	 Returns the number of elements in this chain
      */
      size_t size (void) const
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

      virtual printable *transform (catalyst::catalyst &catalyst);

      /**
	 insert the elements specified at the specified poistion.

	 @param pos where to perform the insertion
	 @param begin the first element to insert
	 @param end the first element not to insert
      */
      template<class iter> void
      insert (iterator pos, iter begin, iter end)
      {
	__chain.insert (pos, begin, end);
      }

    protected:

      virtual void _print (ostream &stream);

    private:
      /**
	 The vecor containing all the items
      */
      vector<printable *> __chain;
      /**
	 The separator string to print between all strings
      */
      string __separator;
      /**
	 If non-zero, insert a newline after printing the specified
	 number of children
      */
      int __do_line_break;
    }
    ;

    /**
       This class represents a limit clause. It is implemented using
       the implicit rownum column.
    */
    class limit 
      : public printable
    {
    public:

      limit (unsigned long long from, unsigned long long to)
	: __from (from), __to(to)
      {
      }

    protected:

      virtual void _print (ostream &stream)
      {
	stream << " shield_rownum >" << __from << " and shield_rownum <= " << __to;
      }  

    private:
      unsigned long long __from;
      unsigned long long __to;

    }
    ;

    /**
       A printable representing a select query. Capable of lots and lots of transformations.
    */
    class select
      : public query
    {
    public:
      select(void);
  
      void set_item_list( chain *list )
      {
	_set_child (__ITEM_LIST, list);
      }

      chain *get_item_list (void)
      {
	return dynamic_cast<chain *> (_get_child (__ITEM_LIST));
      }

      void set_table_list( chain *list )
      {
	assert (list);
	_set_child (__TABLE_LIST, list);
      }

      chain *get_table_list (void)
      {
	return dynamic_cast<chain *> (_get_child (__TABLE_LIST));
      }

      void set_where_clause( printable *clause )
      {
	_set_child (__WHERE_CLAUSE, clause);
      }

      printable *get_where_clause (void)
      {
	return _get_child (__WHERE_CLAUSE);
      }

      void set_group_clause( chain *clause )
      {
	_set_child (__GROUP_CLAUSE, clause);
      }

      chain *get_group_clause (void)
      {
	return dynamic_cast<chain *> (_get_child (__GROUP_CLAUSE));
      }

      void set_having_clause( printable *clause )
      {
	_set_child (__HAVING_CLAUSE, clause);
      }

      printable *get_having_clause (void)
      {
	return _get_child (__HAVING_CLAUSE);
      }

      void set_order_clause( printable *clause )
      {
	_set_child (__HAVING_CLAUSE, clause);
      }

      printable *get_order_clause (void)
      {
	return _get_child (__ORDER_CLAUSE);
      }

      void set_limit_clause( limit *clause )
      {
	_set_child (__LIMIT_CLAUSE, clause);
      }

      limit *get_limit_clause (void)
      {
	return dynamic_cast<limit *> (_get_child (__LIMIT_CLAUSE));
      }

      void set_procedure_clause( printable *clause )
      {
	_set_child (__PROCEDURE_CLAUSE, clause);
      }

      printable *get_procedure_clause (void)
      {
	return _get_child (__PROCEDURE_CLAUSE);
      }

      void set_into_clause( printable *clause )
      {
	_set_child (__INTO_CLAUSE, clause);
      }

      printable *get_into (void)
      {
	return _get_child (__INTO_CLAUSE);
      }

      void set_option_clause( printable *clause )
      {
	_set_child (__OPTION_CLAUSE, clause);
      }

      printable *get_option_clause (void)
      {
	return _get_child (__OPTION_CLAUSE);
      }

      /**
	 Return the true name of a possibly aliased table. If the name
	 is not an alias, the original name is returned.
      */
      virtual text *unalias_table (text *alias);

      /**
	 Use the identity_catalyst.
      */
      virtual printable *internal_transform (void);

    protected:
    
      virtual void _print (ostream &stream);

      /**
	 Create a list of all tables that this query reads from. Table
	 names may be aliased. This list includes tables in
	 subqueries.
      */
      virtual chain *_get_condensed_table_list (void);

    private:
      /**
	 Enum for all child node identifiers
      */
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


    /**
       A node representing a list enclosed in parenthesis. Works just
       like a regular chain, except for the parenthesis.
    */
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
      virtual void _print (ostream &stream)
      {
	stream << " (";
	chain::_print (stream);
	stream << ")";
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
      {
	_set_child (__RENDER, render);
	_set_child (__POST_RENDER, post_render);
      }

      printable *get_post_render (void)
      {
	return _get_child (__POST_RENDER);
      }

      printable *get_render (void)
      {
	return _get_child (__RENDER);
      }

    protected:
      virtual void _print (ostream &stream);

    private:
      enum
	{
	  __RENDER,
	  __POST_RENDER
	}
      ;
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

    protected:
      
      virtual void _print (ostream &stream);
      
    private:
      
      enum
	{
	  __NAME,
	  __KEY_LIST
	}
      ;

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
      virtual void _print (ostream &stream);

    private:
      printable *__field_name;

    }
    ;

    class identity 
      : public printable
    {
    public:

      identity (text *ns=0, text *table=0, text *field=0)
      {
	_set_child (__NAMESPACE, ns);
	_set_child (__TABLE, table);
	_set_child (__FIELD, field);
      }

      text *get_namespace ();
      text *get_table ();
      text *get_field ();

      virtual context get_context ();

    protected:

      virtual void _print (ostream &stream);

    private:
      
      enum
	{
	  __NAMESPACE,
	  __TABLE,
	  __FIELD
	}
      ;
    }
    ;

    class create_table 
      : public query
    {
    public:

      create_table(void);
    
      void set_field_list (chain *l);
      void set_key_list (chain *l);
      void set_create_options (printable *l);
      void set_table_options (printable *l);
      void set_initial_data (printable *l);
      void set_name (printable *l);
      void set_like_clause (printable *l);
      void set_check_existance (bool check);
  
      printable *transform (catalyst::catalyst &catalyst);
      
      printable *get_name (void)
      {
	return __name;
      }
      
      chain *get_prev_index_list (void)
      {
	__prev_index_list;
      }
      
      chain *get_field_list (void)
      {
	return __field_list;
      }
      
    protected:
      virtual void _print (ostream &stream);
  
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

      insert(void);
      
      void set_ignore (bool i);
      void set_field_list (printable *l);
      void set_value_list (chain *l);
      void set_insert_update (printable *l);
      void set_name (printable *l);
      void set_eq_list (chain *l);
      printable *get_field_list ();
      chain *get_value_list ();
      printable *get_insert_update ();
      printable *get_name ();
      
    protected:
      virtual void _print (ostream &stream);
      virtual chain *_get_condensed_table_list (void);
      
    private:
      enum
	{
	  __FIELD_LIST,
	  __VALUE_LIST,
	  __INSERT_UPDATE,
	  __NAME
	}
      ;
      bool __ignore;
    }
    ;

    class drop_table
      : public query
    {
    public:
      
      drop_table( printable *name, bool if_exists);
      
    protected:
      
      virtual void _print (ostream &stream);
      
    private:
      
      bool __if_exists;
      enum 
	{
	  __NAME
	}
      ;
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
      virtual void _print (ostream &stream);

    private:
      printable *__call;
    }
    ;

    class interval 
      : public printable
    {
    public:
      interval (interval_type type, printable *expr=0)
	: __type (type)
      {
	_set_child (__EXPR, expr);
      }

      void set_expr (printable *expr)
      {
	_set_child (__EXPR, expr);
      }

    protected:
      virtual void _print (ostream &stream);
  
    private:

      interval_type __type;
    
      enum
	{
	  __EXPR
	}
      ;
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

      bool get_indexable (void)
      {
	return __indexable;
      }

      void set_indexable (bool i)
      {
	__indexable = i;
      }

      string get_name (void)
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

      type *get_type (void)
      {
	return __type;
      }

      printable *get_attribute (void)
      {
	return __attrib;
      }

      printable *get_name (void)
      {
	return __name;
      }

    protected:
      virtual void _print (ostream &stream)
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
      {
	_set_child (__ITEM, item);
	_set_child (__ALIAS, alias);
      }

      void set_item (text *item)
      {
	_set_child (__ITEM, item);
      }

      printable *get_item (void)
      { 
	return _get_child (__ITEM);
      }

      void set_alias (text *alias)
      {
	_set_child (__ALIAS, alias);
      }

      text *get_alias (void)
      {
	return dynamic_cast<text *> (_get_child (__ALIAS));
      }

      bool has_alias (void)
      {
	return get_alias () != 0;
      }

    protected:
      virtual void _print (ostream &stream)
      {
	stream << *get_item ();

	print_alias (stream);
      }

      void print_alias (ostream &stream)
      {
	text *alias = get_alias ();
	if( alias && alias->length () )
	  stream << *alias;
      }

    private:
      enum
	{
	  __ITEM,
	  __ALIAS
	}
      ;

    }
    ;


    class select_item_wild 
      : public printable_alias
    {
    public:
      select_item_wild (text *ns=0, text *table=0)
	: printable_alias (0, 0)
      {
	_set_child (__NAMESPACE, ns);
	_set_child (__TABLE, table);
      }

      text *get_namespace (void)
      {
	return dynamic_cast<text *> (_get_child (__NAMESPACE));
      }

      text *get_table (void)
      {
	return dynamic_cast<text *> (_get_child (__TABLE));
      }

    protected:
      virtual void _print (ostream &stream)
      {
	stream << " ";

	if (get_namespace () && get_namespace ()->length ())
	  {
	    stream << *get_namespace () << ".";
	  }
	if (get_table () && get_table ()->length ())
	  {
	    stream << *get_table () << ".";
	  }
	stream << "*";

	print_alias (stream);
      }

    private:

      enum
	{
	  __NAMESPACE,
	  __TABLE
	}
      ;

    };


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
      virtual void _print (ostream &stream);

    private:
      /**
	 Enum for all child node identifiers
      */
      enum {
	__OP,
	__ARG1,
	__ARG2
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
      virtual void _print (ostream &stream);

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
      {
	_set_child (__INNER, p);
      } 

    protected:
      virtual void _print (ostream &stream);

    private:
      enum
	{
	  __INNER
	}
      ;

    }
    ;


    /**
       Node representing an update query. Pretty simple query.
    */
    class update
      : public query
    {
    public:

      /**
	 Setter for the table list
      */
      void set_table_list (chain *name)
      {
	_set_child (__NAME, name);
      }

      /**
	 Getter for the table list
      */
      chain *get_table_list (void)
      {
	return dynamic_cast<chain *> (_get_child (__NAME));
      }

      /**
	 Setter for the update list
      */
      void set_update_list (printable *name)
      {
	_set_child (__UPDATE_LIST, name);
      }

      /**
	 Getter for the update list
      */
      printable *get_update_list (void)
      {
	return _get_child (__UPDATE_LIST);
      }

      /**
	 Setter for the where clause
      */
      void set_where_clause (printable *name)
      {
	_set_child (__WHERE_CLAUSE, name);
      }

      /**
	 Getter for the where clause
      */
      printable *get_where_clause (void)
      {
	return _get_child (__WHERE_CLAUSE);
      }

      /**
	 Setter for the order clause
      */
      void set_order_clause (printable *name)
      {
	_set_child (__ORDER_CLAUSE, name);
      }

      /**
	 Getter for the order clause
      */
      printable *get_order_clause (void)
      {
	return _get_child (__ORDER_CLAUSE);
      }

      /**
	 Setter for the delete limit clause
      */
      void set_delete_limit_clause (printable *name)
      {
	_set_child (__DELETE_LIMIT_CLAUSE, name);
      }

      /**
	 Getter for the delete limit clause
      */
      printable *get_delete_limit_clause (void)
      {
	return _get_child (__DELETE_LIMIT_CLAUSE);
      }

      /**
	 Perform identity catalyst transform
      */
      virtual printable *internal_transform (void);

    protected:
      virtual void _print (ostream &stream);
      virtual chain *_get_condensed_table_list (void);


    private:

      /**
	 Enum for all child node identifiers
      */
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

    /**
       Error callback for yyparse and yylex. Prints the specified error message.
    */
    void yyerror (char const *s);

    /**
       Parse the whole syntax tree, transform it and write it out to
       standard output.

       If something happens, any subclass of shield::exception::exception may be thrown.
    */
    int yyparse (void);
    
    /**
       Print the shield package definition. This is the contents of the file shield.package.sql.
    */
    void print_package (void);

    /**
       Set which string the lexer should tokenize
    */    
    void *lex_set_string (const string &str);

  }

}

/**
   The lexer (tokenizer). This function can't be a part of any
   namespace without some pretty ugly hacks, since it is automatically
   generated.
*/
int yylex (void);

#endif //#ifndef TRANSFORM_HH

