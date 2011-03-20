/* file: double2string.cpp
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

#include <math.h>
#include <stdio.h>
#include "medusa.h"

using namespace std;

/*
 *
 */
string
medusa::double2string(const double x) {
  char buf[64];
  double integer = 0.0;
  double fraction = modf(x, &integer);
  if((fraction == 0) || (integer > 1e5)) {
    sprintf(buf, "%.0f", x);
    return string(buf);
  }
  if(integer > 1e4) {
    sprintf(buf, "%.2f", x);
    return string(buf);
  }
  if(integer > 1e2) {
    sprintf(buf, "%.4f", x);
    return string(buf);
  }
  if(integer > 0) {
    sprintf(buf, "%.6f", x);
    return string(buf);
  }
  if(fraction > 1e-2) {
    sprintf(buf, "%.8f", x);
    return string(buf);
  }
  sprintf(buf, "%.5e", x);
  return string(buf);
}
