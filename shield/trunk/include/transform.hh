/**
   @file transform.hh

   The transform namespace contains all code for parsing MySQL sql and
   transforming it into Oracle sql.

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.


*/


#ifndef TRANSFORM_HH
#define TRANSFORM_HH

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stdarg.h>

#include "shield.hh"
#include "logger.hh"

namespace shield
{

  using namespace shield;

  namespace catalyst
  {
    class catalyst;
  }

  namespace introspection
  {
    class column_type;
  }

  /**
     @namespace shield::transform

     All AST node types as well as parsing functions.
  */
  namespace transform
  {

    using namespace std;

    extern const char sep;
    extern int yypos;
    extern char *yytext;

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
       Different interval lengths of the interval type. 
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
       List of all node child types. This list has to be shared
       between all node types to avoid clashes when a node type
       inherits nodes from its parent.
    */
    enum child_type
      {
	CHILD_ITEM_LIST,
	CHILD_TABLE_LIST,
	CHILD_WHERE_CLAUSE,
	CHILD_GROUP_CLAUSE,
	CHILD_HAVING_CLAUSE,
	CHILD_ORDER_CLAUSE,
	CHILD_LIMIT_CLAUSE,
	CHILD_PROCEDURE_CLAUSE,
	CHILD_INTO_CLAUSE,
	CHILD_OPTION_CLAUSE,
	CHILD_RENDER,
	CHILD_POST_RENDER,
	CHILD_NAME,
	CHILD_KEY_LIST,
	CHILD_NAMESPACE,
	CHILD_TABLE,
	CHILD_FIELD,
	CHILD_FIELD_LIST,
	CHILD_VALUE_LIST,
	CHILD_INSERT_UPDATE,
	CHILD_EXPR,
	CHILD_ITEM,
	CHILD_ALIAS,
	CHILD_OP,
	CHILD_ARG1,
	CHILD_ARG2,
	CHILD_INNER,
	CHILD_UPDATE_LIST,
	CHILD_DELETE_LIMIT_CLAUSE,
	CHILD_TYPE,
	CHILD_ATTRIBUTE,
	CHILD_CRATE_OPTIONS,
	CHILD_TABLE_OPTIONS,
	CHILD_CREATE_OPTIONS,
	CHILD_INITIAL_DATA,
	CHILD_LIKE_CLAUSE,
	CHILD_PARAM
      }
    ;
    
    class printable_alias;
    class limit;
    class chain;
    class printable;
    class query;
    class type;
    class text;
    class create_table;

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
	 Set the data_type of this printable to be the desired
	 state. This function does not work on all subclasses - many
	 of them, such as field names, know what context they belong
	 to and ignore this value.
      */
      void set_context (data_type c)
      {
	__context = c;
      }

      /**
	 Returns the \c data_type of this printable. 
      */
      virtual data_type get_context (void)
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

	 @param parent the new parent. May not be null, or an invalid_state
	 exception is thrown.
      */
      void set_parent (printable *parent);

      /**
	 Return the query that this node is a part of.
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

      /**
	 Return a string showing the path from the root to this node

	 @param is_first internal parameter caller should not set this.
      */
      string get_path ( bool is_first=true);

      /**
	 Returns a hopefully human readble string representing this
	 node type. Overloaded versions of this method may give additional information.
      */
      virtual string get_node_name (void);

      /**
	 Return a string representing the tree created by this node and all its children

	 @param level internal parameter caller should not set this.
      */
      virtual string get_tree (int level=0);

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
      void _set_child (child_type id, printable *value);

      /**
	 Return the child with the specified id, or null if no child
	 with that id exists.
      */

      printable *_get_child (child_type id);

      template <class T>
      T *_get_child (child_type id)
      {
	printable *res = _get_child (id);
	T *res_cast = dynamic_cast<T *> (res);
	if (res && !res_cast)
	  {
	    invalid_type ("child node", typeid(T).name ());
	  }
	return res_cast;
      }

      /*
	Add a new query to the root of the AST. 
      */
      void _add_query (query *q);

    private:

