/* file: string2double.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include "medusa.h"

using namespace std;

/*
 *
 */
double
medusa::string2double(const string& x) {
  unsigned int i;
  unsigned int n = x.size();
  
  /* Replace commas. */
  string s = x;
  for(i = 0; i < n; i++)
    if(s[i] == ',') s[i] = '.';
  
  /* Check if illegible. */
  bool flag = false;
  for(i = 0; i < n; i++) {
    if(!flag && isspace(s[i])) continue;
    if(flag && isspace(s[i])) return medusa::nan();
    if(flag && (s[i] == '.')) return medusa::nan();
    if(isdigit(s[i])) return atof(s.c_str());
    flag = true;
  }
  return medusa::nan();
}
