/* file: svgobject_methods.cpp
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

static string get_color(vector<double>);
static vector<double> get_rgb(string);

/*
 *
 */
vector<double>
SVGObject::bgcolor() {
  string arg = retrieve("BackgroundColor");
  return get_rgb(arg);
}

/*
 *
 */
vector<double>
SVGObject::fgcolor() {
  string arg = retrieve("ForegroundColor");
  return get_rgb(arg);
}

/*
 *
 */
bool
SVGObject::finish() {
  if(size() == 0) append(prolog());
  while(group(false) > 0) {};
  if(append("\n</g>\n</svg>\n")) return close();
  return false;
}

/*
 *
 */
int
SVGObject::group(const bool flag) {
  if(flag) {
    if(size() == 0) append(prolog());
    if(append("\n<g>")) return ++ngroups;
    return -1;
  }
  if(ngroups > 0) { 
    if(append("\n</g>")) return --ngroups;
    return -1; 
  }
  return ngroups;
}

/*
 *
 */
string
SVGObject::prolog() {
  char buf[2048];
  char* ptr = buf;


  /* Determine canvas size and scale. */
  double w = CM2DOT*width();
  double h = CM2DOT*height();
  double z = atof(retrieve("Scale").c_str());
  if((z < 0.01) || (z > 100.0)) {
    sprintf(buf, "ERROR! %s at line %d: Invalid scale.\n",
	    __FILE__, __LINE__);
    exit(1);
  }

  /* Begin document. */
  ptr += sprintf(ptr, "<?xml version=\"1.0\"?>\n");
  ptr += sprintf(ptr, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n");
  ptr += sprintf(ptr, "\"http://www.w3.org/Graphics/");
  ptr += sprintf(ptr, "SVG/1.1/DTD/svg11.dtd\">\n");
  ptr += sprintf(ptr, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
  ptr += sprintf(ptr, "x=\"0\" y=\"0\"\n");
  ptr += sprintf(ptr, "width=\"%.0f\" height=\"%.0f\">\n", z*w, z*h);

  /* Cartesian coordinates. */
  if(z == 1.0) ptr += sprintf(ptr, "\n<g>\n");
  else ptr += sprintf(ptr, "\n<g transform=\"scale(%.4f)\">\n", z);

  /* Set background color. */
  Style sty;
  sty.fillcolor = bgcolor();
  sty.fillopacity = 1;
  sty.strokeopacity = 0;
  ptr += sprintf(ptr, "\n<rect ");
  ptr += sprintf(ptr, "x=\"0\" y=\"0\" ");
  ptr += sprintf(ptr, "width=\"%.0f\" height=\"%.0f\"\n", w, h);
  ptr += sprintf(ptr, "%s/>\n", style(sty).c_str());

  return string(buf);
}

/*
 *
 */
string
SVGObject::style(const Style& sty) {
  char buf[2048]; buf[0] = '\0';
  char* ptr = buf;
  string sdef = "";
  string tform = "";

  /* Rotation. */
  if((sty.angle != 0) && (sty.origin.size() > 1)) {
    ptr += sprintf(ptr, "\nrotate(%.2f, ", sty.angle);
    ptr += sprintf(ptr, "%.2f, ", CM2DOT*(sty.origin[0]));
    ptr += sprintf(ptr, "%.2f)", CM2DOT*(sty.origin[1]));
  }

  /* Collect transforms. */
  if(ptr != buf) tform = ("transform=\"" + string(buf) + "\"\n");
  ptr = buf; buf[0] = '\0';

  /* Font. */
  double fs = atof(retrieve("FontSize").c_str());
  string ffam = retrieve("FontFamily");
  if(sty.fontsize > 0) fs = sty.fontsize;
  if(sty.fontfamily != "") ffam = sty.fontfamily;
  ptr += sprintf(ptr, "\nfont-size: %.2fpx;", CM2DOT*fs);
  ptr += sprintf(ptr, "\nfont-family: %s;", ffam.c_str());
  
  /* Text-anchor. */
  if(sty.anchor != "")
    ptr += sprintf(ptr, "\ntext-anchor: %s;", sty.anchor.c_str());

  /* Fill color. */
  if((sty.fillcolor.size() > 2) && (sty.fillopacity > 0)) {
    ptr += sprintf(ptr, "\nfill: %s;", get_color(sty.fillcolor).c_str());
    if(sty.fillopacity < 1) 
      ptr += sprintf(ptr, "\nfill-opacity: %.2f;", sty.fillopacity);
  }
  else
    ptr += sprintf(ptr, "\nfill: none;");
  
  /* Stroke color and width. */
  if((sty.strokecolor.size() > 2) && (sty.strokeopacity > 0)) {
    ptr += sprintf(ptr, "\nstroke: %s;", get_color(sty.strokecolor).c_str());
    ptr += sprintf(ptr, "\nstroke-linecap: round;");
    if(sty.strokewidth > 0)
      ptr += sprintf(ptr, "\nstroke-width: %.2fpx;", CM2DOT*(sty.strokewidth));
    if(sty.strokeopacity < 1)
      ptr += sprintf(ptr, "\nstroke-opacity: %.2f;", sty.strokeopacity);
  }
  else
    ptr += sprintf(ptr, "\nstroke: none;");

  /* User-defined. */
  if(sty.custom != "") 
    ptr += sprintf(ptr, "\n%s", sty.custom.c_str());

  /* Collect style definitions. */
  if(ptr != buf) sdef = ("style=\"" + string(buf) + "\"\n");
  ptr = buf; buf[0] = '\0';

  /* Combine results. */
  return (tform + sdef);
}

/*
 *
 */
static string
get_color(vector<double> rgb) {
  unsigned int i;
  char buf[64];
  vector<int> clr(3, 0);
  for(i = 0; (i < rgb.size()) && (i < clr.size()); i++) {
    clr[i] = (int)(255*rgb[i]);
    if(clr[i] < 0) clr[i] = 0;
    if(clr[i] > 255) clr[i] = 255;
  }
  sprintf(buf, "#%02x%02x%02x", clr[0], clr[1], clr[2]);
  return string(buf);
}

/*
 *
 */
static vector<double>
get_rgb(string arg) {
  int code = atoi(arg.c_str());
  vector<double> rgb(3);
  if(code < 0) code = 0;
  if(code > 999999) code = 999999;
  rgb[0] = code/10000;
  rgb[1] = (code - (int)(rgb[0]*10000))/100;
  rgb[2] = (code - (int)(rgb[0]*10000 + rgb[1]*100));
  rgb[0] /= 100; rgb[1] /= 100; rgb[2] /= 100;
  return rgb;
}
