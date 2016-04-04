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
** serversock()
**
** Creates an internet socket, binds it to an address, and prepares it for
** subsequent accept() calls by calling listen().
**
** Input: port number desired, or 0 for a random one
** Output: file descriptor of socket, or a negative error
*/
int serversock(int port);

/*
** portnum()
**
** Returns the internet port number for a socket.
**
** Input: file descriptor of socket
** Output: inet port number
*/
int portnum(int fd);

/*
** clientsock()
**
** Returns a connected client socket.
**
** Input: host name and port number to connect to
** Output: file descriptor of CONNECTED socket, or a negative error (-9999
**         if the hostname was bad).
*/
int clientsock(const char *host, int port);

/*
** readable()
**
** Poll a socket for pending input.  Returns immediately.  This is a front-end
** to waitread() below.
**
** Input: file descriptor to poll
** Output: 1 if data is available for reading
*/
int readable(int fd);

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
int waitread(int fd, int time);