      /**
	 This function handles the case of a child node having an
	 invalid type by throwing an invalid_type exception. It
	 exists because _get_child<> is a template function and thus
	 needs to be defined in this header, but no header-defined
	 function may throw an exception.  The reason for this is that
	 it is desirable to not include the exception.hh file fromt
	 his main header to avoid excessive dependencies.
      */
      void invalid_type (const string &what, const string &type);

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
	 The type data_type of this node. This is only valid for nodes that
	 represent an expression.
      */
      data_type __context;

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
      virtual void _make_condensed_table_list (void);
      
      vector<printable *> _condensed_table_list;

    private:

    }
    ;

    /**
       The output operator for a printable node. This operator calls
       the virtual print method of the node in order to print the
       whole tree.
    */
    ostream &operator << (ostream &stream, printable &e);

    /**
      A printable that represents an arbitrary piece of text, a
      string literal, or an identifier.

      When type is LITERAL, this class will automatically take care of
      unescaping any mysql escape sequences and escape using oracle
      syntax.

      - When type is EXACT, not transformation will be performed.

      - When type is IDENTIFIER, reserved words will have an underscore appended to them.

      - When type is IDENTIFIER_QUOTED, the quoting is stripped and reserved words will have an underscore appended to them.
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
	  set_context (DATA_TYPE_CHAR);

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

      virtual string get_node_name ();

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
       internal relation except for their internal order. The class
       mostly implements the vector interface.
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
	 Construct a new chain with the specified contents.
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
      
      /**
	 Returns a string reresenting this node and its shildren as a tree
      */
      virtual string get_tree (int level=0);

