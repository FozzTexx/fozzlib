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

#include <stdio.h>

typedef struct AtomicFILE {
  FILE *file;
  char *filename;
  char *tempname;
} AtomicFILE;

extern int loaddir(char *dirname, char ***dir);
extern char *change_extension(char *name, char *from, char *to, int insert);
extern char *make_unique(char *filename);
extern FILE *fcaseopen(char *filename, const char *mode);
extern char *flgets(char **str, size_t *bufsize, FILE *file);
extern AtomicFILE *fopenAtomic(const char *filename, const char *mode);
extern int fcloseAtomic(AtomicFILE *aFile);
extern int mkdirs(const char *path, int mode);