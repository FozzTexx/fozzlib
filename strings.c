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
 *
 * $Id: strings.c,v 1.10 2007/04/30 16:15:07 fozztexx Exp $
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include "strings.h"

#if !defined(__GNUC__) || defined(NeXT)
char *strdup(const char *str)
{
  char *p;
  int len;


  if (!str)
    return NULL;
  
  len = strlen(str);
  p = malloc(len + 1);
  strncpy(p, str, len);
  p[len] = 0;

  return p;
}
#endif

#if !defined(__USE_GNU) && !defined(__USE_XOPEN2K8)
char *strndup(const char *str, size_t len) /* DOES append null at end */
{
  char *p;


  if (!str)
    return NULL;
  
  p = malloc (len + 1);
  strncpy (p, str, len);
  p[len] = 0;

  return p;
}
#endif

#ifndef __STDC__
char *strpos(const char *str1, const char *str2)
{
  int i, j = strlen (str2);


  for (i = 0; str1[i]; i++)
    if (!strncmp (&str1[i], str2, j))
      break;

  if (!str1[i])
    return NULL;

  return &str1[i];
}
#endif

const char *strnpos(const char *str1, int s1len, const char *str2)
{
  int i, j = strlen (str2);


  for (i = 0; i < s1len; i++)
    if (!strncmp (&str1[i], str2, j))
      break;

  if (i >= s1len)
    return NULL;

  return &str1[i];
}

char *strlcat(char **str, const char *str2)
{
  char *p = *str;

  
  if (p)
    p = realloc(p, strlen(p) + strlen(str2) + 1);
  else {
    p = malloc(strlen(str2) + 1);
    *p = 0;
  }

  strcat(p, str2);
  *str = p;

  return p;
}

char *strlcpy(char **str, const char *str2)
{
  char *p = *str;

  
  if (p)
    p = realloc(p, strlen(str2) + 1);
  else {
    p = malloc(strlen(str2) + 1);
    *p = 0;
  }

  strcpy(p, str2);
  *str = p;

  return p;
}

char *strlncat(char **str, const char *str2, int len)
{
  char *p = *str;
  int l2;

  
  if (p) {
    l2 = strlen(p);
    p = realloc(p, strlen(p) + len + 1);
  }
  else {
    l2 = 0;
    p = malloc(len + 1);
    *p = 0;
  }

  strncat(p, str2, len);
  p[l2 + len] = 0;
  *str = p;

  return p;
}

char *strlncpy(char **str, const char *str2, int len)
{
  char *p = *str;

  
  if (p)
    p = realloc(p, len + 1);
  else {
    p = malloc(len + 1);
    *p = 0;
  }

  strncpy(p, str2, len);
  p[len] = 0;
  *str = p;

  return p;
}

int skipwhite(const char *str)
{
  int i;

  
  for (i = 0; str[i] && (str[i] == ' ' || str[i] == '\t'); i++);

  return i;
}

int break_line(const char *line, char ***args)
{
  int i, j, k;
  int is;
  int na;
  char **s = *args;


  for (na = is = i = 0; line[i];) {
    while (line[i] && isspace(line[i]))
      i++;
    if (line[i]) {
      na++;
      is = 0;
      while (line[i] && (!isspace(line[i]) || is)) {
	if ((is && line[i] == is) || (!is && (line[i] == '\'' || line[i] == '"')))
	  is = line[i] == is ? 0 : line[i];
	i++;
      }
    }
  }

  if (!s) {
    s = (char **) malloc((na + 1) * sizeof(char *));
    memset(s, 0, (na + 1) * sizeof(char *));
  }
  else {
    for (i = 0; s[i]; i++);
    if (i < na) {
      s = (char **) realloc(s, (na + 1) * sizeof(char *));
      for (j = i; j <= na; j++)
	s[j] = NULL;
    }
  }

  for (k = is = i = 0; line[i];) {
    while (line[i] && isspace(line[i]))
      i++;
    if (line[i]) {
      is = 0;
      j = i;
      while (line[i] && (!isspace(line[i]) || is)) {
	if ((is && line[i] == is) || (!is && (line[i] == '\'' || line[i] == '"')))
	  is = line[i] == is ? 0 : line[i];
	i++;
      }
      if (!s[k])
	s[k] = (char *) malloc(i - j + 1);
      else if (strlen(s[k]) < i - j)
	s[k] = realloc(s[k], i - j + 1);
      strncpy(s[k], &line[j], i - j);
      s[k][i - j] = 0;
      k++;
    }
  }

  *args = s;
  return na;
}

void fix_chars(char *str, int eatspace, char **boringwords)
{
  int i, was;


  if (eatspace) { /* take out spaces and slashes too */
    for (i = 0; str[i]; i++)
      if (!isalnum(str[i]) && !strchr("-._", str[i]))
	str[i] = '_';
  }
  else {
    for (i = 0; str[i]; i++)
      if (!isalnum(str[i]) && !isspace(str[i]) && !strchr("-._/", str[i]))
	str[i] = '_';
  }

  do {
    was = strlen(str);
    while (*str && !isalnum(*str))
      strcpy(str, str+1);
    while (*str && !isalnum(str[strlen(str) - 1]))
      str[strlen(str) - 1] = 0;

    for (i = 0; str[i]; i++)
      while (str[i] == '_' && str[i+1] == '_')
	strcpy(&str[i], &str[i+1]);
    
    for (i = 0; boringwords && boringwords[i]; i++) {
      while (!strncasecmp(str, boringwords[i], strlen(boringwords[i])) &&
	     (str[strlen(boringwords[i])] == '_' || !str[strlen(boringwords[i])]))
	strcpy(str, &str[strlen(boringwords[i])]);
      while (!strcasecmp(&str[strlen(str) - strlen(boringwords[i])], boringwords[i]) &&
	     str[strlen(str) - strlen(boringwords[i]) - 1] == '_')
	str[strlen(str) - strlen(boringwords[i])] = 0;
    }
  } while (strlen(str) != was);

  return;
}

int slprintf(char **str, size_t *size, const char *format, ...)
{
  int len = *size;
  int i;
  char *p = *str;
  va_list ap;
#ifdef NeXT
  FILE *file;
#endif


  if (!p) {
    len = 256;
    p = malloc(len);
  }

  if (!len) {
    len = 256;
    p = realloc(p, len);
  }

#ifdef NeXT
  va_start(ap, format);
  file = fopen("/dev/null", "w");
  if ((i = vfprintf(file, format, ap)) > len-1) {
    len = i+1;
    p = realloc(p, len);
  }
  fclose(file);
  vsprintf(p, format, ap);
  va_end(ap);
#else
  for (;;) {
    va_start(ap, format);
    i = vsnprintf(p, len, format, ap);
    va_end(ap);

    if (i > -1 && i < len)
      break;

    if (i > -1)    /* glibc 2.1 */
      len = i+1;
    else           /* glibc 2.0 */
      len *= 2;

    p = realloc(p, len);
  }
#endif

  *size = len;
  *str = p;

  return i;
}
