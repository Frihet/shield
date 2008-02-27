/**
   @file shield.hh
   
   Common header data.

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 3.


*/
/**
   @mainpage Shield API documentation

   @section Introduction

   Shield, or SQL Helper Interfacce Enabling Legacy Databases, is a
   wrapper program, that is intended to make it possible to use the
   Oracle database program with program that where written for the
   MySQL database.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 3.

   @section about About the program


   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.


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

   - The first is the lexer, also called the tokenizer. This module
     divides the input into logical tokens and classifies them. The
     lexer is written in the flex language, and is defined in
     transform_lex.y. Flex files consist of a list of regular
     expressions, and an action associated with each expression. The
     longest matching regular expression is always chosen.

   - The second is the actual parser. This module actually constructs
     the AST from the tokens. The lexer is written as a bison grammar
     and is defined in transform_yacc.y. Each token returned by the
     lexer is regarded as an input node to Bison. A bison grammar
     consists of a series of rules. Each rule tells how the compiler
     how to transform a set of zero or more nodes into a single new
     node. When parsing a file, these rules are repeatedly applied on
     the input until only a singe node remains, at which point the AST
     has been constructed and parsing is done.

   For more information about flex and bison, see the respective
   manuals.

   @subsection transformation Transformation

   This phase consists of various transformations of the AST. It is
   implemented using the transform method and functors deriving from
   the catalyst base class. The catalyst base class defines a functor
   that takes an AST node as argument and returns a node to replace it
   with.
   
   When the transform function is called for a specific node, its
   children have always all been transformed.

   @subsection output Output

   In this phase, the AST is written to an output stream. Each node
   has a _print method that should print that specific node and all
   its children in the correct order. The _print method is overloaded
   by most nodes to perform node type specific formating.

   Some nodes, like the cast node, which is used to convert an
   expression to a specific type, do all their actual work at print
   time.
      
   @subsection memory-management Memory managment

   The non-parser part of shield is written using a simple modern C++
   memory managment style where memory managment is handled implicityl
   and very few objects are dynamically allocated. Unfortunatly, bison
   requires all AST nodes to be inserted into a union, and C++ does
   not permit putting classes in union, so this is not possible in the
   parser. Instead, Shield has an extremly simple allocation strategy:
   The entire AST inherits from the \c printable base class, and the
   constructor of printable inserts every new element into a vector.
   Once an AST tree has been written, every element of the vector is
   deleted. This has a few drawbacks:

   - Multiple inheritance is not allowed. This can probably be worked
     around by overloading the new operator.

   - Multithreading in the parser is not allowed without some clever
     coding. In order to have multithreading, one would need to use
     Thread Local Storage for the allocation vector.

   - All AST node elements _must_ be dnamically allocated, since there
     is no way for the printable constructor to know if it should be
     dynamically allocated or not. This can probably be worked around
     by overloading the new operator.

   The benefit of this memory model is that memory leaks in the parser
   become extremely rare. The above limitations are not a serious
   limitations, and can be worked around with some additional effort
   if need be.
   
   @subsection style Coding style

   Shield relies heavily on dynamic casting to perform its task. Many
   catalysts only work on one specific type of nodes, and test each
   node using dynamic casting.

   @subsection ast-structure AST structure

   After finishing the parsing phase, the parser creates a fake_root
   object containing a chian to be the root of the AST. That means
   that if any query wishes to add additional queries to the tree this
   can be done during the transformation phase. The _add_query ()
   method does just that. Keep in mind that a node must _never_ call
   _add_query during the parsing phase, or an exception will be
   thrown.

   @section issues Known issues and differences between MySQL and Shield

   In MySQL, adding a number to the output of the date function
   results in mysql stripping the month and day part of the date and
   performing arithmetic on the year. In shield, performing the same
   operation results in the whole date value being converted to a
   number, the same way as when performing arithmetic on e.g. the
   output of the curdate function.

   Shield only supports the parts of the MySQL syntax directly needed
   to run Joomla under Oracle. Since Joomla is a relatively large and
   complex system, this actually means that a pretty large part of the
   MySQL syntax is covered, though.

   Shield crops long table/index/field names since Oracle has
   significantly shorter maximum name length than MySQL. Make sure
   that the first 13 characters of each table, index and field name is
   unique.

   The order of fields in a select list can be changed when using the
   limit clause or the group by clause in combination with wildcards
   or unaggregated field values.

   Here is an incomplete list of common features that are implemented:

   - 'select ...' queries, inclding limit clauses, 'group by ...' clauses, 'having ...' clauses, etc.
   - 'create table ...' queries, including primary keys, indices, dates, default values, nullable fields.
   - 'show tables ...' queries on the current database
   - 'delete from ...' queries
   - 'drop table ...' queries, incling the 'if exists' caluse
   - 'insert ...' queries
   - 'update ...' queries

   Here is an incomplete list of semi-common features that are missing
   from shield:

   - 'replace ...' queries
   - 'insert ... on duplicate key ...' queries
   - 'alter ...' queries

   Some other shield limitations:

   - Various information, such as storage engine, index type, etc. is ignored by shield
   - Named primary keys and unnamed indexes are unsupported


   @section type-mapping Type mapping

   Short text fields (char and varchar) are mapped to chars and
   varchar2s. Longer fields are mapped to clob.

   All fix point number types are mapped to the number type.

   All floating point number types are mapped to the double type.

   The date and datetime types are mapped to the Oracle date
   type. Shield uses an internal table to remember which fields should
   be dates and which should be datetimes.


   @section implementation-details Implementation details:

   @subsection empty-string Handling empty strings

   In Oracle, the empty string is the same thing as the null
   value. This is not the case in MySQL. To work around this, shield
   stores the empty string as a single character with binary value
   1. The client needs to unconvert this.

   @subsection limit Handling limit clauses

   Shield implements the limit clause using a double subselect,
   e.g. 'select ..., rownum r from (select ... (select ...) where
   rownum < ... ) where r > ...'. In oracle, subselects can not be
   mixed with field wildcards, e.g 'select *'. According to web
   sources like 'Ask Tom', this is the canonical way to implement
   limits in Oracle, and it is also reported that this is actually in
   good taste. 

   Oracle has many limitations on how wildcards can be used with group
   by clauses, as well as differences in how fields are named. In the
   end, this all means that Shield expands any and all wildcards in
   the field selection list of select statements.

   @subsection nameing Handling differences in naming
   
   In MySQL, index names live in a per-table namespace, so it is
   possible to have two indices in two different tables with the same
   name. In Oracle, that is not the case. To work around this, the
   table name and an underscore is prepended to all index names. This
   leads to another problem in Oracle - the 30 character name limit of
   all tables, indices, fields, etc. When an index named would have
   broken the 30 character limit, the name is shortened. Care is taken
   to use a prefix of both the table and original index name in the
   derived name, so as to minimize the collision risk, but this is not
   fool proof. 

   @subsection casting Handling the need for casting

   MySQL dates can be implicitly converted to strings and the other
   way around. Also, strings and numbers can be compared to each other
   without manual conversion. This is not possible with Oracle dates,
   you always need to use casting functions. For this reason, Shield
   keeps track of the type of each expression and performs casting as
   needed.
   
   It should be noted that it is not possible to directly compare to
   clobs in Oracle. They need to be converted to chars, which have a
   maximum length of 32000 characters. If you need to compare fields
   with a maximum langth of more than 32000 characters, you are in
   trouble.

   @section aliases Handling naming and aliases

   MySQL and Oracle differ in how column names are returned in select
   queries. To get something approximating MySQL style field names from
   Oracle, Shield returns all column names, one per line, in a comment
   before every select query.

   @section shield-package The shield package

   The shield program uses various shield specific PL/SQL functions,
   tables and other data.  The source code for all this package is
   given when shield is called with the --package switch. Ideally, all
   functions and such would be contained in the package (which is very
   cleverly named 'shield'), but it is impossible to make
   e.g. aggregation functions into members of a package, so this is
   not the case. In the end, all functions and tables outside of the
   shield package have the 'shield_' prefix.

   @section performance Performance

   There are many possibilities for increasing the speed at which
   Shield executes. At the time when this document was written, the
   most obvious ways to this are:

   - Rewrite the query cache to strip away the content of literals in
     the cache to make all queries with the same structure but
     different data use the same query cache item. This should enable
     a noticablly higher cache hit rate, probably around 95
     %. Currently, the cache hit rate under Joomla is ~80 %.

   - Replace the use of dynamically generated queries with stored
     procedures to avoid rerunning compliation/optimization on every
     query.

  Both of these optimizations require significant amounts of work and
  could potentiall significantly complicate the code base. Profiling
  done using Joomla show that on a basline Joomla installation with
  only the sample data installed, PHP spends 25 % of the time working
  on Joomla code, 25 % either in the shield database abstraction or
  waiting for shield, and 50 % waiting on Oracle. The fact that Oracle
  takes roughly double the time shield takes implies that the possible
  savings from optimizing shield itself is small. This in turn implies
  that the second optimization, using stored procedures, might bring
  forth significant performance savings.

*/

