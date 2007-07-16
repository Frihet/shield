/**

   @file shield_multiplex.cc

   The Shield socket connection multiplexer. Nothing interesting
   happens here, this is only a small program to allow multiple
   processes access to the same Shield process using a simple socket
   interface.

   The program automatically daemonizes itself and listens for queries
   on a socket, using the exact same, barindead interface used by the
   main shield binary.

   @remark package: shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz
   
   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.
   
   This file uses the C socket library, which obviouly uses C-style
   streams and not C++ style streams. Because of this, the code uses
   quite a bit of C-isms here and there. The second generation
   multiplexer should use a C++ wrapper around the socket interface. It
   did not seem to be worth the effort of doing this for this simple
   first-generation hack.
   
   This file borrows a lot of code from the fishd.c file from the fish
   shell (http://www.fishshell.org), which is actually licensed under
   GPLv2. But since I am the author of fish I am allowed to relicense
   it under GPLv3 and include it in Shield. Yay!!
*/

#include <iostream>

#ifdef HAVE_SYS_SELECT_H
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <errno.h>
#include <locale.h>


#include <locale.h>
#include <getopt.h>

#include "include/logger.hh"
#include "include/util.hh"

using namespace std;

namespace
{

  logger::logger error ("shield_multiplex error");
  logger::logger debug ("shield_multiplex debug");
  logger::logger warning ("shield_multiplex warning");
  int listen_socket;

  class context
  {
  public:
    int shield_fd[0];
    int shield_pid;
  }
    ;

  /**
     Print a short help message.
     
     @param stream the stream to print the message to
  */
  void print_help (ostream &stream)
  {
    stream << "Usage: shield_multiplex.cc [OPTION]... ." << endl;
    stream << "Convert MySQL code to Oracle code through a socket interface." << endl;
  }

  pid_t retry_fork ()
  {
    pid_t pid;
    while (true)
      {
	pid = fork ();
	if (pid != (pid_t)-1)
	  break;
	
	if (errno != EINTR)
	  {
	    error << (string("Error in 'fork' function call: ") + strerror (errno));
	    break;
	  }
      }
    return pid;
  }
  
  int retry_pipe (int *fd)
  {
    int res=0;
    while(true)
      {
	res = pipe (fd);
	if (!res)
	  break;

	if (errno != EINTR)
	  {
	    error << (string("Error in 'pipe' function call: ") + strerror (errno));
	    break;
	  }
      }

    return res;
  }
  
  int retry_close (int fd)
  {
    int res;
    while(true)
      {
	res = close (fd);
	if (!res)
	  break;

	if (errno != EINTR)
	  {
	    error << (string("Error in 'close' function call: ") + strerror (errno));
	    break;
	  }
      }

    return res;
  }
  
  int retry_dup (int fd)
  {
    int res;
    while(true)
      {
	res = dup (fd);
	if (res != -1)
	  break;

	if (errno != EINTR)
	  {
	    error << (string("Error in 'dup' function call: ") + strerror (errno));
	    break;
	  }
      }

    return res;    
  }

  
  pid_t create_child (char **argv, char **envp, int *fds)
  {
    pid_t pid;
    
    if (retry_pipe (fds))
      {
	error << "Could not create pipe for communicating with child";
	exit(1);
      }

    
    pid = retry_fork ();
    switch (pid)
      {
      case (pid_t)-1:
	/*
	  Something went south. Lets split before it gets any worse.
	 */
	error << "Could not fork child process";
	exit(1);
	break;

      case 0:
	/*
	  I am child. Lets do childish stuff.
	*/
	
	if( retry_dup (fds[0])==-1 || retry_dup (fds[1])==-1)
	  {
	    error << "Failed to set up pipe to shield. Exiting.";
	    exit (1);
	  }
	execve("shield", argv, envp);
	error << (string("Function call 'execve' failed: ") + strerror (errno));
	exit (1);
	break;

      default:
	debug << "Parent done in create_child, returning";
	return pid;

      }
  }

  string translate_query (const string &query)
  {
    
    return "hej hopp\n\n";
  }

  void handle_query (context &ctx, int socket, const string &query)
  {
    
    if (query != "")
      {
	debug << string ("Read string ")+query;
	string query_out = translate_query (query);
	write (socket, query_out.c_str (), query_out.size ());
      }
	
  }    


  void handle_connection (context &ctx, int socket)
  {
    string str;

    while (true)
      {
	int c;

	char buff[1];

	ssize_t count = read (socket, buff, 1);
	if (count == (ssize_t)-1)
	  break;

	c = buff[0];
	
	if (c)
	  {
	    str += (char)c;
	  }
	else
	  {
	    handle_query (ctx, socket, str);
	    str="";
		
	  }
      }

    handle_query (ctx, socket, str);
    
  }



