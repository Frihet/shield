/**
@file catalyst.hh

Headers for all catalysts. Catalysts are functors used to transform the AST. 

*/
#ifndef CATALYST_HH
#define CATALYST_HH

#include <string>
//#include <iostream>
#include <vector>
#include <map>
//#include <stdarg.h>
//#include <assert.h>

#include "logger.hh"

namespace shield
{

  namespace transform
  {
    class printable;
    class query;
    class chain;
  }

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
      virtual transform::printable *operator () (transform::printable *node) = 0;
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

      virtual transform::printable *operator () (transform::printable *node);

    private:
      map<string, transform::printable *> __mapping;
    }
    ;

    /**
       Simple class to validate that the tree is sane. Never changes any nodes.
    */
    class validation
      : public catalyst
    {
    public:
      
      virtual transform::printable *operator () (transform::printable *node);
    }
    ;

    /**
       This catalyst transforms any identifiers that are represented
       by a text node into one represented by an identity node.
    */
    class identity
      : public catalyst
    {
    public:
      identity (transform::query *q)
	: __query (q)
      {
      }

      virtual transform::printable *operator () (transform::printable *node);
      
    private:
      transform::query *__query;
    }
    ;

    class internal 
      : public catalyst
    {
    public:
      virtual transform::printable *operator () (transform::printable *node);
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
      virtual transform::printable *operator () (transform::printable *node);

      /**
	 Return the accumulated list of tables
      */
      transform::chain *get_table_list (void)
      {
	return __res;
      }
      
    private:
      transform::chain *__res;
      transform::query *__query;
    }
    ;

  }

}

#endif //#ifndef CATALYST_HH

