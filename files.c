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

#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "files.h"
#include "strings.h"
#include <errno.h>
#ifndef NeXT
#include <unistd.h>
#else
#include <libc.h>
#endif

#define BUFSIZE 256

int loaddir(char *dirname, char ***dir)
{
  DIR *dirp;
  struct direct *dent;
  int i, j;
  int na;
  char **s;


  if (!(dirp = opendir(dirname)))
    return -1;

  for (na = 0; readdir(dirp); na++);

  if (!*dir) {
    *dir = (char **) malloc((na + 1) * sizeof(char *));
    bzero(*dir, (na + 1) * sizeof(char *));
  }
  else {
    for (i = 0; (*dir)[i]; i++);
    if (i < na) {
      s = (char **) malloc((na + 1) * sizeof(char *));
      for (j = 0; j < i; j++)
	s[i] = (*dir)[i];
      free(*dir);
      *dir = s;
      bzero(*dir, (na + 1) * sizeof(char *));
    }
  }

  rewinddir(dirp);
  for (i = 0; (dent = readdir(dirp)); i++) {
#ifdef NeXT
    if (!(*dir)[i])
      (*dir)[i] = (char *) malloc(dent->d_namlen + 1);
    else if (strlen((*dir)[i]) < dent->d_namlen) {
      free((*dir)[i]);
      (*dir)[i] = (char *) malloc(dent->d_namlen + 1);
    }
    strncpy((*dir)[i], dent->d_name, dent->d_namlen);
    (*dir)[i][dent->d_namlen] = 0;
#else
    if (!(*dir)[i])
      (*dir)[i] = (char *) malloc(dent->d_reclen + 1);
    else if (strlen((*dir)[i]) < dent->d_reclen) {
      free((*dir)[i]);
      (*dir)[i] = (char *) malloc(dent->d_reclen + 1);
    }
    strncpy((*dir)[i], dent->d_name, dent->d_reclen);
    (*dir)[i][dent->d_reclen] = 0;
#endif
  }

  closedir(dirp);
  
  return na;
}

char *change_extension(char *name, char *from, char *to, int insert)
{
  static char buf[MAXPATHLEN + 1];
  char buf2[10];
  int i, j;


  strcpy(buf, name);

  i = strlen(buf);
  if (from && to) {
    j = strlen(to);
    if (!(strcmp(&buf[i - j], to)) && buf[i - j - 1] == '.')
      buf[i - j - 1] = 0;
    else {
      j = strlen(from);
      if (!(strcmp(&buf[i - j], from)) && buf[i - j - 1] == '.')
	buf[i - j - 1] = 0;
    }

    i = strlen(buf);
    if (insert) {
      if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
	buf[i - 4] = 0;

      sprintf(buf2, ".%03i", insert);
      strcat(buf, buf2);
    }

    strcat(buf, ".");
    strcat(buf, to);
  }
  else if (to) {
    j = strlen(to);
    if (!(strcmp(&buf[i - j], to)) && buf[i - j - 1] == '.')
      buf[i - j - 1] = 0;
    
    i = strlen(buf);
    if (insert) {
      if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
	buf[i - 4] = 0;

      sprintf(buf2, ".%03i", insert);
      strcat(buf, buf2);
    }

    strcat(buf, ".");
    strcat(buf, to);
  }
  else if (insert) {
    i = strlen(buf);
    if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
      buf[i - 4] = 0;

    sprintf(buf2, ".%03i", insert);
    strcat(buf, buf2);
  }
  
  return buf;
}

char *make_unique(char *filename)
{
  int nv;
  char *p;
  struct stat sbuf;

  
  p = filename;
  nv = 2;
  while (!stat(p, &sbuf)) {
    p = change_extension(filename, NULL, NULL, nv);
    nv++;
  }

  return p;
}

FILE *fcaseopen(char *filename, const char *mode)
{
  static char path[MAXPATHLEN + 1];
  static char name[MAXPATHLEN + 1];
  DIR *dirp;
  struct direct *ent;
  int len;
  char *p;


  strcpy(path, filename);
  if (!(p = rindex(path, '/'))) {
    strcpy(path, "./");
    strcpy(name, filename);
  }
  else {
    strcpy(name, p+1);
    *(p+1)=0;
  }
  
  if (!(dirp = opendir (path)))
    return NULL;

  for (ent = readdir(dirp); ent; ent = readdir(dirp)) {
#ifdef NeXT
    if (ent->d_namlen == strlen(name) && !strncasecmp(ent->d_name, name, ent->d_namlen)) {
      len = strlen(path);
      strncat(path, ent->d_name, ent->d_namlen);
      path[len + ent->d_namlen] = 0;
      closedir(dirp);
      break;
    }
#else
    if (ent->d_reclen == strlen(name) && !strncasecmp(ent->d_name, name, ent->d_reclen)) {
      len = strlen(path);
      strncat(path, ent->d_name, ent->d_reclen);
      path[len + ent->d_reclen] = 0;
      closedir(dirp);
      break;
    }
#endif
  }
  if (!ent)
    strcpy(path, filename);

  strcpy(filename, path);
  
  return fopen(path, mode);
}
  
