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

#ifndef _FZSTRINGS_H
#define _FZSTRINGS_H
#define GCC_VERSION (__GNUC__ * 10000 \
		     + __GNUC_MINOR__ * 100 \
		     + __GNUC_PATCHLEVEL__)

#if !defined(__GNUC__) || defined(NeXT)
extern char *strdup(const char *str);
#endif
#if GCC_VERSION >= 30101
#include <string.h>
#endif
extern char *strndup(const char *str, size_t len);
#ifdef __STDC__
#include <string.h>
#define strpos(x,y) strstr(x,y)
#else
extern char *strpos(const char *str1, const char *str2);
#endif

#ifdef __APPLE__
#define strlcat fz_strlcat
#define strlcpy fz_strlcpy
#endif

extern const char *strnpos(const char *str1, int s1len, const char *str2);
extern char *strlcat(char **str, const char *str2);
extern char *strlcpy(char **str, const char *str2);
extern char *strlncat(char **str, const char *str2, int len);
extern char *strlncpy(char **str, const char *str2, int len);
extern int skipwhite(const char *str);
extern int break_line(const char *line, char ***args);
extern void fix_chars(char *str, int eatspace, char **boringwords);
int slprintf(char **str, size_t *size, const char *format, ...);
#endif /* _FZSTRINGS_H */
