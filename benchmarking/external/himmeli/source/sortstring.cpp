/* file: sortstring.cpp
  Copyright (C) 2009 Ville-Petteri Makinen

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

    WWW: http://www.iki.fi/~vpmakine
*/

#include <algorithm>
#include "medusa.h"

using namespace std;

struct StringItem {
  string s;
  unsigned int rank;
};
class CompareString {
public:
  bool operator()(const StringItem& x, const StringItem& y) const {
    return (x.s < y.s);
  };
};

/*
 *
 */
vector<unsigned int>
medusa::sortstring(vector<string>& items) {
  unsigned int k;
  vector<StringItem> tuples;
  
  /* Create string-rank pairs. */ 
  for(k = 0; k < items.size(); k++) {
    StringItem item;
    item.s = items[k];
    item.rank = k;
    tuples.push_back(item);
  }
  
  /* Sort string items. */
  vector<unsigned int> ind(tuples.size()); 
  sort(tuples.begin(), tuples.end(), CompareString());
  for(k = 0; k < tuples.size(); k++) {
    ind[k] = tuples[k].rank;
    items[k] = tuples[k].s;
  }

  return ind;
}
