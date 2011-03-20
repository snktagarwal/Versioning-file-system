/* file: row.cpp
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

#define RANK_ind     0
#define N_FIELDS_ind 1
#define RSIZE_ind    2

static string get_data(const vector<char>&, int);
static void   get_info(int*, const vector<char>&);

/*
 *
 */
bool
Row::operator==(const Row& r) const {
  return (buffer == r.buffer);
}

/*
 *
 */
bool
Row::operator!=(const Row& r) const {
  return (buffer != r.buffer);
}

/*
 *
 */
string
Row::operator[](const int j) const {
  return get_data(buffer, j);
}

/*
 *
 */
unsigned int
Row::mem_size() const {
  int info[3];
  get_info(info, buffer);
  return info[RSIZE_ind];
}

/*
 *
 */
double
Row::number(const int j) const {
  return medusa::string2double(get_data(buffer, j));
}

/*
 *
 */
int
Row::rank() const {
  int info[3];
  get_info(info, buffer);
  return info[RANK_ind];
}

/*
 *
 */
unsigned int
Row::size() const {
  int info[3];
  get_info(info, buffer);
  return info[N_FIELDS_ind];
}

/*
 *
 */
double
Row::nan() {
  return medusa::nan();
}

/*
 *
 */
RowObject::RowObject() {}

/*
 *
 */
void
RowObject::fill(const void* ptr_row) {
  int k;
  unsigned int i;
  unsigned int pos = 0;
  int info[3];
  char* ptr;
  char* field;
  row_t* row = (row_t*)ptr_row;
  if(row == NULL) return;

  /* Copy row info. */
  info[RANK_ind] = row->rank;
  info[N_FIELDS_ind] = row->n_fields;
  info[RSIZE_ind] = row->size;
  ptr = (char*)info;
  for(i = 0; i < 3*sizeof(int); i++)
    buffer.push_back(ptr[i]);

  /* Copy field addresses. Note that the position in the
     block array is stored, not including the buffer offset. */
  field = row->line;
  for(k = 0; k < row->n_fields; k++) {
    pos = (unsigned int)(field - row->line);
    ptr = (char*)(&pos);
    for(i = 0; i < sizeof(int); i++)
      buffer.push_back(ptr[i]);
    field += (strlen(field) + 1);
  }

  /* Copy row data. */
  ptr = row->line;
  for(i = 0; i < row->size; i++)
    buffer.push_back(ptr[i]);
}

/*
 *
 */
static string
get_data(const vector<char>& v, int j) {
  unsigned int i, n;
  unsigned int pos = 0;
  int info[3] = {0, 0, 0};
  char* ptr;

  /* Extract row info. */
  n = 0;
  if(v.size() > 3*sizeof(int)) {
    ptr = (char*)info;
    for(i = 0; i < 3*sizeof(int); i++)
      ptr[i] = v[i];
    n = i;
  }

  /* Check if field is empty. */  
  if(n < 1) return "";
  if(j < 0) return "";
  if(j >= info[N_FIELDS_ind]) return "";

  /* Locate field block. */
  ptr = (char*)(&pos);
  n += j*sizeof(unsigned int);
  for(i = 0; i < sizeof(unsigned int); i++)
    ptr[i] = v[n+i];
  pos += 3*sizeof(int) + info[N_FIELDS_ind]*sizeof(unsigned int);

  /* Count block size. */
  i = pos;
  while(v[i] != '\0') i++;

  /* Create output string. */
  n = (i - pos);
  if(n > 0) {
    string s(n, '\0'); 
    for(i = 0; i < n; i++)   
      s[i] = v[pos+i];
    return s;
  }

  return "";
}

/*
 *
 */
static void
get_info(int* info, const vector<char>& v) {
  unsigned int i;
  char* ptr;
  info[RANK_ind] = -1;
  info[N_FIELDS_ind] = 0;
  info[RSIZE_ind] = 0;
  if(v.size() > 3*sizeof(int)) {
    ptr = (char*)info;
    for(i = 0; i < 3*sizeof(int); i++)
      ptr[i] = v[i];
  }
}
