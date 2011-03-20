/* file: graphobject.print_nodes.cpp
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

static void  draw_emblem(Artist&, Emblem&, Vertex&);
static void  draw_label(Artist&, Emblem&, Vertex&);

/*
 *
 */
void
GraphObject::print_nodes(Artist& art, map<string, Vertex>& vertices) {
  map<string, Vertex>::iterator pos;

  /* Emblems. */
  art.group(true);
  for(pos = vertices.begin(); pos != vertices.end(); pos++)
    draw_emblem(art, emblems[pos->first], pos->second);
  art.group(false);

  /* Labels. */
  art.group(true);
  for(pos = vertices.begin(); pos != vertices.end(); pos++)
    draw_label(art, emblems[pos->first], pos->second);
  art.group(false);
}

/*
 *
 */
static void
draw_emblem(Artist& art, Emblem& e, Vertex& v)  {
  double r = DEFAULT_RADIUS*(e.radius);

  /* Define style. */
  Style sty;  
  sty.strokewidth = DEFAULT_WIDTH*(r + 1)/5.0;
  sty.fontsize = -1;
  sty.fillopacity = 1.0;
  sty.strokeopacity = 0.5;
  sty.fillpattern = e.pattern;
  if(e.color >= 0) sty.fillcolor = color2rgb(e.color);

  /* Print emblem. */
  art.shape(v.x, v.y, r, e.shape, sty);
}

/*
 *
 */
static void
draw_label(Artist& art, Emblem& e, Vertex& v)  {
  double r = DEFAULT_RADIUS*(e.radius);
  double fs = atof(art["FontSize"].c_str());
  if(e.label == "") return;

  /* Adjust font size to radius. */
  if(r < 0.5) fs *= 0.8;
  if(r > 1.5) fs *= 1.5;
  if(r > 3.0) fs *= 1.5;

  /* Define style. */
  Style sty;  
  sty.fontsize = fs;
  sty.fillopacity = 0.7;
  sty.strokeopacity = 0;

  /* Print label. */
  art.text((v.x + r + fs/8), (v.y + fs/4), e.label, sty);
}
