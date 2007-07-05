#ifndef LOG_HH
#define LOG_HH

#include <string>
#include <iostream>
#include <set>

namespace log
{
  using namespace std;
  /**
     Logging class. One log should be created for every class/level of
     message, and then individual classes are enabled or disabled.
  */

  class log
  {
  public:
    
    /**
       Construct a new log.

       @param name The name to prepend to every log message to identify the program and/or log type
       
       @param stream the stream to log to

       @param enabled whether this log should start up enabled
    */
    log (const string &name, ostream &stream=cerr, bool enabled = false)
      : __name (name), __stream (stream), __enabled (enabled)
    {
    }

    /**
       Enables this log, i.e. makes it actually output log messages sent to it
    */
    void enable (void)
    {
      __enabled = true;
    }

    /**
       Disables this log, i.e. makes it not actually output log messages sent to it
    */
    void disable (void)
    {
      __enabled = false;
    }

    /**
       Returns true if this log is enabled
    */
    bool is_enabled (void)
    {
      return __enabled;
    }

    /**
       The logging operator
    */
    log &operator << (const string &message);

    /**
       Oveloaded operator for handling character literals
    */
    log &operator << (const char *message)
    {
      *this << string (message);
      return *this;
    }

  private:

    string __timestamp (void);

  private:

    /**
       This flag is true if this log is enabled
    */
    bool __enabled;
    
    /**
       This is the underlying stream that one logs to
    */
    ostream &__stream;

    /**
       The name to use first in all messages
    */
    string __name;

  }
  ;
}

#endif //#ifndef LOG_HH