char *flgets(char **str, size_t *bufsize, FILE *file)
{
  char *p = *str;
  int i = *bufsize;
  char *err;
  int j;


  if (!p)
    p = malloc(i = BUFSIZE);
  else if (i < BUFSIZE)
    p = realloc(p, i = BUFSIZE);

  j = 0;
  while ((err = fgets(p + j, i - j, file))) {
    j = strlen(p);
    if (p[j-1] == '\n')
      break;
    if (i - j < 2)
      p = realloc(p, i += BUFSIZE);
  }
  
  *str = p;
  *bufsize = i;

  return (!p || !j) && !err ? NULL : p;
}

AtomicFILE *fopenAtomic(const char *filename, const char *mode)
{
  AtomicFILE *aFile;
  FILE *file;
  char *fname;
  char buf[20];


  fname = strdup(filename);
  sprintf(buf, ".%i", getpid());
  strlcat(&fname, buf);
  strlcat(&fname, ".new");
  
  if (!(file = fopen(fname, mode))) {
    free(fname);
    return NULL;
  }

  aFile = malloc(sizeof(AtomicFILE));
  aFile->file = file;
  aFile->filename = strdup(filename);
  aFile->tempname = fname;

  return aFile;
}

int *enumerateDirectory(const char *directory, const char *filename, int *len)
{
  int *nums = NULL;
  int count = 0;
  int i, j, k;
  DIR *dir;
#ifndef NeXT
  struct dirent *dirp;
#else
  struct direct *dirp;
#endif
  

  if ((dir = opendir(directory))) {
    j = strlen(filename);
    while ((dirp = readdir(dir))) {
      k = strlen(dirp->d_name);
      if (k > j && dirp->d_name[j] == '.' && dirp->d_name[j+1] == '~' && dirp->d_name[k-1] == '~' &&
	  !strncmp(filename, dirp->d_name, j)) {
	i = atoi(&dirp->d_name[j+2]);

	if (nums)
	  nums = realloc(nums, sizeof(int) * (count+1));
	else
	  nums = malloc(sizeof(int));
	nums[count] = i;
	count++;		 
      }
    }

    for (i = 0; i < count-1; i++)
      for (j = i+1; j < count; j++)
	if (nums[j] < nums[i]) {
	  k = nums[j];
	  nums[j] = nums[i];
	  nums[i] = k;
	}
  }

  *len = count;
  return nums;
}

int fcloseAtomic(AtomicFILE *aFile)
{
  char *fname;
  int err = -1;
  struct stat st;
  char *p;
  char buf[MAXPATHLEN+1];
  int i;
  int *nums;
  int count;
  
  
  fclose(aFile->file);
  fname = strdup(aFile->filename);
  if (!(p = strrchr(fname, '/'))) {
    strcpy(fname, ".");
    p = aFile->filename;
  }
  else {
    *p = 0;
    p++;
  }

  nums = enumerateDirectory(fname, p, &count);
  if (count > 3) {
    for (i = 2; i < count-1; i++) {
      sprintf(buf, "%s.~%i~", aFile->filename, nums[i]);
      unlink(buf);
    }
  }

  if (count)
    i = nums[count-1];
  else
    i = 0;
  if (nums)
    free(nums);
  
  strcpy(fname, aFile->filename);
  strlcat(&fname, ".~");
  sprintf(buf, "%i~", i+1);
  strlcat(&fname, buf);
  unlink(fname); /* Make sure our backup is clear */
  
  if (stat(aFile->filename, &st) || (!link(aFile->filename, fname) && !unlink(aFile->filename))) {
    if (!link(aFile->tempname, aFile->filename))
      err = 0;
    unlink(aFile->tempname); /* It's not an error if we can't unlink the new one, the file we wanted to create still got created */
  }

  free(fname);
  free(aFile->filename);
  free(aFile->tempname);
  free(aFile);
  
  return err;
}

int mkdirs(const char *path, int mode)
{
  const char *p;
  char buf[MAXPATHLEN+1];
  struct stat st;


  p = path;
  while ((p = strchr(p+1, '/'))) {
    strncpy(buf, path, p-path);
    buf[p-path] = 0;
    if (!stat(buf, &st) && !(st.st_mode & S_IFDIR)) {
      errno = EEXIST;
      return -1;
    }
    else if (mkdir(buf, mode) && errno != EEXIST)
      return -1;
  }

  if (!stat(path, &st) && !(st.st_mode & S_IFDIR)) {
    errno = EEXIST;
    return -1;
  }
  else if (mkdir(path, mode) && errno != EEXIST)
    return -1;

  return 0;
}
