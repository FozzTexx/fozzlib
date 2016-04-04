/* Copyright 1997-2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
** SOCKET.C
**
** Written by Steven Grimm (koreth@ssyx.ucsc.edu) on 11-26-87 (Thanksgiving)
** Please distribute widely, but leave my name here.
**
** Various black-box routines for socket manipulation, so I don't have to
** remember all the structure elements.
** Of course, I still have to remember how to call these routines.
*/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "socket.h"

#ifndef FD_SET		/* for 4.2BSD */
#define FD_SETSIZE      (sizeof(fd_set) * 8)
#define FD_SET(n, p)    (((fd_set *) (p))->fds_bits[0] |= (1 << ((n) % 32)))
#define FD_CLR(n, p)    (((fd_set *) (p))->fds_bits[0] &= ~(1 << ((n) % 32)))
#define FD_ISSET(n, p)  (((fd_set *) (p))->fds_bits[0] & (1 << ((n) % 32)))
#define FD_ZERO(p)      bzero((char *)(p), sizeof(*(p)))
#endif

extern int errno;

extern void bzero ();
extern void bcopy ();
#ifndef htons
extern u_short htons ();
#endif
#ifndef ntohs
extern u_short ntohs ();
#endif
extern void close ();
extern int select ();

/*
** serversock()
**
** Creates an internet socket, binds it to an address, and prepares it for
** subsequent accept() calls by calling listen().
**
** Input: port number desired, or 0 for a random one
** Output: file descriptor of socket, or a negative error
*/
int serversock(int port)
{
	int	sock, x;
	struct	sockaddr_in server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return -errno;

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	x = bind(sock, (struct sockaddr *) &server, sizeof(server));
	if (x < 0)
	{
		close(sock);
		return -errno;
	}

	listen(sock, 5);

	return sock;
}

/*
** portnum()
**
** Returns the internet port number for a socket.
**
** Input: file descriptor of socket
** Output: inet port number
*/
int portnum(int fd)
{
  socklen_t	length;
  int err;
  struct	sockaddr_in address;

  length = sizeof(address);
  err = getsockname(fd, (struct sockaddr *) &address, &length);
  if (err < 0)
    return -errno;

  return ntohs(address.sin_port);
}

/*
** clientsock()
**
** Returns a connected client socket.
**
** Input: host name and port number to connect to
** Output: file descriptor of CONNECTED socket, or a negative error (-9999
**         if the hostname was bad).
*/
int clientsock(const char *host, int port)
{
	int	sock;
	struct	sockaddr_in server;
	struct	hostent *hp, *gethostbyname();

	hp = gethostbyname(host);
	if (hp == NULL)
		return -9999;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return -errno;

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		close(sock);
		return -errno;
	}

	return sock;
}

/*
** readable()
**
** Poll a socket for pending input.  Returns immediately.  This is a front-end
** to waitread() below.
**
** Input: file descriptor to poll
** Output: 1 if data is available for reading
*/
int readable(int fd)
{
	return(waitread(fd, 0));
}

/*
** waitread()
**
** Wait for data on a file descriptor for a little while.
**
** Input: file descriptor to watch
**	  how long to wait, in seconds, before returning
** Output: 1 if data was available
**	   0 if the timer expired or a signal occurred.
*/
int waitread(int fd, int time)
{
	fd_set readbits, other;
	struct timeval timer;
	int ret;

	timerclear(&timer);
	timer.tv_sec = time;
	FD_ZERO(&readbits);
	FD_ZERO(&other);
	FD_SET(fd, &readbits);

	ret = select(fd+1, &readbits, &other, &other, &timer);
	if (FD_ISSET(fd, &readbits))
		return 1;
	return 0;
}

