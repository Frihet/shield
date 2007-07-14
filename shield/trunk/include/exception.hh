/**
@file exception.hh

Various exceptions generated by different parts of shield. Only the
exception subclasses defined in this file are ever thrown by any
Shield code.

Note that the classes themselves, not pointers to the classes are
thrown. Catch statements should catch a reference to the
exception. That way, the exception will be deallocated at the end of
the catch clause with no explicit memory management.

All these exceptions in this namespace return a stack trace toghether
with their error message.

@package Shield
@author Axel Liljencrantz

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 3.

*/

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <string>
#include <iostream>
#include <exception>

namespace shield
{

  /**
     @namespace shield::exception

     All Shield exceptions. All exceptions in this namespace inherit
     the ability to to print a stack trace from
     shield::exception::exception. On platforms where the function
     calls in the stack trace are mangled, the names are autometically
     unmangled to make the trace significantly easier to read.
  */
  namespace exception
  {

    using namespace std;
    
    /**
       The base class for all shield exceptions. Contains convenience
       methods for easily printing out an error message and an associated stack trace.
    */
    class exception
      : public std::exception
    {
    public:

      exception (const exception &e)
      {
	__what = strdup (e.__what);
      }

      /**
	 Create a stack trace of the calling function. 
	 
	 On platforms where the function calls in the stack trace are
	 mangled, the names are automatically unmangled to
	 make the trace significantly easier to read.
	 
	 It would be very nice to get line numbers for the stack
	 trace, but there does not seem to be any method of obtaining
	 that. 
      */
      exception (void);

      /**
	 Returns the string specified using \c _set_what () and a human readable stack trace.
      */
      virtual const char *what (void) const throw ()
      {
	return __what;
      }
      
      virtual ~exception (void) throw ()
      {
	free (__what);
      }

    protected:

      /**
	 Set the return message of the \c what () functions.
      */
      void _set_what (const string &what)
      {
	string tmp = what + "\n" + __stack_trace;
	__what = strdup (tmp.c_str ());
      }

    private:

      /**
	 The message string returned by what ().
      */
      char *__what;

      /**
	 The stack trace, which is created by the constructor.
      */
      string __stack_trace;
    }
    ;
 

   /**
       This exception is thrown when a a valid SQL query is entered, which shield can not handle because that functionality has yet to be implemented.
       This is thrown in lots and lots of places in the parser, since only a very small subset of the entire syntax tree is supported.       
    */
    class unsupported
      : public exception
    {
    public:

      unsupported (const string &file, int line);
      unsupported (const string &message);
  
    }
    ;

    /**
       This exception is thrown when the later stages of parsing encounter a problem in the supplied syntax.

       Earlier stages of parsing i.e. error in the bison grammar, are
       communicated using th yyerror function.
    */
    class syntax
      : public exception
    {
    public:
  
      syntax (const string &err)
      {
	_set_what ("Syntax error: "+ err);
      }  
    };

    /**
       This exception is thrown when expected data was not present.

       This exception is often thrown when an AST node expected to
       have a child node of a specific type, but it was not present.
    */
    class not_found
      : public exception
    {
    public:

      not_found (const string &not_found)
      {
	_set_what ("Could not find item '" + not_found + "'");
      }
    };

    /**
       This exception is thrown when data (usually an AST node) was of an unexpected class.
    */
    class invalid_type
      : public exception
    {
    public:

      invalid_type (const string &where, const string &what)
      {
	_set_what ("Item in '" + where + "' was not of expected type '" + what + "'");
      }

    }
    ;

    /**
       Exception for when data (usually an AST node) is in
       an invalid state, such as if there are cycles in the the tree
       or if a node is of an unexpected type. 
    */
    class invalid_state
      : public exception
    {
    public:

      invalid_state (const string &what)
      {
	_set_what (what);
      }

    }
    ;

    /**
       Exception for an invalid input parameter to a method or function. 
    */
    class invalid_param
      : public exception
    {
    public:

      invalid_param (const string &what)
      {
	_set_what (string ("Invalid parameter: ")+what);
      }

    }
    ;

    /**
       Exception for database errors.
    */
    class database
      : public exception
    {
    public:

      database (const string &what)
      {
	_set_what (what);
      }

    }
    ;

  }

}

#endif //#ifndef EXCEPTION_HH

