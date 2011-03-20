/* file: graphobject.print_info.cpp
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

#define FONT_UNIT        0.4
#define MAX_LABEL_LENGTH 12

static vector<double> get_limits(vector<Component>&);

/*
 *
 */
bool
GraphObject::print_info(Artist& art) {
  unsigned int i;
  double r = 1.5*FONT_UNIT;

  /* Text style. */
  Style txtsty;
  txtsty.fontsize = FONT_UNIT;
  txtsty.fillopacity = 0.7;

  /* Symbol style. */
  Style sty;
  sty.strokewidth = 0.07*r;
  sty.fontsize = -1;

  /* Read edge info items. */
  vector<int> e_color_codes;
  vector<string> e_color_labels;
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("EdgeColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    e_color_codes.push_back((int)(row.number(2)));
    e_color_labels.push_back(row[1]);
  }

  /* Read vertex info items. */
  vector<int> v_color_codes;
  vector<string> v_pattern_codes;
  vector<string> v_shape_codes;
  vector<string> v_color_labels;
  vector<string> v_pattern_labels;
  vector<string> v_shape_labels;
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_color_codes.push_back((int)(row.number(2)));
    v_color_labels.push_back(row[1]);
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexPatternInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_pattern_codes.push_back(row[2]);
    v_pattern_labels.push_back(row[1]);
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexShapeInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_shape_codes.push_back(row[2]);
    v_shape_labels.push_back(row[1]);
  }
  
  /* Count number of rows. */
  double n_erows = 0.0;
  double n_vrows = 0.0;
  if(cfg["EdgeColorVariable"][1] != "")
    n_erows += (e_color_codes.size() + 1.3);
  if(cfg["EdgeLabelVariable"][1] != "") n_erows += 1.3;
  if(cfg["EdgeWidthVariable"][1] != "") n_erows += 1.3;
  if(cfg["VertexColorVariable"][1] != "")
    n_vrows += (v_color_codes.size() + 1.3);
  if(cfg["VertexLabelVariable"][1] != "") n_vrows += 1.3;
  if(cfg["VertexPatternVariable"][1] != "")
    n_vrows += (v_pattern_codes.size() + 1.3);
  if(cfg["VertexShapeVariable"][1] != "")
    n_vrows += (v_shape_codes.size() + 1.3);
  if(cfg["VertexSizeVariable"][1] != "") n_vrows += 1.3;
  
  /* Check dynamic range. */
  vector<double> lim = get_limits(components);
  if(decoration_mode && (lim[1] > lim[0])) n_erows += 3.3;
  if((n_erows < 1) && (n_vrows < 1)) return false;

  /* Determine legend size. */
  char buffer[64];
  double xmax = 12.3*r;
  double ymax = (n_erows + n_vrows + 2)*r;
  sprintf(buffer, "%.2f,%.2f", xmax, ymax);
  art.assign("PageSize", string(buffer));

  /* Starting point. */
  double x = r;
  double y = 1.5*r;

  /* Print colorbar. */
  if(decoration_mode && (lim[1] > lim[0])) {
    x = r; 
    art.text(x, y, "Automatic coloring:", txtsty);
    sty.strokeopacity = 0.0;
    sty.fillopacity = 1.0;
    x += 0.3*r;
    for(i = 0; i < 20; i++) {
      unsigned int clcode = colormap(DEFAULT_COLORMAP, (i + 0.5)/20);
      sty.fillcolor = color2rgb(clcode);
      art.shape(x, (y + 0.75*r), r/2.1, "square", sty);
      x += r/2;
    }

    /* Colorbar labels. */
    Style lbsty = txtsty;
    lbsty.fontsize *= 0.8;
    lbsty.anchor = "start";
    x = r; y += 1.7*r;
    if(cfg["EdgeWeightTransform"][1] == "off") {
      art.text(x, y, "0.0", lbsty);
      lbsty.anchor = "end"; x += 10.13*r;
      art.text(x, y, "1.0", lbsty);
    }
    else {
      art.text(x, y, "min", lbsty);
      lbsty.anchor = "end"; x += 10.13*r;
      art.text(x, y, "max", lbsty);
    }
    y += 1.6*r;
  }

  /* Edge color info. */
  string heading = string2safe(cfg["EdgeColorVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Edge color: " + heading), txtsty);
    x = 2.5*r; y += r;
    sty.strokeopacity = 0.5;
    sty.fillopacity = 1.0;
    for(i = 0; i < e_color_labels.size(); i++) {
      string label = string2safe(e_color_labels[i], MAX_LABEL_LENGTH);
      sty.fillcolor = color2rgb(e_color_codes[i]);
      art.shape(x, (y - FONT_UNIT/2.8), r/2.5, "circle", sty);
      art.text((x + r), y, label, txtsty);
      y += r;
    }
    y += 0.3*r;
  }

  /* Edge label info. */
  heading = string2safe(cfg["EdgeLabelVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Edge label: " + heading), txtsty);
    y += 1.3*r;
  }

  /* Edge width info. */
  heading = string2safe(cfg["EdgeWidthVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Edge width: " + heading), txtsty);
    y += 1.3*r;
  }

  /* Vertex color info. */
  heading = string2safe(cfg["VertexColorVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Vertex color: " + heading), txtsty);
    x = 2.5*r; y += r;
    sty.strokeopacity = 0.5;
    sty.fillopacity = 1.0;
    for(i = 0; i < v_color_labels.size(); i++) {
      string label = string2safe(v_color_labels[i], MAX_LABEL_LENGTH);
      sty.fillcolor = color2rgb(v_color_codes[i]);
      art.shape(x, (y - FONT_UNIT/2.8), r/2.5, "circle", sty);
      art.text((x + r), y, label, txtsty);
      y += r;
    }
    y += 0.3*r;
  }

  /* Vertex pattern info. */
  heading = string2safe(cfg["VertexPatternVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Vertex pattern: " + heading), txtsty);
    x = 2.5*r; y += r;
    sty.strokeopacity = 0.5;
    sty.fillopacity = 0.0;
    for(i = 0; i < v_pattern_labels.size(); i++) {
      string label = string2safe(v_pattern_labels[i], MAX_LABEL_LENGTH);
      sty.fillpattern = v_pattern_codes[i];
      art.shape(x, (y - FONT_UNIT/2.8), r/2, "square", sty);
      art.text((x + r), y, label, txtsty);
      y += r;
    }
    sty.fillpattern = "";
    y += 0.3*r;
  }

  /* Vertex shape info. */
  heading = string2safe(cfg["VertexShapeVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Vertex shape: " + heading), txtsty);
    x = 2.5*r; y += r;
    sty.strokeopacity = 0.7;
    sty.fillopacity = 0.0;
    for(i = 0; i < v_shape_labels.size(); i++) {
      string label = string2safe(v_shape_labels[i], MAX_LABEL_LENGTH);
      art.shape(x, (y - FONT_UNIT/2.8), r/2.5, v_shape_codes[i], sty);
      art.text((x + r), y, label, txtsty);
      y += r;
    }
    y += 0.3*r;
  }

  /* Vertex size info. */
  heading = string2safe(cfg["VertexSizeVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    x = r;
    art.text(x, y, ("Vertex size: " + heading), txtsty);
    y += 1.3*r;
  }

  return true;
}

/*
 *
 */
static vector<double>
get_limits(vector<Component>& components) {
  unsigned int i, k;
  vector<double> lim(4);

  lim[0] = FLT_MAX;
  lim[1] = -FLT_MAX;
  lim[2] = FLT_MAX;
  lim[3] = -FLT_MAX;
  for(i = 0; i < components.size(); i++) {
    vector<Edge> edges = components[i].edges();
    for(k = 0; k < edges.size(); k++) {
      if(lim[0] > edges[k].weight) lim[0] = edges[k].weight;
      if(lim[1] < edges[k].weight) lim[1] = edges[k].weight;
    }
    map<string, Vertex>::iterator pos;
    map<string, Vertex> vertices = components[i].vertices();
    for(pos = vertices.begin(); pos != vertices.end(); pos++) {
      Vertex v = pos->second;
      double s = (v.strength_in + v.strength_out);
      if(lim[2] > s) lim[2] = s;
      if(lim[3] < s) lim[3] = s;
    }
  }

  return lim;
}

