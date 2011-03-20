/* file: componentobject_methods.cpp
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

#include "componentobject.h"

/*
 *
 */
bool
ComponentObject::assign(const string& s, const string& value) {
  if(value == "") return false;
  if(parameters.count(s) < 1) return false;
  parameters[s] = value;
  return true;
}

/*
 *
 */
vector<Edge>
ComponentObject::chassis() const {
  unsigned int i, k;
  vector<Edge> edges;
  Edge e;

  for(i = 0; i < nodes.size(); i++) {
    const vector<Link>& links_in = nodes[i].links_in;
    for(k = 0; k < nodes[i].chassis_degree_in; k++) {
      int head = links_in[k].head;
      int tail = links_in[k].tail;
      e.head = nodes[head].name;
      e.tail = nodes[tail].name;
      e.weight = links_in[k].weight;
      edges.push_back(e);
    }
  }

  return edges;
}

/*
 *
 */
vector<Edge>
ComponentObject::edges() const {
  unsigned int i, k;
  vector<Edge> edges;
  Edge e;

  for(i = 0; i < nodes.size(); i++) {
    const vector<Link>& links_in = nodes[i].links_in;
    for(k = 0; k < links_in.size(); k++) {
      int head = links_in[k].head;
      int tail = links_in[k].tail;
      e.head = nodes[head].name;
      e.tail = nodes[tail].name;
      e.weight = links_in[k].weight;
      edges.push_back(e);
    }
  }

  return edges;
}

/*
 *
 */
void
ComponentObject::update(const map<string, vector<float> >& coord) {
  unsigned int i;
  if(coord.size() < 1) return;
  for(i = 0; i < nodes.size(); i++) {
    if(coord.count(nodes[i].name) < 1) continue;
    map<string, vector<float> >::const_iterator iter;
    iter = coord.find(nodes[i].name);
    vector<float> p = iter->second;
    if(p.size() < 1) continue;
    nodes[i].x = p[0]; 
    if(p.size() < 2) continue;
    nodes[i].y = p[1]; 
    if(p.size() < 3) continue;
    nodes[i].z = p[2]; 
  }
}

/*
 *
 */
map<string, Vertex>
ComponentObject::vertices() const {
  unsigned int i, k;
  map<string, Vertex> vertices;

  for(i = 0; i < nodes.size(); i++) {
    const vector<Link>& links_in = nodes[i].links_in;
    const vector<Link>& links_out = nodes[i].links_out;

    Vertex v;
    v.degree_in = links_in.size();
    v.degree_out = links_out.size();
    v.strength_in = 0.0;
    for(k = 0; k < v.degree_in; k++)
      v.strength_in += links_in[k].weight;
    v.strength_out = 0.0;
    for(k = 0; k < v.degree_out; k++)
      v.strength_out += links_out[k].weight;
    v.x = nodes[i].x;
    v.y = nodes[i].y;
    v.z = nodes[i].z;
    vertices[nodes[i].name] = v;
  }

  return vertices;
}
