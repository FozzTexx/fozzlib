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

#include <sys/ioctl.h>

typedef struct shellinfo {
  int in, out, err;
} shellinfo;

typedef struct ttyinfo {
#if !defined(linux) && !defined(__APPLE__)
  struct sgttyb setp;
  struct tchars setc;
  struct ltchars sltc;
#endif
  int lset;
  int setd;
  struct winsize win;
} ttyinfo;
  
const char *getpty(int *master, int *slave);
void get_ttyinfo(int fd, ttyinfo *info);
void set_ttyinfo(int fd, ttyinfo *info);
int fork_dup(shellinfo *sinfo, ttyinfo *tinfo);
int fork_shell(const char *path, const char *args, shellinfo *sinfo, ttyinfo *tinfo);
int fork_shell_wpty(const char *path, const char *args);
#ifdef NeXT
int putenv(const char *s);
#endif