      /**
	 Return the context of the first subitem
      */
      virtual data_type get_context (void);

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
	_set_child (CHILD_ITEM_LIST, list);
      }

      chain *get_item_list (void)
      {
	return _get_child<chain> (CHILD_ITEM_LIST);
      }

      void set_table_list( chain *list )
      {
	_set_child (CHILD_TABLE_LIST, list);
      }

      chain *get_table_list (void)
      {
	return _get_child<chain> (CHILD_TABLE_LIST);
      }

      void set_where_clause( printable *clause )
      {
	_set_child (CHILD_WHERE_CLAUSE, clause);
      }

      printable *get_where_clause (void)
      {
	return _get_child (CHILD_WHERE_CLAUSE);
      }

      void set_group_clause( chain *clause )
      {
	_set_child (CHILD_GROUP_CLAUSE, clause);
      }

      chain *get_group_clause (void)
      {
	return _get_child<chain> (CHILD_GROUP_CLAUSE);
      }

      void set_having_clause( printable *clause )
      {
	_set_child (CHILD_HAVING_CLAUSE, clause);
      }

      printable *get_having_clause (void)
      {
	return _get_child (CHILD_HAVING_CLAUSE);
      }

      void set_order_clause( printable *clause )
      {
	_set_child (CHILD_HAVING_CLAUSE, clause);
      }

      printable *get_order_clause (void)
      {
	return _get_child (CHILD_ORDER_CLAUSE);
      }

      void set_limit_clause( limit *clause )
      {
	_set_child (CHILD_LIMIT_CLAUSE, clause);
      }

      limit *get_limit_clause (void)
      {
	return _get_child<limit> (CHILD_LIMIT_CLAUSE);
      }

      void set_procedure_clause( printable *clause )
      {
	_set_child (CHILD_PROCEDURE_CLAUSE, clause);
      }

      printable *get_procedure_clause (void)
      {
	return _get_child (CHILD_PROCEDURE_CLAUSE);
      }

      void set_into_clause( printable *clause )
      {
	_set_child (CHILD_INTO_CLAUSE, clause);
      }

      printable *get_into (void)
      {
	return _get_child (CHILD_INTO_CLAUSE);
      }

      void set_option_clause( printable *clause )
      {
	_set_child (CHILD_OPTION_CLAUSE, clause);
      }

      printable *get_option_clause (void)
      {
	return _get_child (CHILD_OPTION_CLAUSE);
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
      virtual void _make_condensed_table_list (void);

    private:
      
      void __resolve_item_list ();
      void __pre_calculate ();

    private:

      /**
	 Set of fields on which grouping is performed
      */
      set<string> __group_field;

      /**
	Mappings from field aliases to the underlying field
	definition. The field definition may be an expression.
      */
      map<string, printable *> __field_alias;



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
       A class for creating an index
    */
    class create_index
      : public query
    {
    public:

      create_index (void);
      void set_key_type (key_type type);
      void set_name (printable *name);
      void set_key_list (chain *key_list);

      /*
	Filter the __key_list to remove any columns which are known to be
	of non-indexable type (i.e. clob) and return the result.
      */
      chain *get_filtered_key_list (chain *field_list);

      /**
	 Walk through tree to locate field and query information, then
	 reparent this node as a separate query.
      */
      virtual printable *internal_transform (void);

    protected:
      
      virtual void _print (ostream &stream);
      
    private:
      /**
	 What type of index to create.
      */
      key_type __type;

      /**
	 The query that created this node.
	 
	 This is not a child node, it should not be converted to using
	 _set_child. It should not be transformed.
      */
      create_table *__table_query;

    }
    ;

    /**
       Creates sequences and triggers required to simulate an auto_increment:ed field under Oracle.

    */
    class auto_increment
      : public query
    {

    public:
      /**
	 Walk through tree to locate field and query information, then
	 reparent this node as a separate query.
      */
      virtual printable *internal_transform (void);

    protected:
      /**
	 Create a new sequence and a trigger to simulate auto_increment
      */
      virtual void _print (ostream &stream);

    private:

      /**
	 The query that created this node.
	 
	 This is not a child node, it should not be converted to using
	 _set_child. It should not be transformed.
      */
      create_table *__table_query;

      /**
	 The field to auto_increment.

	 This is not a child node, it should not be converted to using
	 _set_child. It should not be transformed.
      */
      printable *__field_name;
    }
    ;

    /**
       Node representing a field or table identity 
    */
    class identity 
      : public printable
    {
    public:

      identity (text *ns=0, text *table=0, text *field=0)
      {
	_set_child (CHILD_NAMESPACE, ns);
	_set_child (CHILD_TABLE, table);
	_set_child (CHILD_FIELD, field);
      }

      text *get_namespace (void);
      text *get_table (void);
      text *get_field (void);

      virtual data_type get_context (void);

    protected:

      virtual void _print (ostream &stream);

    private:
      
    }
    ;

    /**
       Print a table definition.
    */
    class create_table 
      : public query
    {
    public:

      create_table(void);
    
      void set_field_list (chain *l)
      {
	_set_child (CHILD_FIELD_LIST, l);
      }

      chain *get_field_list (void)
      {
	return _get_child<chain> (CHILD_FIELD_LIST);
      }

      void set_key_list (chain *l)
      {
	_set_child (CHILD_KEY_LIST, l);
      }

      chain *get_key_list (void)
      {
	return _get_child<chain> (CHILD_KEY_LIST);
      }

      void set_create_options (printable *l)
      {
	_set_child (CHILD_CREATE_OPTIONS, l);
      }

      printable *get_create_options (void)
      {
	return _get_child (CHILD_CREATE_OPTIONS);
      }

      void set_table_options (printable *l)
      {
	_set_child (CHILD_TABLE_OPTIONS, l);
      }

      printable *get_table_options (void)
      {
	return _get_child (CHILD_TABLE_OPTIONS);
      }

      void set_initial_data (printable *l)
      {
	_set_child (CHILD_INITIAL_DATA, l);
      }

      printable *get_initial_date (void)
      {
	return _get_child (CHILD_INITIAL_DATA);
      }

      void set_name (printable *l)
      {
	_set_child (CHILD_NAME, l);
      }

      printable *get_name (void)
      {
	return _get_child (CHILD_NAME);
      }

      void set_like_clause (printable *l)
      {
	_set_child (CHILD_LIKE_CLAUSE, l);
      }

      printable *get_like_clause (void)
      {
	return _get_child (CHILD_LIKE_CLAUSE);
      }

      void set_check_existance (bool check);
  
      vector<printable *> &get_prev_index_list (void)
      {
	return __prev_index_list;
      }
      
      virtual printable *internal_transform (void);

      void create_lookup_entries(void);

    protected:
      virtual void _print (ostream &stream);
  
    private:

      /**
	 If true, only define this table if it does not already exist
      */
      bool __check;

      /**
	 List of all created indices. 
      */
      vector<printable *>__prev_index_list;

    }
    ;
    
    /**
       Node representing an insert query
    */
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
      printable *get_field_list (void);
      chain *get_value_list (void);
      printable *get_insert_update (void);
      printable *get_name (void);
      
    protected:
      virtual void _print (ostream &stream);
      virtual void _make_condensed_table_list (void);
      
    private:
      bool __ignore;
    }
    ;

    /**
       Query for sropping a table
    */
    class drop_table
      : public query
    {
    public:
      
      drop_table( printable *name, bool if_exists);
      
    protected:
      
      virtual void _print (ostream &stream);
      
    private:
      
      bool __if_exists;
    }
    ;

    /**
       Node representing a sql function call. Contains information
       about function return type.
    */
    class function
      : public printable
    {

    public:

      function (printable *name, data_type type, printable *param);

      printable *get_name (void)
      {
	_get_child (CHILD_NAME);
      }

      printable *get_param (void)
      {
	_get_child (CHILD_PARAM);
      }

      /**
	 Function calls have a complex get_context implementation. Or
	 maybe it's more nasty than complex. Maybe it is both. Who can
	 tell?

	 Anyhow, when defined, a function is given a return type. This
	 works well for most functions, and isn't a problem. But some
	 functions have output types that depend on the input
	 type. Nasty, that. What shield does is to try and walk down
	 the tree to locate the first non-chain context, and return
	 that. That should be the type of the first parameter, which
	 should work for the functions we use.
      */
      virtual data_type get_context (void);

    protected:
      virtual void _print (ostream &stream);

    private:
      data_type __type;
    }
    ;

    /**
       Node representing a date interval
    */
    class interval 
      : public printable
    {
    public:
      interval (interval_type type, printable *expr=0)
	: __type (type)
      {
	_set_child (CHILD_EXPR, expr);
      }
      
      void set_expr (printable *expr)
      {
	_set_child (CHILD_EXPR, expr);
      }
      
    protected:
      virtual void _print (ostream &stream);
  
    private:

      interval_type __type;
    
    }
    ;

    /**
       Type definition node
    */
    class type
      : public chain
    {

    public:
      type (data_type type,
	    printable *a=0, 
	    printable *b=0, 
	    printable *c=0, 
	    printable *d=0, 
	    printable *e=0, 
	    printable *f=0, 
	    printable *g=0, 
	    printable *h=0)
	: __type (type), chain (a, b, c, d, e, f, g, h)
      {
      }
      
      bool get_indexable (void)
      {
	return __type != DATA_TYPE_CLOB;
      }
      
      string get_name (void)
      {
	return (*this)[0]->str ();
      }

      data_type get_type (void)
      {
	return __type;
      }
      
    protected:

      virtual void _print (ostream &stream);

    private:
      
      data_type __type;
    }
    ;

    /**
       Node representing a field specification
    */
    class field_spec
      : public printable
    {
    public:
      field_spec (printable *name, type *type, printable *attribute)
	: __name (name), __type (type), __attrib (attribute)
      {
	_set_child (CHILD_NAME, name);
	_set_child (CHILD_TYPE, type);
	_set_child (CHILD_ATTRIBUTE, attribute);
	__name->set_skip_space (true);
      }

      type *get_type (void)
      {
	return _get_child<type> (CHILD_TYPE);
      }

      printable *get_attribute (void)
      {
	return _get_child (CHILD_ATTRIBUTE);
      }

      printable *get_name (void)
      {
	return _get_child (CHILD_NAME);
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
      printable_alias (printable *item=0, text *alias=0, bool alias_implicit=false)
	: __alias_implicit (alias_implicit)
      {
	_set_child (CHILD_ITEM, item);
	_set_child (CHILD_ALIAS, alias);
      }

      void set_item (text *item)
      {
	_set_child (CHILD_ITEM, item);
      }

      printable *get_item (void)
      { 
	return _get_child (CHILD_ITEM);
      }

      void set_alias (text *alias, bool alias_implicit = false)
      {
	__alias_implicit = alias_implicit;
	_set_child (CHILD_ALIAS, alias);
      }

      text *get_alias (void)
      {
	return _get_child<text> (CHILD_ALIAS);
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
	if (!__alias_implicit)
	  {
	    text *alias = get_alias ();
	    if( alias && alias->length () )
	      stream << *alias;
	  }
      }

    private:

      bool __alias_implicit;

    }
    ;

    /**
       A wildcarded field selection item for select queries.
    */
    class select_item_wild 
      : public printable_alias
    {
    public:
      select_item_wild (text *ns=0, text *table=0)
	: printable_alias (0, 0)
      {
	_set_child (CHILD_NAMESPACE, ns);
	_set_child (CHILD_TABLE, table);
      }

      text *get_namespace (void)
      {
	return _get_child<text> (CHILD_NAMESPACE);
      }

      text *get_table (void)
      {
	return _get_child<text> (CHILD_TABLE);
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

    }
    ;

    /**
       A node representing a type cast. Type casting can specify a
       list of acceptable types, and unless the expression is already
       of one of these types it will be cast to the one of them which
       means the least loss of information.
    */
    class cast
      : public printable
    {
    public:
      /**
	 Create a new type cast node.
	 
	 @param p the expression to cast
	 @param contexts an or:ed set of data_type values
      */
      cast (printable *p, int contexts);

      virtual data_type get_context (void);

    protected:
      virtual void _print (ostream &stream);

    private:
      int __contexts;

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
	_set_child (CHILD_INNER, p);
      } 

    protected:
      virtual void _print (ostream &stream);

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
	_set_child (CHILD_NAME, name);
      }

      /**
	 Getter for the table list
      */
      chain *get_table_list (void)
      {
	return _get_child<chain> (CHILD_NAME);
      }

      /**
	 Setter for the update list
      */
      void set_update_list (printable *name)
      {
	_set_child (CHILD_UPDATE_LIST, name);
      }

      /**
	 Getter for the update list
      */
      printable *get_update_list (void)
      {
	return _get_child (CHILD_UPDATE_LIST);
      }

      /**
	 Setter for the where clause
      */
      void set_where_clause (printable *name)
      {
	_set_child (CHILD_WHERE_CLAUSE, name);
      }

      /**
	 Getter for the where clause
      */
      printable *get_where_clause (void)
      {
	return _get_child (CHILD_WHERE_CLAUSE);
      }

      /**
	 Setter for the order clause
      */
      void set_order_clause (printable *name)
      {
	_set_child (CHILD_ORDER_CLAUSE, name);
      }

      /**
	 Getter for the order clause
      */
      printable *get_order_clause (void)
      {
	return _get_child (CHILD_ORDER_CLAUSE);
      }

      /**
	 Setter for the delete limit clause
      */
      void set_delete_limit_clause (printable *name)
      {
	_set_child (CHILD_DELETE_LIMIT_CLAUSE, name);
      }

      /**
	 Getter for the delete limit clause
      */
      printable *get_delete_limit_clause (void)
      {
	return _get_child (CHILD_DELETE_LIMIT_CLAUSE);
      }

      /**
	 Perform identity catalyst transform
      */
      virtual printable *internal_transform (void);

    protected:
      virtual void _print (ostream &stream);
      virtual void _make_condensed_table_list (void);

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

    /**
       Get the string the lexer is tokenizing
    */
    string lex_get_string ();

    /**
       Return the next token type
    */
    int lex_do (void);

  }

}

/**
   The lexer (tokenizer). This function can't be a part of any
   namespace without some pretty ugly hacks, since it is automatically
   generated.
*/
int yylex (void);

#endif //#ifndef TRANSFORM_HH

