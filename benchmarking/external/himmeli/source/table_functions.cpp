/* file: table_functions.cpp
  Copyright (C) 2008 Ville-Petteri Makinen

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
  Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  CITATION (if not provided by software website)
    Makinen V-P, software name, URL:http://www.iki.fi/~vpmakine/

  CONTACT (if not provided by citation)
    Ville-Petteri Makinen
 
    Folkhalsan Research Center
    Biomedicum Helsinki P.O.Box 63
    Haartmaninkatu 8 00014, Helsinki, Finland
    Tel: +358 9 191 25462
    Fax: +358 9 191 25452

    WWW:   http://www.iki.fi/~vpmakine
*/

#include "table.h"

static unsigned int trim_line(char*);

/*
 *  Read the contents of the stream and append a linebreak
 *  and terminating null character to the buffer.
 */
char*
read_buffer(FILE* input, unsigned int iobufcap) {
  unsigned int size;
  unsigned int n_read = 0;
  unsigned int cap = 0;
  char* buf = NULL;
  if(iobufcap < BUFSIZ) iobufcap = BUFSIZ;

  n_read = 0;
  cap = iobufcap;
  buf = (char*)malloc(iobufcap + 2);
  while((size = fread((buf + n_read), 1, iobufcap, input)) > 0) {
    n_read += size;
    if((n_read + iobufcap) > BUF_LIMIT) {      
      fprintf(stderr, "WARNING! %s: Buffer limit exceeded.\n", __FILE__);
      break;
    }
    if((n_read + iobufcap) > cap) {      
      cap = (3*n_read/2 + iobufcap + 2);
      buf = (char*)realloc(buf, cap);
    }
  }
  buf[n_read] = '\n';
  buf[n_read+1] = '\0';  

  return buf;
}


/*
 * Return the trimmed buffer and number of lines.
 * Buffer must be null terminated.
 */
char*
trim_buffer(char* buf, int* num) {
  unsigned int len = 0;
  unsigned int n_read = 0;
  unsigned int n_bytes = 0;
  char* ptr = buf;

  /* Read one line at a time. */
  *num = 0;
  while((ptr = strpbrk(ptr, "\r\n")) != NULL) {
    *ptr = '\0';
    ptr++;    

    if((len = trim_line(buf + n_read)) < 1) {
      n_read = (unsigned int)(ptr - buf);
      continue;
    }
    memmove((buf + n_bytes), (buf + n_read), (len + 1));

    (*num)++;
    n_bytes += (len + 1);
    n_read = (unsigned int)(ptr - buf);
  }

  return (char*)realloc(buf, n_bytes);
}

/*
 * Return the tidy buffer and number of lines. Replace all sequences 
 * of linebreaks (newline or carriage return) with a single null
 * character. Empty lines are ignored. Buffer must be null terminated.
 */
char*
tidy_buffer(char* buf, int* num, char sep) {
  unsigned int i;
  unsigned int len = 0;
  unsigned int n_read = 0;
  unsigned int n_bytes = 0;
  char* ptr = buf;

  /* Read one line at a time. */
  *num = 0;
  while((ptr = strpbrk(ptr, "\r\n")) != NULL) {
    while((*ptr == '\n') || (*ptr == '\r')) {
      *ptr = '\0';
      ptr++;
    }
    len = strlen(buf + n_read);

    /* Accept only lines with at least one non-separator. */
    for(i = n_read; buf[i] != '\0'; i++) {
      if(buf[i] == sep) continue;
      memmove((buf + n_bytes), (buf + n_read), (len + 1));
      n_bytes += (len + 1);
      *num += 1;
      break;
    }
    n_read = (unsigned int)(ptr - buf);
  }

  return (char*)realloc(buf, n_bytes);
}

/*
 *  Input buffer must be trimmed. 
 *  Return the maximum number of tokens per line.
 */
unsigned int
finish_buffer(char* buf, row_t* rows, unsigned int n_rows, char sep) {
  unsigned int i, n;
  unsigned int n_cols = 0;
  unsigned int size;
  char* line;
  char* ptr;

  /* Find pointer and count the num of tokens for each line. */
  line = buf;
  for(i = 0; i < n_rows; i++) {
    size = (strlen(line) + 1);

    /* Replace separator with '\0'. */
    ptr = line;
    for(n = 1; (ptr = strchr(ptr, sep)) != NULL; n++) {
      *ptr = '\0'; 
      ptr++;
    }
    if(n > n_cols) n_cols = n;

    rows[i].rank = i;
    rows[i].size = size;
    rows[i].n_keys = 0;
    rows[i].n_fields = n;
    rows[i].key_0 = NULL;
    rows[i].key_1 = NULL;
    rows[i].key_2 = NULL;
    rows[i].key_3 = NULL;
    rows[i].line = line;
    rows[i].keycols = NULL;
    line += size;
  }

  return n_cols;
}

