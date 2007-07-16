#include <iostream>


//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
//#include <pwd.h>
#include <fcntl.h>


#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <errno.h>
#include <locale.h>
//#include <signal.h


#include "include/logger.hh"

using namespace std;

namespace
{

  logger::logger debug ("shield_multiplex debug");
  logger::logger warning ("shield_multiplex warning");
  int listen_socket;

  int get_socket()
  {
    int s, len, doexit = 0;

    struct sockaddr_un local;
    char *sock_name = "/tmp/shield.socket";

    local.sun_family = AF_UNIX;
    strcpy( local.sun_path, sock_name );
    len = sizeof(local);

    debug << "Connect to socket";

    s = socket( AF_UNIX, SOCK_STREAM, 0 );
    if( s == -1 )
      {
	perror( "socket" );
	doexit = 1;
      }
    else
      {
	/*
	  First check whether the socket has been opened by another process;
	  if so, exit.
	*/
	if( connect( s, (struct sockaddr *)&local, len ) == 0 )
	  {
	    debug << "Socket already exists, exiting";
	    doexit = 1;
	  }
	else
	  {
	    unlink( local.sun_path );

	    if( bind( s, (struct sockaddr *)&local, len ) == -1 )
	      {
		perror( "bind" );
		doexit = 1;
	      }
	    else
	      {
		if( fcntl( s, F_SETFL, O_NONBLOCK ) != 0 )
		  {
		    perror( "fcntl" );
		    close( s );
		    doexit = 1;
		  } 
		else if( listen( s, 64 ) == -1 )
		  {
		    perror( "listen" );
		    doexit = 1;
		  }
	      }
	  }
      }

    if( doexit )
      {
	exit( 1 );
      }

    return s;
  }

  /**
     Make program into a creature of the night.
  */
  void daemonize()
  {

    /*
      Fork, and let parent exit
    */
    switch( fork() )
      {
      case -1:
	debug << "Could not put program in background. Quitting";
	perror( "fork" );
	exit(1);

      case 0:
	{
	  break;
	}

      default:
	{
	  debug << "Parent exiting after daemonization";
	  exit(0);
	}
      }

    /*
      Put ourself in out own processing group
    */
    setsid();

    /*
      Close stdin and stdout. We only use stderr, anyway.
    */
    //    cout.rdbuf ()->close ();
    //cin.rdbuf ()->close ();

  }


  /**
     Do all sorts of boring initialization.
  */
  void init()
  {
    debug.enable ();
    listen_socket = get_socket();
    daemonize();
  }


  void handle_query (int fd)
  {

  }

}




int main (int argc, char **argv)
{

  fd_set read_fd, write_fd;
  int child_socket;
  struct sockaddr_un remote;
  socklen_t t;
  int max_fd;
  int res;

  init();
  while(1)
    {
      t = sizeof( remote );

      FD_ZERO( &read_fd );
      FD_SET( listen_socket, &read_fd );

      max_fd = listen_socket+1;

      while( 1 )
        {
	  res=select( max_fd, &read_fd, &write_fd, 0, 0 );

	  if( res != -1 )
	    break;

	  if( errno != EINTR )
            {
	      perror( "select" );
	      exit(1);
            }

        }

      if( FD_ISSET( listen_socket, &read_fd ) )
        {
	  if( (child_socket =
	       accept( listen_socket,
		       (struct sockaddr *)&remote,
		       &t) ) == -1)
	    {
	      perror( "accept" );
	      exit(1);
	    }
	  else
            {
	      debug <<"Connected with new child";

	      handle_query (child_socket);
	      close (child_socket);
            }
        }

    }

}

