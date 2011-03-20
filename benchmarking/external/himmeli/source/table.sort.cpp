/* file: table.sort.cpp
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

/*
 * Sort using every column. 
 */
void
Table::sort() {
  int i;
  vector<int> cols;
  TableBuffer* tb = (TableBuffer*)buffer;
  for(i = 0; i < tb->n_cols; i++)
    cols.push_back(i);
  this->sort(cols);
};

/*
 * Column indices start from 0 as in C-style.
 */
void
Table::sort(const vector<int>& cols) {
  int i, j, k;
  int n = cols.size();
  int cap = (n + 5);
  char* ptr;
  TableBuffer* tb = (TableBuffer*)buffer;
  int* keycols = tb->keycols;
  row_t* rows = tb->rows;

  /* Reset keycols and rows. */
  tb->keycols = (int*)realloc(tb->keycols, cap*sizeof(int));
  keycols = tb->keycols;
  for(i = 0; i < cap; i++)
    keycols[i] = -1;
  for(i = 0; i < tb->n_rows; i++) {
    rows[i].n_keys = n;
    rows[i].key_0 = NULL;
    rows[i].key_1 = NULL;
    rows[i].key_2 = NULL;
    rows[i].key_3 = NULL;
    rows[i].keycols = keycols;
  }

  /* Copy valid column indices. */
  for(i = 0, k = 0; i < n; i++) {
    if(cols[i] < 0) continue;
    if(cols[i] >= tb->n_cols) continue;
    keycols[k++] = cols[i];
  }

  /* Update keys. */
  for(i = 0; i < tb->n_rows; i++) {
    n = rows[i].n_fields;
    ptr = rows[i].line;
    for(j = 0; j < n; j++) {
      if(j == keycols[0]) rows[i].key_0 = ptr;
      if(j == keycols[1]) rows[i].key_1 = ptr;
      if(j == keycols[2]) rows[i].key_2 = ptr;
      if(j == keycols[3]) rows[i].key_3 = ptr;
      ptr += (strlen(ptr) + 1);
    }
  }

  /* Sort rows. */
  qsort(rows, tb->n_rows, sizeof(row_t), &rowcmp);
  for(i = 0; i < tb->n_rows; i++) {
    k = rows[i].rank;
    tb->rank2locus[k] = i;
  }
}