  int get_socket ()
  {
    int s, len, doexit = 0;

    struct sockaddr_un local;
    char *sock_name = "/tmp/shield.socket";

    local.sun_family = AF_UNIX;
    strcpy (local.sun_path, sock_name);
    len = sizeof (local);

    debug << "Connect to socket";

    s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
      {
	perror ("socket");
	doexit = 1;
      }
    else
      {
	/*
	  First check whether the socket has been opened by another process;
	  if so, exit.
	*/
	if (connect (s, (struct sockaddr *)&local, len) == 0)
	  {
	    debug << "Socket already exists, exiting";
	    doexit = 1;
	  }
	else
	  {
	    unlink (local.sun_path);

	    if (bind (s, (struct sockaddr *)&local, len) == -1)
	      {
		perror ("bind");
		doexit = 1;
	      }
	    else
	      {
		if (fcntl (s, F_SETFL, O_NONBLOCK) != 0)
		  {
		    perror ("fcntl");
		    retry_close (s);
		    doexit = 1;
		  } 
		else if (listen (s, 64) == -1)
		  {
		    perror ("listen");
		    doexit = 1;
		  }
	      }
	  }
      }

    if (doexit)
      {
	exit (1);
      }

    debug << (string ("Created new listener socket on fd ") + util::stringify (s));
    return s;
  }

  /**
     Make program into a creature of the night. This involves forking
     and letting the parent die (isn't that always the way with
     daemons?), putting ourselves in our own process group and killing
     stdin and stdout.

     FIXME:

     We should probably add a signal handler or two as well.
  */
  void daemonize ()
  {

    /*
      Fork, and let parent exit. 
    */
    switch (retry_fork ())
      {
      case (pid_t)-1:
	debug << "Could not put program in background. Quitting";
	perror ("fork");
	exit (1);

      case 0:
	{
	  debug << "Created child process";
	  break;
	}

      default:
	{	
	  debug << "Parent exiting after daemonization";
	  exit (0);
	}
      }

    /*
      Put ourself in our own processing group. Means we won't get any
      signal meant for our grandparent by accident.

      According to its manpages, this call cannot fail on a newly
      forked child. Imagine that - a Unix syscall that doesn't fail.
    */
    setsid ();


    /*
      Close stdin and stdout. We only use stderr, anyway. Makes sure
      we don't get any silly sighups that spoil the day. Also frees a
      few resources and prevents accidental reading of stdin/stdout.

      If retry_close fails, this probably means stdin/stdout are
      somehow dead, and there isn't much we can do about it. We need
      to reassign those descriptors in the child. We'll just have to
      exit...
    */
    if (retry_close (0) || retry_close (1))
      {
	error << "Could not close standard input/output. Exiting.";
	exit (1);
      }

    debug << "Process has been daemonized";

  }


  /**
     Do all sorts of boring initialization.
  */
  void init ()
  {
    error.enable ();
    error.set_pid (true);
    debug.enable ();
    debug.set_pid (true);
    

    listen_socket = get_socket ();
    daemonize ();
  }

}


int main (int argc, char **argv, char **envp)
{
  using namespace util;

  fd_set read_fd, write_fd;

  int client_socket;
  context ctx;

  struct sockaddr_un remote;
  socklen_t t;
  int max_fd;
  int res;
  
  init ();  
  ctx.shield_pid = create_child (argv, envp, ctx.shield_fd);

  handle_query (ctx, 2, "select now ()");

  debug << "Tested multiplexer, shutting down";
  exit(0);

  while(1)
    {
      t = sizeof (remote);

      FD_ZERO (&read_fd);
      FD_ZERO (&write_fd);
      FD_SET (listen_socket, &read_fd);

      max_fd = listen_socket+1;

      while (1)
        {
	  res=select (max_fd, &read_fd, &write_fd, 0, 0);

	  if (res != -1)
	    break;
	  
	  if (errno != EINTR)
            {
	      perror ("select");
	      exit (1);
            }

        }
      
      if (FD_ISSET (listen_socket, &read_fd))
        {
	  if ((client_socket =
	       accept (listen_socket,
		       (struct sockaddr *)&remote,
		       &t)) == -1)
	    {
	      perror ("accept");
	      exit (1);
	    }
	  else
            {
	      debug << "Connected with new child";

	      handle_connection (ctx, client_socket);
	      retry_close (client_socket);
            }
        }

    }

}

