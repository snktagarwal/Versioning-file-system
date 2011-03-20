/* file: svgobject.getpattern.cpp
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

static void get_template(vector<double>&, vector<double>&,
			 double, string, double);

/*
 *
 */
string
SVGObject::getpattern(double x, double y, double r, string name,
		      string path) {
  unsigned int i;
  string id = "";
  string clip = "";
  string pattern = "";
  char buf[2048];
  char* ptr = buf;
  vector<double> lx, ly;
  vector<double> px, py;
  vector<double> cx, cy;
  if(path.size() < 1) return "";

  /* Create clip element. */
  id = newname();
  clip = ("\n<clipPath id=\"" + id + "\">");
  clip += (path + "style=\"\nfill: black;\nstroke: none;\"/>\n");
  clip += ("</clipPath>");
  
  /* Determine symbol rotation. */
  string s = (name + "/0.0000000");
  const char* array = s.c_str();
  char* pos = strchr(array, '/'); pos++;
  double theta = atof(pos)*M_PI/180;

  /* Create patterns. */
  if(s.substr(0, 6) == "stripe")
    get_template(lx, ly, r, "stripe", theta);
  if(s.substr(0, 4) == "spot")
    get_template(px, py, r, "spot", theta);
  if(s.substr(0, 3) == "arc")
    get_template(cx, cy, r, "arc", theta);
  if(s.substr(0, 4) == "half")
    get_template(px, py, r, "half", theta);
  if(s.substr(0, 6) == "cheque")
    get_template(px, py, r, "cheque", theta);
  if(s.substr(0, 4) == "hole")
    get_template(px, py, r, "hole", theta);
  
  /* Null style. */
  Style sty;
  sty.fillcolor = fgcolor(); 
  sty.strokecolor = fgcolor(); 
  sty.fillopacity = 0; 
  sty.strokeopacity = 0; 
  sty.strokewidth = 0.06*sqrt(r/CM2DOT);

  /* Circular pattern. */
  if(cx.size() > 0) {
    sty.fillopacity = 0.7;
    ptr += sprintf(ptr, "\n<circle ");
    if(clip.size() > 0)
      ptr += sprintf(ptr, "clip-path=\"url(#%s)\" ", id.c_str());
    ptr += sprintf(ptr, "\ncx=\"%.2f\" ", (x + cx[0]));
    ptr += sprintf(ptr, "cy=\"%.2f\" ", (y + cy[0]));
    ptr += sprintf(ptr, "r=\"%.2f\"\n", sqrt(cx[0]*cx[0] + cy[0]*cy[0]));
    ptr += sprintf(ptr, "%s/>\n", style(sty).c_str());
    pattern = string(buf);
  }

  /* Line pattern. */
  if(lx.size() > 0) {
    sty.strokeopacity = 0.7;
    ptr += sprintf(ptr, "\n<path ");
    if(clip.size() > 0)
      ptr += sprintf(ptr, "clip-path=\"url(#%s)\" ", id.c_str());
    ptr += sprintf(ptr, "d=\"\nM\t%.2f\t%.2f", (x + lx[0]), (y + ly[0]));
    for(i = 1; i < lx.size(); i++)
      ptr += sprintf(ptr, "\nL\t%.2f\t%.2f", (x + lx[i]), (y + ly[i]));
    ptr += sprintf(ptr, "\"\n");
    ptr += sprintf(ptr, "%s/>\n", style(sty).c_str());
    pattern = string(buf);
  }
  
  /* Polygon pattern. */
  if(px.size() > 0) {
    sty.fillopacity = 0.7;
    ptr += sprintf(ptr, "\n<polygon ");
    if(clip.size() > 0)
      ptr += sprintf(ptr, "clip-path=\"url(#%s)\" ", id.c_str());
    ptr += sprintf(ptr, "points=\"");
    for(i = 0; i < px.size(); i++) {
      ptr += sprintf(ptr, "\n\t%.2f,", (x + r*px[i]));
      ptr += sprintf(ptr, "%.2f", (y + r*py[i]));
    }
    ptr += sprintf(ptr, "\"\n");
    ptr += sprintf(ptr, "%s/>\n", style(sty).c_str());
    pattern = string(buf);
  }

  /* Check if pattern was created. */
  if(pattern == "") return "";
  return (clip + pattern);
}

/*
 *
 */
static void
get_template(vector<double>& vx, vector<double>& vy,
	     double r, string name, double theta) {
  unsigned int i;
  vector<double> px;
  vector<double> py;

  if(name == "cheque") {
    px = vector<double>(6);
    py = vector<double>(6);
    px[0]  =  2.0000; py[0]  =  2.0000;
    px[1]  =  0.0000; py[1]  =  2.0000;
    px[2]  =  0.0000; py[2]  = -2.0000;
    px[3]  = -2.0000; py[3]  = -2.0000;
    px[4]  = -2.0000; py[4]  =  0.0000;
    px[5]  =  2.0000; py[5]  =  0.0000;
  }
  
  if(name == "stripe") {
    unsigned int n = 24;
    px = vector<double>(n);
    py = vector<double>(n);
    for(i = 0; i < n; i += 2) {
      px[i] = 1.3*((i%4) - 1.0);
      px[i+1] = px[i];
      py[i] = 2.6*((i + 0.0)/n - 0.5);
      py[i+1] = 2.6*((i + 2.0)/n - 0.5);
    }
  }
  
  if(name == "spot") {
    px = vector<double>(4);
    py = vector<double>(4);
    px[0] = 0.0000; py[0] = 0.0000;
    px[1] = 0.0000; py[1] = 2.0000;
    px[2] = 2.0000; py[2] = 2.0000;
    px[3] = 2.0000; py[3] = 0.0000;
  }
  
  if(name == "hole") {
    px = vector<double>(6);
    py = vector<double>(6);
    px[0] = -2.0000; py[0] =  2.0000;
    px[1] =  0.0000; py[1] =  2.0000;
    px[2] =  0.0000; py[2] =  0.0000;
    px[3] =  2.0000; py[3] =  0.0000;
    px[4] =  2.0000; py[4] = -2.0000;
    px[5] = -2.0000; py[5] = -2.0000;    
  }
  
  if(name == "half") {
    px = vector<double>(4);
    py = vector<double>(4);
    px[0] =  0.0000; py[0] = -2.0000;
    px[1] =  0.0000; py[1] =  2.0000;
    px[2] =  2.0000; py[2] =  2.0000;
    px[3] =  2.0000; py[3] = -2.0000;    
  }
  
  if(name == "arc") {
    px = vector<double>(1);
    py = vector<double>(1);
    px[0] = 0.3500; py[0] = 0.3500;
  }
  
  /* Rotate. */
  for(i = 0; i < px.size(); i++) {
    double phi = 0.0;
    double rad = sqrt(px[i]*px[i] + py[i]*py[i]);
    if(px[i] > 0) phi = atan(py[i]/(px[i] + FLT_EPSILON));
    else phi = (M_PI + atan(py[i]/(px[i] - FLT_EPSILON)));
    px[i] = rad*cos(phi + theta);
    py[i] = rad*sin(phi + theta);
  }

  /* Copy coordinates. */
  double scale = sqrt(r/CM2DOT)*CM2DOT;
  vx.clear(); vy.clear();
  for(i = 0; i < px.size(); i++) {
    vx.push_back(scale*px[i]);
    vy.push_back(scale*py[i]);
  }
}