#ifndef SHIELD_HH
#define SHIELD_HH

#include "enum_char.hh"


namespace logger
{
  class logger;
}
/**
   @namespace shield

   The main namespece of the Shield program. All shield specific
   namespaces are nested under this namespace. Only a very limited set
   of definitions that are used in multiple subspaces live directly in
   this namespace.

   
*/

namespace shield
{

  /**
     This is not a function, it is a magical enum whose members can be
     converted to strings. 

     The members represent different sql data types.

     @param data_type the name of the enum

	@param DATA_TYPE_UNDEFINED unknown data type, this is the type returned from literals and such. It is sortable.
	@param DATA_TYPE_CLOB The clob type
	@param DATA_TYPE_NUMBER  A fixed point number type
	@param DATA_TYPE_FLOAT A floating point number type
	@param DATA_TYPE_CHAR A character type
	@param DATA_TYPE_VARCHAR A varble length character type
	@param DATA_TYPE_DATE A date
	@param DATA_TYPE_DATETIME A date and time
  */
  ENUM_STRING (data_type,
	       DATA_TYPE_CLOB=1,
	       DATA_TYPE_UNDEFINED=2, 
	       DATA_TYPE_NUMBER=4, 
	       DATA_TYPE_FLOAT=8, 
	       DATA_TYPE_CHAR=16,
	       DATA_TYPE_VARCHAR=32,
	       DATA_TYPE_DATE=64,
	       DATA_TYPE_DATETIME=128
	       );

  /**
     All data types that can be sorted. Everything except clob,
     basically.
  */
  const int DATA_TYPE_SORTABLE = DATA_TYPE_UNDEFINED | DATA_TYPE_NUMBER | DATA_TYPE_FLOAT | DATA_TYPE_CHAR | DATA_TYPE_VARCHAR | DATA_TYPE_DATE | DATA_TYPE_DATETIME;

  /**
     All data types that can be used as select fields. This does not
     include the date and datetime types, becvause oracle returns them
     as numbers, but mysql returns them aas strings. We need to make
     an implicit conversion.
  */
  const int DATA_TYPE_SELECTABLE = DATA_TYPE_CLOB | DATA_TYPE_UNDEFINED | DATA_TYPE_NUMBER | DATA_TYPE_FLOAT | DATA_TYPE_CHAR | DATA_TYPE_VARCHAR;

  /**
     Global logger for errors
  */
  extern logger::logger error;

  /**
     Returns the shield_multiplex socket filename
  */
  std::string get_socket_name();

  /**
     Parse the specified argument vector
  */
  int parse_args (int argc, char **argv);

}

#endif //#ifndef SHIELD_HH
