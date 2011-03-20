/* file: svgobject.curve.cpp
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

#include "scriptum.h"
#include "artistobject.h"

using namespace std;
using namespace scriptum;

/*
 *
 */
bool
SVGObject::curve(const vector<double>& vx0, const vector<double>& vy0,
		 const Style& sty) {
  unsigned int i;
  unsigned int n = vx0.size();
  if(n != vy0.size()) return false;
  if(n < 2) return false;

  /* Convert centimeters to pixels. */
  vector<double> vx = vx0;
  vector<double> vy = vy0;
  for(i = 0; i < n; i++) {
    vx[i] *= CM2DOT;
    vy[i] *= CM2DOT;
    if(!(0*vx[i] == 0)) return false;
    if(!(0*vy[i] == 0)) return false;
  }

  /* Check if closed path. */
  bool closeflag = ((vx[0] == vx[n-1]) && (vy[0] == vy[n-1]));
  if(closeflag && (n < 3)) return false;

  /* Create polyline. */
  char buf[2048];
  char* ptr = buf;
  ptr += sprintf(ptr, "\n<path d=\"\n");
  ptr += sprintf(ptr, "M\t%.2f\t%.2f", vx[0], vy[0]);
  if(closeflag) {
    for(i = 1; i < (n - 1); i++)
      ptr += sprintf(ptr, "\nL\t%.2f\t%.2f", vx[i], vy[i]);
    ptr += sprintf(ptr, "\nZ\"\n");
  }
  else {
    for(i = 1; i < n; i++)
      ptr += sprintf(ptr, "\nL\t%.2f\t%.2f", vx[i], vy[i]);
    ptr += sprintf(ptr, "\"\n");
  }

  /* Apply style. */
  if(ptr == buf) return false;
  ptr += sprintf(ptr, "%s", style(sty).c_str());
  ptr += sprintf(ptr, "/>\n");

  /* Print code. */
  if(size() == 0) append(prolog());
  return append(string(buf));
}
