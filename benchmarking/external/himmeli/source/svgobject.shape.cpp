/* file: svgobject.shape.cpp
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

static string get_path(double, double, double, string);

/*
 *
 */
bool
SVGObject::shape(const double x0, const double y0, const double r0,
		 const string& name, const Style& sty0) {
  Style sty = sty0;

  /* Convert centimeters to pixels. */
  double x = CM2DOT*x0;
  double y = CM2DOT*y0;
  double r = CM2DOT*r0;
  if(!(0*x == 0)) return false;
  if(!(0*y == 0)) return false;
  if(!(0*r == 0)) return false;

  /*  Outline. */
  string path = get_path(x, y, r, name);
  if(path.size() < 1) return false;

  /*  Pattern and clip path. */
  string pattern = getpattern(x, y, r, sty.fillpattern, path);

  /* Style. */
  if(sty.fillcolor.size() < 3) sty.fillcolor = bgcolor();
  if(sty.strokecolor.size() < 3) sty.strokecolor = fgcolor();
  string looks = (style(sty) + "/>\n");

  /* Print code. */
  if(size() == 0) append(prolog());
  if(pattern.size() > 0)
    return append("<g>\n" + path + looks + pattern + "</g>\n");
  return append(path + looks);
}

/*
 *
 */
static string
get_path(double x, double y, double r, string name) {
  unsigned int i;
  char buf[2048];
  char* ptr = buf;
  vector<double> px;
  vector<double> py;

  /* Determine symbol rotation. */
  string s = (name + "/0.0000000");
  const char* array = s.c_str();
  char* pos = strchr(array, '/'); pos++;
  double theta = atof(pos)*M_PI/180;
  
  /* Create shapes. */
  if(s.substr(0, 6) == "circle") {
    ptr += sprintf(ptr, "\n<circle ");
    ptr += sprintf(ptr, "cx=\"%.2f\" cy=\"%.2f\" ", x, y);
    ptr += sprintf(ptr, "r=\"%.3f\"\n", 0.8190*r);
    return string(buf);
  }
  if(s.substr(0, 8) == "triangle") {
    px = vector<double>(3); py = vector<double>(3);
    px[0]  =  0;      py[0]  =  0.9906;
    px[1]  =  0.9009; py[1]  = -0.5694;
    px[2]  = -0.9009; py[2]  = -0.5694;
  }
  if(s.substr(0, 5) == "cross") {
    px = vector<double>(12); py = vector<double>(12);
    px[0]  = -0.3666; py[0]  = -0.3666;
    px[1]  = -0.8798; py[1]  = -0.3666;
    px[2]  = -0.8798; py[2]  =  0.3666;
    px[3]  = -0.3666; py[3]  =  0.3666;
    px[4]  = -0.3666; py[4]  =  0.8798;
    px[5]  =  0.3666; py[5]  =  0.8798;
    px[6]  =  0.3666; py[6]  =  0.3666;
    px[7]  =  0.8798; py[7]  =  0.3666;
    px[8]  =  0.8798; py[8]  = -0.3666;
    px[9]  =  0.3666; py[9]  = -0.3666;
    px[10] =  0.3666; py[10] = -0.8798;
    px[11] = -0.3666; py[11] = -0.8798;
  }
  if(s.substr(0, 4) == "star") {
    px = vector<double>(10); py = vector<double>(10);
    px[0]  =  0.9347; py[0]  =  0.3037;
    px[1]  =  0.2888; py[1]  =  0.3976;
    px[2]  =  0.0000; py[2]  =  0.9828;
    px[3]  = -0.2888; py[3]  =  0.3976;
    px[4]  = -0.9347; py[4]  =  0.3037;
    px[5]  = -0.4673; py[5]  = -0.1519;
    px[6]  = -0.5777; py[6]  = -0.7951;
    px[7]  = -0.0000; py[7]  = -0.4914;
    px[8]  =  0.5777; py[8]  = -0.7951;
    px[9]  =  0.4673; py[9]  = -0.1519;
  }
  if(s.substr(0, 6) == "square") {
    px = vector<double>(4); py = vector<double>(4);
    px[0]  = -0.7020; py[0]  =  0.7020;
    px[1]  =  0.7020; py[1]  =  0.7020;
    px[2]  =  0.7020; py[2]  = -0.7020;
    px[3]  = -0.7020; py[3]  = -0.7020;
  }
  
  /* Rotate. */
  for(i = 0; i < px.size(); i++) {
    double phi = 0.0;
    double r = sqrt(px[i]*px[i] + py[i]*py[i]);
    if(px[i] > 0) phi = atan(py[i]/(px[i] + FLT_EPSILON));
    else phi = (M_PI + atan(py[i]/(px[i] - FLT_EPSILON)));
    px[i] = r*cos(phi + theta);
    py[i] = r*sin(phi + theta);
  }

  /* Draw polygon. */
  if(px.size() > 0) {
    ptr += sprintf(ptr, "\n<polygon points=\"");
    for(i = 0; i < px.size(); i++) {
      ptr += sprintf(ptr, "\n\t%.2f,", (x + r*px[i]));
      ptr += sprintf(ptr, "%.2f", (y + r*py[i]));
    }
    ptr += sprintf(ptr, "\"\n");
    return string(buf);
  }

  return "";
}