/* 
 * Return the new length (not including the terminating 
 * null character).  Replace all sequences of white-space characters
 * with a single '\t' ('\0' if end of line). Line must be a null
 * terminated string.
 */
static unsigned int
trim_line(char* line) {
  unsigned int i;
  unsigned int len;
  char* ptr;

  if((len = strlen(line)) < 1)
    return len;

  /* Remove unnecessary white-space characters. */
  line[len] = ' ';
  ptr = line;
  for(i = 0; i <= len; i++)
    if(!isspace((int)(line[i]))) {
      if((*ptr == '\0') && (ptr > line)) ptr++;
      *ptr = line[i];
      ptr++;
    }
    else
      *ptr = '\0';

  /* Replace null separators with tabulator. */
  len = (int)(ptr - line);
  for(i = 0; i < len; i++)
    if(line[i] == '\0') line[i] = '\t';
  line[len] = '\0';
  
  return len;
}

/*
 *
 */
int 
rowcmp(const void* ptr1, const void* ptr2) {
  bool flag1 = 0;
  bool flag2 = 0;
  int i, k;
  int n_keys;
  float f1;
  float f2;
  string s1;
  string s2;
  char* line1;
  char* line2;
  int* keycols1;
  int* keycols2;
  row_t* r1 = (row_t*)ptr1;
  row_t* r2 = (row_t*)ptr2;

  n_keys = r1->n_keys;
  if(n_keys > r2->n_keys) n_keys = r2->n_keys;
  if(n_keys < 1) return (r1->rank - r2->rank);

  /* Compare key shortcuts. */
  flag1 = (r1->key_0 == NULL);
  flag2 = (r2->key_0 == NULL);
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1;
  f1 = atof(r1->key_0);
  f2 = atof(r2->key_0);
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  s1 = string(r1->key_0);
  s2 = string(r2->key_0);
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  if(n_keys < 2) return 0;

  flag1 = (r1->key_1 == NULL);
  flag2 = (r2->key_1 == NULL);
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1; 
  f1 = atof(r1->key_1);
  f2 = atof(r2->key_1);
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  s1 = string(r1->key_1);
  s2 = string(r2->key_1);
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  if(n_keys < 3) return 0;

  flag1 = (r1->key_2 == NULL);
  flag2 = (r2->key_2 == NULL);
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1; 
  f1 = atof(r1->key_2);
  f2 = atof(r2->key_2);
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  s1 = string(r1->key_2);
  s2 = string(r2->key_2);
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  if(n_keys < 4) return 0;
 
  flag1 = (r1->key_3 == NULL);
  flag2 = (r2->key_3 == NULL);
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1; 
  f1 = atof(r1->key_3);
  f2 = atof(r2->key_3);
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  s1 = string(r1->key_3);
  s2 = string(r2->key_3);
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  if(n_keys < 5) return 0;

  /* Check that keycols are consistent. */
  keycols1 = r1->keycols;
  keycols2 = r2->keycols;
  for(i = 0; i < n_keys; i++) {
    if(keycols1[i] != keycols2[i]) {
      fprintf(stderr, "ERROR! %s at line %d: Invalid program state.\n", 
	      __FILE__, __LINE__);
      exit(1);
    }
  }

  /* Compare all relevant fields. */
  k = 0;
  line1 = r1->line;
  line2 = r2->line;
  while((k < r1->n_fields) || (k < r2->n_fields)) {
    for(i = 4; i < n_keys; i++) {
      if(keycols1[i] != k) continue;
      f1 = atof(line1);
      f2 = atof(line2);
      if(f1 < f2) return -1;
      if(f1 > f2) return 1;
      s1 = string(line1);
      s2 = string(line2);
      if(s1 < s2) return -1;
      if(s1 > s2) return 1;
    }
    if(k >= r1->n_fields) line1 = &EmptyChar;
    else line1 += (strlen(line1) + 1);
    if(k >= r2->n_fields) line2 = &EmptyChar;
    else line2 += (strlen(line2) + 1);
    k++;
  }
  
  return 0;
}
