/* file: long2text.cpp
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
#include "medusa.h"

using namespace std;

/*
 * Convert integers into human readable form.
 */
string
medusa::long2text(const long unsigned int n) {
  int exponent = 0;
  double size = 1.0*n;
  char buf[32];

  if(size >= 1000.0) {
    while(size >= 100.0) {
      size /= 1000;
      exponent++;
    }
    if(size < 10.0) sprintf(buf, "%4.2f?", size);
    else if(size < 100.0) sprintf(buf, "%4.1f?", size);
    switch(exponent) {
    case 1: buf[4] = 'K'; break;
    case 2: buf[4] = 'M'; break;
    case 3: buf[4] = 'G'; break;
    case 4: buf[4] = 'T'; break;
    case 5: buf[4] = 'P'; break;
    case 6: buf[4] = 'E'; break;
    case 7: buf[4] = 'Z'; break;
    case 8: buf[4] = 'Y'; break;
    }
  }
  else
    sprintf(buf, "%4.0f ", size);

  return string(buf);
}
