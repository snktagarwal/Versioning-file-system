/* file: uniqstring.cpp
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

#include "medusa.h"

using namespace std;

/*
 *
 */
vector<unsigned int>
medusa::uniqstring(vector<string>& items) {
  unsigned int k;
  vector<string> ss = items; items.clear();
  vector<unsigned int> sorted = sortstring(ss);
  vector<unsigned int> ind;
  if(sorted.size() < 1) return ind;

  /* Remove duplicates. */
  ind.push_back(sorted[0]);
  items.push_back(ss[0]);
  for(k = 1; k < sorted.size(); k++) {
    if(ss[k] == ss[k-1]) continue;
    ind.push_back(sorted[k]);
    items.push_back(ss[k]);
  }

  return ind;
}
