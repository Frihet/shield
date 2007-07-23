/**
   @file catalyst.hh

   Headers for all catalysts. Catalysts are functors used to transform the AST. 

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz

   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.


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
    class identity;
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

       When the transform function is called for a specific node, its
       children have always all been transformed.
    */
    class catalyst
    {
    public:

      catalyst ()
	: __called (false)
      {
      }

      /**
	 The transformation operation. Takes a printable as an
	 argument and returns another one, or possibly the same one.
	 
	 @param node The node to transform
      */
      transform::printable *operator () (transform::printable *node);

      virtual transform::printable *catalyze (transform::printable *node)=0;

      virtual ~catalyst ()
      {
      }

    private:
      
      /**
	 This set contains a listing of all transformed nodes. It is
	 used to keep the same node from being transformed more than
	 once.
      */
      set<transform::printable *>__done;
      bool __called;
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

    
    /**
       This catalyst aggregates any ungrouped, unaggregated fields. 
    */
    class aggregate
      : public catalyst
    {
    public:
      
      aggregate (transform::query *q, const vector<transform::identity *> &group_field);
      
      /**
	 The catalyst function. Detects printable_alias nodes and
	 handles them.
      */
      virtual transform::printable *catalyze (transform::printable *node);
      
    private:

      transform::printable *__aggregate (transform::text *field, transform::text *table_alias);

    private:
      vector<transform::identity *>__group_field;
      transform::query *__query;

    }
    ;

    class set_selectable
      : public catalyst
    {
    public:
      set_selectable (bool selectable)
	: __selectable (selectable)
      {
      }

      virtual transform::printable *catalyze (transform::printable *node)
      {
	node->set_selectable (__selectable);
	return node;
      }

    private:
      bool __selectable;
    }
    ;

  }

}

#endif //#ifndef CATALYST_HH

