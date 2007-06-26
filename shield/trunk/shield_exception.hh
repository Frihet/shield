#ifndef SHIELD_EXCEPTION_HH
#define SHIELD_EXCEPTION_HH

#include <string>
#include <iostream>
#include <vector>
#include <stdarg.h>

using namespace std;

namespace shield
{

class unsupported_exception
{
public:
  
  unsupported_exception (const string &file, int line);
  
  friend ostream &operator << (ostream &stream, const unsupported_exception &e);
  
private:
  string __file;
  int __line;

};

ostream &operator << (ostream &stream, const unsupported_exception &e);

class syntax_exception
{
public:
  
  syntax_exception (const string &err);
  
  friend ostream &operator << (ostream &stream, const syntax_exception &e);
  
private:
  string __err;

};

ostream &operator << (ostream &stream, const syntax_exception &e);

class not_found_exception
{
public:

  not_found_exception (const string &not_found);

  friend ostream &operator << (ostream &stream, const not_found_exception &e);
  
private:
  string __not_found;

};


}

#endif

