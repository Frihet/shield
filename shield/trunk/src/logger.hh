/**
@file logger.hh

A simple, generic logger class making it easy to selectively log very
specific log event types. The logger does not contain any concept of
loglevels. Instead, the idea is to create lots and lots of logger
objects. That way, it becomes easy to enable logging of a specific
type of event, thus avoiding being drowned in unrelated log messages
whever additional logging information is enabled.

@author Axel Liljencrantz

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 or later.

*/

#ifndef LOGGER_HH
#define LOGGER_HH

#include <string>
#include <iostream>
#include <set>
/**
@namespace logger

Logger namespace. Defined in the logger.hh file. Provides the logger class. For performing logging. 

*/

namespace logger
{
  using namespace std;
  /**
     Logging class. One log should be created for every class/level of
     message, and then individual classes are enabled or disabled.
  */

  class logger
  {
  public:
    
    /**
       Construct a new log.

       @param name The name to prepend to every log message to identify the program and/or log type
       @param stream the stream to log to
       @param enabled whether this log should start up enabled
    */
    logger (const string &name, ostream &stream=cerr, bool enabled = false)
      : __name (name), __stream (stream), __enabled (enabled)
    {
    }

    /**
       Enables this logger, i.e. makes it actually output log messages sent to it
    */
    void enable (void)
    {
      __enabled = true;
    }

    /**
       Disables this logger, i.e. makes it not actually output log messages sent to it
    */
    void disable (void)
    {
      __enabled = false;
    }

    /**
       Returns true if this logger is enabled
    */
    bool is_enabled (void)
    {
      return __enabled;
    }

    /**
       The logging operator
    */
    logger &operator << (const string &message);

    /**
       Oveloaded operator for handling character literals
    */
    logger &operator << (const char *message)
    {
      *this << string (message);
      return *this;
    }

  private:

    /**
       Returns a string containing a human-readable time stamp
    */
    string __timestamp (void);

  private:

    /**
       This flag is true if this logger is enabled
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

#endif //#ifndef LOGGER_HH
