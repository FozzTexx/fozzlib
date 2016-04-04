/* Copyright 1991-2016 by Chris Osborn <fozztexx@fozztexx.com>
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

#include "database.h"
#include "strings.h"
#include "files.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BUFSIZE 256
#define MAXLINELENGTH 76

int DBPutChar(FILE *file, unsigned int c)
{
  if (c == '\n') {
    fprintf(file, "\\n");
    return 2;
  }
  else if (c == '\t') {
    fprintf(file, "\\t");
    return 2;
  }
  else if (c == '\\') {
    fprintf(file, "\\\\");
    return 2;
  }
  else if (c == '-') {
    fprintf(file, "\\-");
    return 2;
  }
  else if (c == '*') {
    fprintf(file, "\\*");
    return 2;
  }
  if (c > 126) {
    fprintf(file, "\\%d%d%d", (c >> 6) & 7, (c >> 3) & 7, c & 7);
    return 4;
  }

  fputc(c, file);
  return 1;
}
  
int DBNPutString(FILE *file, const char *string, int len)
{
  int i, j;
  const unsigned char *p = (unsigned char *) string;


  for (i = j = 0; j < len; p++, j++)
    i += DBPutChar(file, *p);

  return i;
}

int DBPutString(FILE *file, const char *string)
{
  int i;
  const unsigned char *p = (unsigned char *) string;


  for (i = 0; *p; p++)
    i += DBPutChar(file, *p);

  return i;
}
  
int DBPutText(FILE *file, const char *string)
{
  int i, j, k, l, m;
  const char *p, *q, *s;

  
  for (i = k = l = m = 0, j = strlen(string), p = q = string; i < j; k++, q++) {
    if (k >= MAXLINELENGTH || q-string == j || *q == '\n') {
      if (*q != '\n' && k >= MAXLINELENGTH)
	q--;
      s = q;
      while (*s && !isspace(*s) && s >= p)
	s--;
      if (s <= p)
	s = q;
      while (*s && !isspace(*s))
	s++;
      while (*s && isspace(*s) && *s != '\n')
	s++;
      while (*p == '\n') {
	fputc('\n', file);
	l++;
	p++;
	i++;
	if (!m)
	  fputc('\n', file), l++;
	m++;
      }
      if ((s - p) > 0) {
	l += DBNPutString(file, (char *) p, s - p);
	if (*s && *s != '\n')
	  fputc('\n', file), l++;
	m = 0;
	i += s - p;
	p = s;
      }
      q = p;
      k = 0;
    }
  }

  if (!m)
    fputc('\n', file), l++;
  
  return l;
}

int DBWriteRecordTitle(FILE *file, const char *string)
{
  int i;


  i = fwrite(SEP_RECORD, 1, SEP_RECORD_LEN, file);
  fputc(' ', file);
  i++;
  i += DBPutString(file, string);
  fputc(' ', file);
  i++;
  i += fwrite(SEP_RECORD, 1, SEP_RECORD_LEN, file);
  fputc('\n', file);
  i++;

  return i;
}

int DBWriteSectionTitle(FILE *file, const char *string)
{
  int i;


  i = fwrite(SEP_SECTION, 1, SEP_SECTION_LEN, file);
  fputc(' ', file);
  i++;
  i += DBPutString(file, string);
  fputc(' ', file);
  i++;
  i += fwrite(SEP_SECTION, 1, SEP_SECTION_LEN, file);
  fputc('\n', file);
  i++;

  return i;
}
  
int DBWriteString(FILE *file, const char *title, const char *value)
{
  int i;

  
  i = DBPutString(file, title);
  i += fwrite(": ", 1, 2, file);
  i += DBPutString(file, value);
  fputc('\n', file);
  i++;

  return i;
}

int DBWriteText(FILE *file, const char *title, const char *value)
{
  int i;


  i = DBWriteSectionTitle(file, title);
  i += DBPutText(file, value);
  if (value[strlen(value)-1] != '\n')
    fputc('\n', file), i++;
  
  return i;
}

void DBDecodeString(char *string)
{
  for (; *string; string++) {
    if (*string == '\\') {
      switch (*(string+1)) {
      case 'n':
	strcpy(string, string+1);
	*string = '\n';
	break;

      case 't':
	strcpy(string, string+1);
	*string = '\t';
	break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
	*string = ((*(string+1) - '0') << 6) | ((*(string+2) - '0') << 3) | (*(string+3) - '0');
	strcpy(string+1, string+4);
	break;
	
      default:
	strcpy(string, string+1);
	break;
      }
    }
  }

  return;
}

int DBGetString(FILE *file, char **string, size_t *len)
{
  char *p;
  int i;
  
  
  if (flgets(string, len, file)) {
    p = *string;
    if (!strncmp(p, SEP_RECORD, SEP_RECORD_LEN)) {
      fseek(file, -strlen(p), SEEK_CUR);
      return EOF-1;
    }
    if (!strncmp(p, SEP_SECTION, SEP_SECTION_LEN)) {
      fseek(file, -strlen(p), SEEK_CUR);
      return EOF-2;
    }

    i = strlen(p);
    DBDecodeString(p);
    return i;
  }

  return EOF;
}

int DBGetText(FILE *file, char **string, size_t *len)
{
  int i = 0;
  int err;
  char *buf = NULL;
  char *p, *q;
  

  if (*string)
    **string = 0;
  
  while ((err = DBGetString(file, &buf, len)) >= 0) {
    i += err;
    if (*buf == '\n') {
      if (*string && **string) {
	p = *string;
	for (q = p + strlen(p) - 1; isspace(*q) && *q != '\n'; q--);
	*(q+1) = 0;
      }
      strlcat(string, buf);
    }
    else {
      if (*string && **string) {
	p = *string;
	for (q = p + strlen(p) - 1; isspace(*q) && *q != '\n'; q--);
	*(q+1) = 0;
	if (!isspace(*q))
	  strlcat(string, " ");
      }
      strlncat(string, buf, strlen(buf)-1);
    }
  }
  
  p = *string;
  for (q = p + strlen(p) - 1; isspace(*q); q--);
  *(q+1) = 0;
  strlcat(string, "\n");

  if (buf)
    free(buf);
  
  return i;
}

int DBReadRecordTitle(FILE *file, char **string, size_t *len)
{
  char *p, *q;
  int i;
  
  
  while (flgets(string, len, file))
    if (!strncmp(*string, SEP_RECORD, SEP_RECORD_LEN))
      break;
  if (feof(file))
    return EOF;

  p = *string;
  i = strlen(p);
  
  for (q = p + SEP_RECORD_LEN; *q && isspace(*q); q++);
  strcpy(p, q);
  while (isspace(p[strlen(p)-1]))
    p[strlen(p)-1] = 0;	 
  for (q = p + strlen(p) - (2 + SEP_RECORD_LEN); isspace(*q); q--);
  *(q+1) = 0;

  DBDecodeString(p);
  
  return i;
}

int DBReadSectionTitle(FILE *file, char **string, size_t *len)
{
  char *p, *q;
  int i;
  
  
  while (flgets(string, len, file)) {
    if (!strncmp(*string, SEP_RECORD, SEP_RECORD_LEN)) {
      fseek(file, -strlen(*string), SEEK_CUR);
      return EOF-1;
    }

    if (!strncmp(*string, SEP_SECTION, SEP_SECTION_LEN))
      break;
  }
  if (feof(file))
    return EOF;

  p = *string;
  i = strlen(p);
  
  for (q = p + SEP_RECORD_LEN; *q && isspace(*q); q++);
  strcpy(p, q);
  while (isspace(p[strlen(p)-1]))
    p[strlen(p)-1] = 0;	 
  for (q = p + strlen(p) - (2 + SEP_RECORD_LEN); isspace(*q); q--);
  *(q+1) = 0;

  DBDecodeString(p);
  
  return i;
}

int DBReadString(FILE *file, char **title, char **value)
{
  char *buf = NULL;
  size_t len = 0;
  char *p, *q;
  int err;
  

  do {
    err = DBGetString(file, &buf, &len);
  } while (err >= 0 && !strchr(buf, ':'));
  if (err < 0) {
    if (buf)
      free(buf);
    return err;
  }

  q = strchr(buf, ':');
  strlncpy(title, buf, q-buf);
  q++;
  while (*q && isspace(*q))
    q++;

  strlcpy(value, q);
  p = *value;
  for (q = p + strlen(p) - 1; isspace(*q); q--);
  *(q+1) = 0;

  if (buf)
    free(buf);
  
  return strlen(*title) + strlen(*value);
}
