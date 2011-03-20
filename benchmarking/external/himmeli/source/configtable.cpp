/* file: configtable.cpp
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

#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include "table.h"

/*
 *
 */
ConfigTable::ConfigTable() {}

/*
 *
 */
ConfigTable::ConfigTable(const ConfigTable& ct) {
  ConfigTable* pct = (ConfigTable*)(&ct);
  TableBuffer* tb = (TableBuffer*)buffer;
  if(tb != NULL) delete tb;
  buffer = pct->buffer;
  pct->buffer = new TableBuffer(0);
}

/*
 *
 */
ConfigTable::ConfigTable(const std::string& s, const char c) {
  TableBuffer* tb = (TableBuffer*)buffer;
  if(tb != NULL) delete tb;
  ConfigTable* t = (ConfigTable*)(new Table(s, c));
  buffer = t->buffer;
  t->buffer = new TableBuffer(0);
  delete t;
  this->sort();
}

/*
 *
 */
Row
ConfigTable::operator[](const string& attr) const {
  int i, n;
  int index = -1;
  int rank = -1;
  vector<int> indices;
  vector<int> columns;
  vector<string> keywords;
  Table* t = (Table*)this;
  Row r;

  /* Find relevant rows. */
  if(t->size() < 1) return r;
  columns.push_back(0);
  keywords.push_back(attr);
  indices = t->find(keywords, columns);
  
  /* Find row with largest rank in the original data. */
  n = indices.size();
  for(i = 0; i < n; i++) {
    r = (*t)[indices[i]];
    if(r.rank() > rank) {
      rank = r.rank();
      index = indices[i];
    }
  }

  return (*t)[index];
}

/*
 *
 */
Row
ConfigTable::get(const string& attr, const int i) const {
  int n;
  int index = -1;
  vector<int> indices;
  vector<int> columns;
  vector<string> keywords;
  Table* t = (Table*)this;
  Row r;

  /* Find relevant rows. */
  if(t->size() < 1) return r;
  columns.push_back(0);
  keywords.push_back(attr);
  indices = t->find(keywords, columns);

  /* Find row with the given index. */
  index = -1;
  n = indices.size();
  if((i >= 0) && (i < n))
    index = indices[i];

  return (*t)[index];
}

/*
 *
 */
int
ConfigTable::ordinal(const string& attr, const int n_arg, ...) const {
  int i;
  va_list ap;
  char* label;
  string field;

  va_start(ap, n_arg);
  field = (*this)[attr][1];
  if(field.length() > 0) {
    for(i = 0; i < n_arg; i++) {
      label = va_arg(ap, char*);
      if(label == NULL) return -1;
      if(field == label) return i;
    }
  }
  
  return -1;
}
