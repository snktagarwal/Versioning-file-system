/* file: svgobject.text.cpp
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
SVGObject::text(const double x0, const double y0, const string& s,
		const Style& sty0) {
  double x = CM2DOT*x0;
  double y = CM2DOT*y0;
  char buf[2048];
  if(s.size() < 1) return false;
  if(!(0*x0 == 0)) return false;
  if(!(0*y0 == 0)) return false;

  /* Check for non-alphanumeric characters. */
  string txt = s;
  if((*this)["SafeMode"] == "on")
    txt = string2safe(txt, SAFE_TXT_SIZE);

  /* Set style. */
  Style sty = sty0;
  if(sty.fillcolor.size() < 3) sty.fillcolor = fgcolor();
  if(sty.strokecolor.size() < 3) sty.strokecolor = bgcolor();

  /* Print text. */
  if(size() == 0) append(prolog());
  sprintf(buf, "\n<text x=\"%.2f\" y=\"%.2f\"\n", x, y);
  if(!append(string(buf))) return false;
  return append(style(sty) + ">\n" + txt + "\n</text>\n");
}
