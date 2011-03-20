/* file: graphobject.decorate.cpp
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

static map<string, Arc> get_arcs(vector<Edge>&, char, bool);
static map<string, Emblem> get_emblems(vector<Edge>&, bool, bool);

/*
 *
 */
bool
GraphObject::decorate() {
  unsigned int i, k;
  vector<Edge> edges;
  map<string, Table>::iterator tpos;

  /* Collect edges from components. */
  for(i = 0; i < components.size(); i++) {
    vector<Edge> links = components[i].edges();
    for(k = 0; k < links.size(); k++)
      edges.push_back(links[k]);
  }

  /* Automatic visualization attributes. */
  bool label_flag = false;
  char delim = delimiter;
  if(label_mode == "on") label_flag = true; 
  if(label_mode == "vertex") label_flag = true; 
  if(delim == '\0') delim = ' ';
  arcs = get_arcs(edges, delim, decoration_mode);
  emblems = get_emblems(edges, decoration_mode, label_flag);

  /* Update edge visualization from input file(s). */
  label_flag = false;
  if(label_mode == "on") label_flag = true; 
  if(label_mode == "edge") label_flag = true; 
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    Table& t = tpos->second;
    map<string, Locus>& loci = variables[tpos->first];
    int head_var = t.column(cfg["EdgeHeadVariable"][1]);
    int tail_var = t.column(cfg["EdgeTailVariable"][1]);
    int color_var = loci["EdgeColorVariable"].column;
    int label_var = loci["EdgeLabelVariable"].column;
    int weight_var = loci["EdgeWeightVariable"].column;
    int width_var = loci["EdgeWidthVariable"].column;
    if(head_var < 0) continue;
    if(tail_var < 0) continue;

    /* Read edge information. */
    char buf[64];
    for(i = 0; i < t.size(); i++) {
      Row r = t[i];
      if(r.rank() < 1) continue;
      if(r[tail_var].length() < 1) continue;
      if(r[head_var].length() < 1) continue;

      string key = (r[head_var] + delim + r[tail_var]);
      if(arcs.count(key) < 1) continue;

      Arc& arc = arcs[key];
      if(color_var >= 0) arc.color = (int)(r.number(color_var));
      if(weight_var >= 0) arc.weight = r.number(weight_var); 
      if((weight_var >= 0) && label_flag) {
	float w = r.number(weight_var);
	if(w < 10) sprintf(buf, "%.2f", w);
	else if(w < 100) sprintf(buf, "%.1f", w);
	else sprintf(buf, "%.0f", w);
	arc.label = string(buf);
      }
      if((label_var >= 0) && label_flag) arc.label = r[label_var];
      if(width_var >= 0) arc.width = r.number(width_var);
    }
  }

  /* Apply edge weight mask. */
  map<string, Arc>::iterator pos;
  vector<double> weights(arcs.size());
  for(i = 0, pos = arcs.begin(); pos != arcs.end(); pos++, i++)
    weights[i] = (pos->second).weight;
  normalize(weights, cfg["EdgeWeightTransform"][1], edge_weight_mask);
  for(i = 0, pos = arcs.begin(); pos != arcs.end(); pos++, i++)
    if(weights[i] <= 0.0) (pos->second).width = -1.0;

  /* Update vertex visualization from input file(s). */
  label_flag = false;
  if(label_mode == "on") label_flag = true; 
  if(label_mode == "vertex") label_flag = true;
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    Table& t = tpos->second;
    map<string, Locus>& loci = variables[tpos->first];
    int name_var = t.column(cfg["VertexNameVariable"][1]);
    int color_var = loci["VertexColorVariable"].column;
    int label_var = loci["VertexLabelVariable"].column;
    int shape_var = loci["VertexShapeVariable"].column;
    int pattern_var = loci["VertexPatternVariable"].column;
    int size_var = loci["VertexSizeVariable"].column;
    int x_var = loci["VertexXVariable"].column;
    int y_var = loci["VertexYVariable"].column;
    if(name_var < 0) continue;

    /* Read vertex information. */
    for(i = 0; i < t.size(); i++) {
      Row r = t[i];
      if(r.rank() < 1) continue;
      if(r[name_var].length() < 1) continue;
      if(emblems.count(r[name_var]) < 1) continue;

      Emblem& emblem = emblems[r[name_var]];
      if(color_var >= 0) emblem.color = (int)(r.number(color_var));
      if(size_var >= 0) emblem.radius = r.number(size_var);
      if((label_var >= 0) && label_flag) emblem.label = r[label_var];
      if(shape_var >= 0) emblem.shape = r[shape_var];
      if(pattern_var >= 0) emblem.pattern = r[pattern_var];
      
      vector<float> coord(0);
      if(x_var >= 0) coord.push_back(r.number(x_var));
      if(y_var >= 0) coord.push_back(r.number(y_var));
      if(coord.size() > 1) coordinates[r[name_var]] = coord;
    }
  }    

  return true;
}

/*
 *
 */
static map<string, Arc>
get_arcs(vector<Edge>& edges, char delim, bool deco) {
  unsigned int i;
  map<string, Arc> arcs;

  for(i = 0; i < edges.size(); i++) {
    string key = (edges[i].head + delim + edges[i].tail);
    Arc a = {-1, 1.0, 1.0, ""};
    a.color = colormap(DEFAULT_COLORMAP, 0.5); 
    if(deco) {
      a.width = pow(edges[i].weight, 0.25);
      a.color = colormap(DEFAULT_COLORMAP, edges[i].weight);
    }
    arcs[key] = a;
  }

  return arcs;
}

/*
 *
 */
static map<string, Emblem>
get_emblems(vector<Edge>& edges, bool deco, bool label_flag) {
  unsigned int i;
  map<string, Emblem> emblems;
  if(edges.size() < 1) return emblems;

  /* Default attributes. */
  for(i = 0; i < edges.size(); i++) {
    string head = edges[i].head;
    string tail = edges[i].tail;
    Emblem e = {-1, 1.0, "", "", DEFAULT_SHAPE};
    emblems[head] = e;
    emblems[tail] = e;
    if(!label_flag) continue;
    emblems[head].label = head;
    emblems[tail].label = tail;
  }
  if(!deco) return emblems;

  /* Collect emblem-specific statistics. */
  for(i = 0; i < edges.size(); i++) {
    string head = edges[i].head;
    string tail = edges[i].tail;
    Emblem eH = emblems[head];
    Emblem eT = emblems[tail];
    eH.color += 1;
    eT.color += 1;
    eH.radius += edges[i].weight;
    eT.radius += edges[i].weight;
    emblems[head] = eH;
    emblems[tail] = eT;
  }
  
  /* Compute statistics. */
  double c_max = 0.0;
  double s_max = -FLT_MAX;
  map<string, Emblem>::iterator pos;
  for(pos = emblems.begin(); pos != emblems.end(); pos++) {
    Emblem e = pos->second;
    if(e.color > c_max) c_max = e.color;
    if(e.radius > s_max) s_max = e.radius;
  }

  /* Use degree for size and strength for color. */
  double rmean = 0.0;
  for(pos = emblems.begin(); pos != emblems.end(); pos++) {
    Emblem e = pos->second;
    e.color = colormap(DEFAULT_COLORMAP, (e.color)/c_max);
    e.radius = pow(e.radius, 0.25);
    pos->second = e;
    rmean += e.radius;
  }

  /* Set mean radius to one. */
  rmean /= emblems.size();
  for(pos = emblems.begin(); pos != emblems.end(); pos++) {
    Emblem e = pos->second;
    e.radius /= rmean;
    pos->second = e;
  }

  return emblems;
}
