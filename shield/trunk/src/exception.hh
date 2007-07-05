/**
@file exception.hh

Various exceptions generated by different parts of shield.

@package Shield
@author Axel Liljencrantz

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 or later.

*/

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <string>
#include <iostream>
#include <exception>

using namespace std;

namespace shield
{

  namespace exception
  {

    class unsupported
      : public std::exception
    {
    public:
  
      unsupported (const string &file, int line);
  
      virtual const char *what () const throw ();

      virtual ~unsupported () throw ()
      {
      }

    private:
      string __str;
    };

    class syntax
      : public std::exception
    {
    public:
  
      syntax (const string &err);
  
      virtual const char *what () const throw ();

      virtual ~syntax () throw ()
      {
      }

    private:
      string __str;

    };

    class not_found
      : public std::exception
    {
    public:

      not_found (const string &not_found);

      virtual const char *what () const throw ();

      virtual ~not_found () throw ()
      {
      }

    private:
      string __str;

    };

  }

}

#endif

