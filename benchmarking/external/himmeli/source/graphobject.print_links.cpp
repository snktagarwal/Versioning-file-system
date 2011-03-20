/* file: graphobject.print_links.cpp
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

#include "graphobject.h"

static void draw_edge(Artist&, Vertex&, Vertex&, Arc&, double, double, bool);

/*
 *
 */
void
GraphObject::print_links(Artist& art, map<string, Vertex>& vertices, 
			 vector<Edge>& edges) {
  unsigned int i;
  char delim = delimiter;
  if(delim == '\0') delim = ' ';

  /* Draw edges. */
  art.group(true);
  sort(edges.begin(), edges.end(), CompareEdge());
  for(i = 0; i < edges.size(); i++) {
    string head = edges[i].head;
    string tail = edges[i].tail;
    string key = (head + delim + tail);
    Vertex vH = vertices[head];
    Vertex vT = vertices[tail];
    Emblem e = emblems[head];
    Arc a = arcs[key];

    /* Determine opacity. */
    double alpha = 1.0;
    if(edges[i].weight < 0.5)
      alpha -= (0.5 - edges[i].weight);

    /* Add the edge to the canvas. */
    double r = DEFAULT_RADIUS*(e.radius);
    draw_edge(art, vH, vT, a, r, alpha, arrow_mode);
  }
  art.group(false);
}

/*
 *
 */
static void
draw_edge(Artist& art, Vertex& vH, Vertex& vT, Arc& a,
	  double r, double alpha, bool arrow_flag) {
  if(!(a.width > 0.0)) return;

  /* Determine font size. */
  double width = DEFAULT_WIDTH*(a.width);
  double fs = 0.6*atof(art["FontSize"].c_str());
  if(width > fs) fs = width;

  /* Define style. */
  Style sty;
  sty.fillopacity = 0;
  sty.strokeopacity = alpha;
  sty.strokewidth = width;
  sty.strokecolor = color2rgb(a.color);

  /* First coordinate. */
  vector<double> vx(1, vT.x);
  vector<double> vy(1, vT.y);

  /* Line angle. */
  double phi = 0.0;
  double dx = (vH.x - vT.x);
  double dy = (vH.y - vT.y);
  double d = sqrt(dx*dx + dy*dy);
  if(dx > 0) phi = atan(dy/(dx + FLT_EPSILON));
  else phi = (M_PI + atan(dy/(dx - FLT_EPSILON)));

  /* Arrowhead. */
  double d1 = d/2;
  double d2 = d/2;
  if(arrow_flag) {
    d1 = (d - r - 1.1*width);
    d2 = (d1 - 1.6*width);
    double psi = atan((d1 - d2)*tan(0.35)/d2);
    vx.push_back(vT.x + d2*cos(phi));
    vy.push_back(vT.y + d2*sin(phi));
    vx.push_back(vT.x + d2*cos(phi + psi));
    vy.push_back(vT.y + d2*sin(phi + psi));
    vx.push_back(vT.x + d1*cos(phi));
    vy.push_back(vT.y + d1*sin(phi));
    vx.push_back(vT.x + d2*cos(phi - psi));
    vy.push_back(vT.y + d2*sin(phi - psi));
    vx.push_back(vT.x + d2*cos(phi));
    vy.push_back(vT.y + d2*sin(phi));
  }
  else {
    vx.push_back(vH.x);
    vy.push_back(vH.y);
  }

  /* Print link. */
  art.curve(vx, vy, sty);

  /* Determine label rotation. */
  double theta = phi;
  unsigned int n = a.label.size();
  double d3 = (d2 - n*fs/2);
  if(theta > M_PI) theta -= M_PI;
  if(theta < 0) theta += M_PI;
  if(d3 < (r + fs)) theta -= M_PI/2;
  if(theta > M_PI/2) theta -= M_PI;
  if(theta < -M_PI/2) theta += M_PI;

  /* Determine label position. */
  if(n < 1) return;
  if(d3 < (r + fs) || !arrow_flag) d3 = d/2;
  double x = (vT.x + d3*cos(phi));
  double y = (vT.y + d3*sin(phi));

  /* Set text style. */
  sty.anchor = "middle";
  sty.fontsize = fs;
  sty.strokewidth = 0.27*fs;
  sty.angle = 180*theta/M_PI;
  sty.origin = vector<double>(2);
  sty.origin[0] = x;
  sty.origin[1] = y;

  /* Make shadow nicer. */
  if(art["Format"] == "SVG")
    sty.custom = "stroke-linejoin: round;";

  /* Choose text color. */
  vector<double> clr = sty.strokecolor;
  double lum = (clr[0])*(clr[1])*(clr[2]);
  if(lum < 0.4) sty.fillcolor = color2rgb(999999);
  else sty.fillcolor = color2rgb(707070);
  if(sty.strokeopacity > 0.7) sty.strokeopacity = 0.7;

  /* Print label. */
  art.group(true);
  art.text(x, (y + 0.27*fs), a.label, sty);
  sty.fillopacity = 1.0; sty.strokeopacity = 0.0;
  art.text(x, (y + 0.27*fs), a.label, sty);
  art.group(false);
}
