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

#include <stdio.h>

#define SEP_RECORD	"****"
#define SEP_RECORD_LEN	4
#define SEP_SECTION	"----"
#define SEP_SECTION_LEN	4

int DBPutString(FILE *file, const char *string);
int DBPutText(FILE *file, const char *string);
int DBWriteRecordTitle(FILE *file, const char *string);
int DBWriteSectionTitle(FILE *file, const char *string);
int DBWriteString(FILE *file, const char *title, const char *value);
int DBWriteText(FILE *file, const char *title, const char *value);

int DBGetString(FILE *file, char **string, size_t *len);
int DBGetText(FILE *file, char **string, size_t *len);
int DBReadRecordTitle(FILE *file, char **string, size_t *len);
int DBReadSectionTitle(FILE *file, char **string, size_t *len);
int DBReadString(FILE *file, char **title, char **value);
