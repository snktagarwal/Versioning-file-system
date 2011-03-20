/* file: table.h
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

#ifndef table_INCLUDED
#define table_INCLUDED

#include <iostream>
#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "medusa.h"
#include "tablet.h"

#define VERSION    "1.4.0"
#define BUF_LIMIT  67108864

using namespace std;
using namespace tablet;

typedef struct {
  int rank;
  int n_keys;
  int n_fields;
  unsigned int size;
  char* key_0;
  char* key_1;
  char* key_2;
  char* key_3;
  char* line;
  int* keycols;
} row_t;

class RowObject: public Row {
public:
  RowObject();
  void fill(const void*);
};

class TableBuffer {
public:
  char separator;
  int n_rows;
  int n_cols;
  unsigned int id_code;
  unsigned int buf_size;
  char* buf;
  int* keycols;
  int* rank2locus;
  row_t* rows;
  char* data_source;
public:
  TableBuffer(unsigned int id) {
    separator = '\0';
    id_code = id;
    n_rows = 0;
    n_cols = 0;
    buf_size = 0;
    buf = NULL;
    data_source = NULL;
    keycols = NULL;
    rank2locus = NULL;
    rows = NULL;
  };
  ~TableBuffer() {
    if(buf != NULL) free(buf);
    if(keycols != NULL) free(keycols);
    if(rank2locus != NULL) free(rank2locus);
    if(rows != NULL) free(rows);
    if(data_source != NULL) free(data_source);
  };
  void prepare(const char);
};

extern char EmptyChar;
extern int  rowcmp(const void*, const void*);

#endif /* table_INCLUDED */




