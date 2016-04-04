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

#include <fozzlib/shell.h>
#include <fozzlib/strings.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef linux
#include <unistd.h>
#else
#include <libc.h>
#endif

#define PTY_TEMPLATE "/dev/pty??"
#define PTY_LENGTH 11

static char device[PTY_LENGTH];

const char *getpty(int *master, int *slave)
{
  char *block, *num;
  char *blockLoc;
  char *numLoc;
  char *msLoc;


  strcpy(device, PTY_TEMPLATE);
  blockLoc = &device[strlen("/dev/pty")];
  numLoc = &device[strlen("/dev/pty?")];
  msLoc = &device[strlen("/dev/")];
  for (block = "pqrs"; *block; block++) {
    *blockLoc = *block;
    for (num = "0123456789abcdef"; *num; num++) {
      *msLoc = 'p';
      *numLoc = *num;
      if ((*master = open(device, O_RDWR)) >= 0) {
	*msLoc = 't';
	if ((*slave = open(device, O_RDWR)) >= 0) {
	  return device;
	}
	close(*master);
      }
    }
  }
  
  return NULL;
}

void get_ttyinfo(int fd, ttyinfo *info)
{
#if !defined(linux) && !defined(__APPLE__)
  ioctl(fd, TIOCGETP, &info->setp);
  ioctl(fd, TIOCGETC, &info->setc);
  ioctl(fd, TIOCLGET, &info->lset);
  ioctl(fd, TIOCGLTC, &info->sltc);
#endif
  ioctl(fd, TIOCGETD, &info->setd);
  ioctl(fd, TIOCGWINSZ, &info->win);

  return;
}

void set_ttyinfo(int fd, ttyinfo *info)
{
#if !defined(linux) && !defined(__APPLE__)
  ioctl(fd, TIOCSETP, &info->setp);
  ioctl(fd, TIOCSETC, &info->setc);
  ioctl(fd, TIOCLSET, &info->lset);
  ioctl(fd, TIOCSLTC, &info->sltc);
#endif
  ioctl(fd, TIOCSETD, &info->setd);
  ioctl(fd, TIOCSWINSZ, &info->win);

  return;
}

int fork_dup(shellinfo *sinfo, ttyinfo *tinfo)
{
  int cpid;
  int tty;


  if ((cpid = fork()) < 0)
    return -1;

  if (cpid) {
    close(sinfo->in);
    close(sinfo->out);
    close(sinfo->err);
    return cpid;
  }

  /* Child */
  if ((tty = open("/dev/tty", O_RDWR)) >= 0) {
    ioctl(tty, TIOCNOTTY, 0);
    close(tty);
  }
  
  dup2(sinfo->in, 0);
  dup2(sinfo->out, 1);
  dup2(sinfo->err, 2);
  close(sinfo->in);
  close(sinfo->out);
  close(sinfo->err);
  
  cpid = getpid();
  ioctl(0, TIOCSPGRP, &cpid);
#ifdef linux
  setpgid(0, cpid);
#else
  setpgrp(0, cpid);
#endif
  set_ttyinfo(0, tinfo);
  
  return 0;
}
    
int fork_shell(const char *path, const char *args, shellinfo *sinfo, ttyinfo *tinfo)
{
  char **argv = NULL;
  int cpid;
  

  if (!(cpid = fork_dup(sinfo, tinfo))) { /* Child */
    break_line(args, &argv);
    execv(path, argv);
  }

  return cpid;
}

int fork_shell_wpty(const char *path, const char *args)
{
  int master, slave;
  shellinfo sinfo;
  ttyinfo tinfo;
  

  if (!getpty(&master, &slave))
    return -1;

  sinfo.in = sinfo.out = sinfo.err = slave;
  get_ttyinfo(0, &tinfo);
  fcntl(master, F_SETFD, 1);
  
  if (fork_shell(path, args, &sinfo, &tinfo) >= 0)
    return master;

  return -1;
}

#ifdef NeXT
/* Define putenv for machines the don't have it in the
 * standard library.
 */
int putenv(const char *s)
{
  int nlen;
  const char *cptr;
  char **nenv, **eptr;
  extern char **environ;

  
  /* First see if there is an existing 'name=value' with the
   * same name as s.
   */
  for (cptr = s; *cptr != '=' && *cptr != '\0'; cptr++)
    ;
  if (*cptr == '=' && cptr > s) {
    nlen = cptr - s + 1;
    for (eptr = environ; *eptr != NULL; eptr++) {
      if (strncmp(*eptr, s, nlen) == 0) {
	*eptr = (char *) s;
	return 0;
      }
    }
  }

  /* New name, so must change environ.
   */
  for (eptr = environ; *eptr != NULL; eptr++)
    ;
  nenv = (char **) malloc((eptr - environ + 2) * sizeof(char *));
  if (nenv == NULL)
    return -1;
  eptr = environ;
  environ = nenv;
  while ((*nenv = *eptr) != NULL)
    nenv++, eptr++;
  *nenv = (char *) s;
  nenv[1] = NULL;
  
  return 0;
}

#endif /* NeXT */
