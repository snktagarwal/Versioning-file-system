/* file: table.cpp
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

#define IOBUFCAP 524288

char EmptyChar = '\0';
static unsigned int ID = 1;
extern char* read_buffer(FILE*, unsigned int);

/*
 *
 */
Table::Table() {
  TableBuffer* tb = new TableBuffer(ID++);
  buffer = tb;
}

/*
 *
 */
Table::Table(const Table& t) {
  Table* pt = (Table*)(&t);
  buffer = pt->buffer;
  pt->buffer = new TableBuffer(ID++);
}

/*
 *
 */
Table::Table(const string& fname, const char sep) {

  /* Default state. */
  TableBuffer* tb = new TableBuffer(ID++);
  tb->data_source = (char*)malloc(fname.length() + 1);
  strcpy(tb->data_source, fname.c_str());
  buffer = tb;

  /* Import data. */
  FILE* input = fopen(fname.c_str(), "r");
  if(input == NULL) return;

  /* Prepare buffer. */
  tb->buf = read_buffer(input, IOBUFCAP);
  tb->prepare(sep);
  fclose(input);
}

/*
 *
 */
Table::~Table() {
  TableBuffer* tb = (TableBuffer*)buffer;
  if(tb != NULL) delete tb;
  buffer = NULL;
};

/*
 *
 */
int
Table::strcmp(const string& s1, const string& s2) {
  bool flag1 = (s1 == "");
  bool flag2 = (s2 == "");
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1;
  float f1 = atof(s1.c_str());
  float f2 = atof(s2.c_str());
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  return 0;
}

/*
 *
 */
string
Table::version() {
  string s = "";
  s += VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
