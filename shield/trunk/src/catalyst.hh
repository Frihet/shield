/**
   @file catalyst.hh

   Headers for all catalysts. Catalysts are functors used to transform the AST. 

   @remark package: shield

   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 or later.


*/
#ifndef CATALYST_HH
#define CATALYST_HH

#include <string>
#include <map>
#include <set>

#include "logger.hh"

namespace shield
{

  namespace transform
  {
    class printable;
    class query;
    class chain;
  }

  /**
     
  @namespace shield::catalyst
  
  Functors that traverse the AST and transform the nodes.
  */

  namespace catalyst
  {

    using namespace std;

    /**
       Logger object for transform warnings
    */
    extern logger::logger warning;
    /**
       Logger object for transform debug output
    */
    extern logger::logger debug;
    
    /**
       This is a functor class used to transform printables. It takes a
       printable and returns it or another printable. It is used
       together with the \c printable::transform function of printables to perform
       transformations on the syntax tree.
    */
    class catalyst
    {
    public:

      /**
	 The transformation operation. Takes a printable as an
	 argument and returns another one, or possibly the same one.
	 
	 @param node The node to transform
      */
      transform::printable *operator () (transform::printable *node);
      virtual transform::printable *catalyze (transform::printable *node)=0;
    private:
      
      /**
	 This set contains a listing of all transformed nodes. It is
	 used to keep the same node from being transformed more than
	 once.
      */
      set<transform::printable *>__done;
    };


    /**
       A catalyst fo replacing a specific shield_text with another
       printable. This is used in the having clause of selects,
       because oracle forbids the use of aliases there for some odd
       reason.
    */
    class replace_identifier
      : public catalyst
    {
    public:

      replace_identifier (map<string, transform::printable *> &mapping)
	: __mapping (mapping)
      {
      }

      virtual transform::printable *catalyze (transform::printable *node);

    private:
      map<string, transform::printable *> __mapping;
    }
    ;

    /**
       Simple class to validate that the tree is sane. Never changes
       any nodes. Detects null nodes, orphaned nodes, nodes who are
       their own parents and query-less.
    */
    class validation
      : public catalyst
    {
    public:
      
      virtual transform::printable *catalyze (transform::printable *node);
    }
    ;

    /**
       This catalyst transforms any identifiers that are represented
       by a text node into one represented by an identity node.
    */
    class create_identity
      : public catalyst
    {
    public:
      create_identity (transform::query *q)
	: __query (q)
      {
      }

      virtual transform::printable *catalyze (transform::printable *node);
      
    private:
      transform::query *__query;
    }
    ;

    /**
       Perform all node-specific transforms.

       Every AST node has the option of specifying a transform that
       should be performed on that node. This is a simple method for
       delaying the execution of some init code from parse time till
       after the full tree has been parsed and can be safely
       traversed. The \c internal catalyst traverses the entire tree
       and calls all such internal transform methods.
    */
    class internal 
      : public catalyst
    {
    public:
      virtual transform::printable *catalyze (transform::printable *node);
    }
    ;

    /**
       This catalyst locates any table names in a from clause. If
       there is a table alias, it is returned in preference over the
       actual table name. 
    */
    class find_table
      : public catalyst
    {
    public:

      find_table (transform::query *q);

      /**
	 The catalyst function. Detects printable_alias nodes and
	 handles them.
      */
      virtual transform::printable *catalyze (transform::printable *node);

      /**
	 Return the accumulated list of tables
      */
      vector<transform::printable *> &get_table_list (void)
      {
	return __res;
      }
      
    private:
      vector<transform::printable *>__res;
      transform::query *__query;
    }
    ;

  }

}

#endif //#ifndef CATALYST_HH

