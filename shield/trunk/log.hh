#ifndef LOG_HH
#define LOG_HH

#include <string>
#include <iostream>
#include <set>

class type_info_compare
{
public:
  bool operator () (const std::type_info *type1, const std::type_info *type2)
  {
    std::cerr << "Compare " << type1->name () << " and " << type1->name () << std::endl;
    bool res = type1->before (*type2);
    std::cerr << "Result " << res << std::endl;
    return !res;
  }

}
;
class log_stream
  : public std::ostream
{
public:
  
  friend log_stream &operator << (log_stream &log, const std::string &msg);

  log_stream (std::ostream &stream, bool print_unknown = false)
    : __stream (stream), __print_unknown (print_unknown)
  {
  }

  void set_logging (const std::type_info &type, bool do_log)
  {
    
    if (do_log)
      {
	std::cerr << "Accept " << type.name () << std::endl;
	__ignore.erase (&type);
	__accept.insert (&type);
      }
    else
      {
	__ignore.insert (&type);
	__accept.erase (&type);
      }

  }

protected:
  
private:
  void __print (const std::string &msg)
  {
    std::cerr << "Log item " << msg << std::endl;
    
    if (__print_unknown)
      {
	if (__ignore.find ( &typeid  (msg)) == __ignore.end ())
	  {
	    __stream << msg << std::endl;
	  }
      }
    else
      {
	std::cerr << "Don't print unknown items" << std::endl;
	
	if (__accept.find ( &typeid  (msg)) != __accept.end ())
	  {
	    std::cerr << "PRIIINT" << std::endl;
	    __stream << msg << std::endl;
	  }
      }
  }

private:

  std::ostream &__stream;
  bool __print_unknown;

  std::set<const std::type_info *, type_info_compare> __accept;
  std::set<const std::type_info *, type_info_compare> __ignore;
}
;

log_stream &operator << (log_stream &log, const std::string &msg)
{
  log.__print (msg);
}



#endif
