/* file: tablebuffer.prepare.cpp
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

extern char*        trim_buffer(char*, int*);
extern char*        tidy_buffer(char*, int*, char);
extern unsigned int finish_buffer(char*, row_t*, unsigned int, char);

/*
 *
 */
void
TableBuffer::prepare(const char sep) {
  int i;

  /* Standardize separator. */
  if(sep != '\0') {
    separator = sep;
    buf = tidy_buffer(buf, &n_rows, separator);
  }
  else {
    buf = trim_buffer(buf, &n_rows);
    separator = '\t';
  }
  if(n_rows < 1) return;
  
  /* Create database. */
  rank2locus = (int*)malloc(n_rows*sizeof(int));
  rows = (row_t*)malloc(n_rows*sizeof(row_t));
  n_cols = finish_buffer(buf, rows, n_rows, separator);
  for(i = 0; i < n_rows; i++) {
    rank2locus[i] = i;
    buf_size += rows[i].size;
  }
}
