/* file: table.find.cpp
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
 *
 */
int
Table::find(const string& keyword, const int column) const {
  int n;
  vector<int> indices;
  vector<int> columns;
  vector<string> keywords;
  columns.push_back(column);
  keywords.push_back(keyword);
  indices = find(keywords, columns);
  n = indices.size();
  if(n < 1) return -1;
  return indices[n-1];
}

/*
 *
 */
vector<int>
Table::find(const vector<string>& keywords,
		  const vector<int>& columns) const {
  int i, k, n;
  row_t rkey[1];
  row_t* hit;
  vector<int> indices(0);
  TableBuffer* tb = (TableBuffer*)buffer;

  n = keywords.size();
  k = columns.size();
  if(n > k) n = k;

  /* Check that table is sorted properly. */
  if(tb->keycols == NULL) {
    cerr << "WARNING! " << __FILE__ <<  ": Table is not sorted.\n";
    return indices;
  }
  for(i = 0, k = 0; i < n; i++) {
    if(columns[i] < 0) continue;
    if(columns[i] >= tb->n_cols) continue;
    if((tb->keycols[k] != columns[i]) || (tb->keycols[k] < 0)) {
      cerr << "WARNING! " << __FILE__
	   <<  ": Table is not sorted correctly.\n";
      return indices;
    }
    k++;
  }

  /* Create prototype row. */
  rkey->rank = -1;
  rkey->size = 0;
  rkey->n_keys = n;
  rkey->n_fields = 1;
  rkey->key_0 = NULL;
  rkey->key_1 = NULL;
  rkey->key_2 = NULL;
  rkey->key_3 = NULL;
  rkey->line = &EmptyChar;
  rkey->keycols = tb->keycols;

  /* Make key shortcuts. */
  if(n > 0) rkey->key_0 = (char*)(keywords[0].c_str());
  if(n > 1) rkey->key_1 = (char*)(keywords[1].c_str());
  if(n > 2) rkey->key_2 = (char*)(keywords[2].c_str());
  if(n > 3) rkey->key_3 = (char*)(keywords[3].c_str());
  if(n > 4) {
    cerr << "ERROR! " << __FILE__ << " at line " << __LINE__
	 << ": Max 4 keys supported.\n";
    exit(1);
  };

  /* Find a matching row. */
  hit = (row_t*)bsearch(rkey, tb->rows, tb->n_rows, sizeof(row_t), &rowcmp);
  if(hit == NULL) return indices;

  /* Find all matching rows. */
  n = 0;
  k = (int)(hit - tb->rows);
  for(i = (k - 1); i >= 0; i--) {
    if(rowcmp((tb->rows + i), rkey) == 0) k = i;
    else break;
  }
  for(i = k; i < tb->n_rows; i++) {
    if(rowcmp((tb->rows + i), rkey) == 0) indices.push_back(i);
    else break;
  }

  return indices;
}
