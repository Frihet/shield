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
   
   When the transform function is called for a specific node, its
   children have always all been transformed.

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

   @subsection ast-structure AST structure

   After finisheing the parsing phase, the parser creates a fake_root
   object containing a chian to be the root of the AST. That means
   that if any query wishes to add additional queries to the tree this
   can be done during the transformation phase. The _add_query ()
   method does just that. Keep in mind that a node must _never_ call
   _add_query during the parsing phase, or an exception will be
   thrown.



*/

#ifndef SHIELD_HH
#define SHIELD_HH

#include "enum_char.hh"

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
	       DATA_TYPE_DATETIME=128,
	       );

  /**
     All data types that can be sorted. Everything except clob, basically.
  */
  const int DATA_TYPE_SORTABLE = DATA_TYPE_UNDEFINED | DATA_TYPE_NUMBER | DATA_TYPE_FLOAT | DATA_TYPE_CHAR | DATA_TYPE_VARCHAR | DATA_TYPE_DATE | DATA_TYPE_DATETIME;

  /**
     All data types that can be used as select fields. This does not
     include the date and datetime types, becvause oracle returns them
     as numbers, but mysql returns them aas strings. We need to make
     an implicit conversion.
  */
  const int DATA_TYPE_SELECTABLE = DATA_TYPE_CLOB | DATA_TYPE_UNDEFINED | DATA_TYPE_NUMBER | DATA_TYPE_FLOAT | DATA_TYPE_CHAR | DATA_TYPE_VARCHAR;


}

#endif //#ifndef SHIELD_HH
