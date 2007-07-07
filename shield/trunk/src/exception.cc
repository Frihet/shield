
#include <string>
#include <execinfo.h>
#include <signal.h>

#include "exception.hh"
#include "util.hh"

namespace shield
{
  
  namespace exception
  {
    
    namespace
    {
      const int BACKTRACE_COUNT=128;
    }
    
    exception::
    exception ()
    {
      void * array[BACKTRACE_COUNT];
      int nSize = backtrace(array, BACKTRACE_COUNT);
      char ** symbols = backtrace_symbols(array, nSize);

      for (int i = 0; i < nSize; i++)
	{
	  string str(symbols[i]);
	  string::const_iterator it, begin;
	  bool found_paran = false;

	  for (it=str.begin (); it!=str.end (); ++it)
	    {
	      if (*it=='#')
		{
		  begin = it+1;
		  found_paran = true;
		__stack_trace += *it;
		}
	      else if (*it=='+')
		{
		  if (found_paran)
		    {
		      string sub (begin, it);
		      __stack_trace += util::cxx_demangle (sub.c_str ());
		      
		      begin = it;
		      break;
		    }
		}
	      
	      if (!found_paran)
		__stack_trace += *it;
	      
	    }
	  
	  if (found_paran)
	    {
	      string::const_iterator end = str.end ();
	      __stack_trace.append (it, end);
	    }
	  
	  __stack_trace += "\n";
	}
      
      __stack_trace += "\n";
      
      free(symbols);
    }
    
  }

}
