/* file: engine.cpp
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

#include "engine.h"

static bool  is_connector(const vector<Node>&, Bond&);
static float get_weight(const vector<Node>&, int, int);

/*
 *
 */
Engine::Engine(const vector<Node>& nodes, bool chassis_flag) {
  unsigned int i, k;
  if(nodes.size() < 2) return;

  /* Copy topology. */
  points = vector<Point>(nodes.size());
  for(i = 0; i < nodes.size(); i++) {
    points[i].x = (nodes[i].x)/DISTANCE_UNIT;
    points[i].y = (nodes[i].y)/DISTANCE_UNIT;
    points[i].power = 0.0;

    const vector<Link>& links_in = nodes[i].links_in;
    const vector<Link>& links_out = nodes[i].links_out;
    unsigned int n_in = links_in.size();
    unsigned int n_out = links_out.size();
    if(chassis_flag) {
      n_in = nodes[i].chassis_degree_in;
      n_out = nodes[i].chassis_degree_out;
    }

    for(k = 0; k < n_in; k++) {
      Bond b;
      b.head = links_in[k].head;
      b.tail = links_in[k].tail;
      b.weight = links_in[k].weight;
      if(b.weight > 1.0) b.weight = (1.0 + log(b.weight));
      bonds.push_back(b);
      points[i].power += links_in[k].weight;
    }
    for(k = 0; k < n_out; k++)
      points[i].power += links_out[k].weight;
  }

  /* Check links. */
  float w_min = 1.0;
  float w_max = 0.0;
  float w_ave = 0.0;
  int n = nodes.size();
  for(i = 0; i < bonds.size(); i++) {
    if((bonds[i].head < 0) || (bonds[i].head >= n)) {
      fprintf(stderr, "ERROR! %s: Invalid head at [%d].\n", __FILE__, i);
      exit(1);
    }
    if((bonds[i].tail < 0) || (bonds[i].tail >= n)) {
      fprintf(stderr, "ERROR! %s: Invalid tail at [%d].\n", __FILE__, i);
      exit(1);
    }
    if(bonds[i].weight < 0.0) {
      fprintf(stderr, "ERROR! %s: Negative link weight.\n", __FILE__);
      exit(1);
    }
    if(bonds[i].weight < w_min) w_min = bonds[i].weight; 
    if(bonds[i].weight > w_max) w_max = bonds[i].weight; 
    w_ave += bonds[i].weight;
  }
  w_ave /= (bonds.size() + 1);

  /* Boost connector links. */
  float w_limit = (0.5*w_min + 0.3*w_ave + 0.2*w_max - 0.01);
  if(w_limit > w_min) { 
    for(i = 0; i < bonds.size(); i++) {
      if(bonds[i].weight >= w_limit) continue;
      if(!is_connector(nodes, bonds[i])) continue;
      bonds[i].weight = (0.3*w_min + 0.4*w_ave + 0.3*w_max);
    }
  }
}

/*
 *
 */
static bool
is_connector(const vector<Node>& nodes, Bond& b) {
  unsigned int k;
  const Node& ndH = nodes[b.head];
  const Node& ndT = nodes[b.tail];

  /* Check neighbors of head vertex. */
  const vector<Link>& linksH_in = ndH.links_in;
  for(k = 0; k < linksH_in.size(); k++) {
    float w = get_weight(nodes, b.tail, linksH_in[k].tail);
    if(w > b.weight) return false;
  }
  const vector<Link>& linksH_out = ndH.links_out;
  for(k = 0; k < linksH_out.size(); k++) {
    float w = get_weight(nodes, b.tail, linksH_out[k].head);
    if(w > b.weight) return false;
  }

  /* Check neighbors of tail vertex. */
  const vector<Link>& linksT_in = ndT.links_in;
  for(k = 0; k < linksT_in.size(); k++) {
    float w = get_weight(nodes, b.head, linksT_in[k].tail);
    if(w > b.weight) return false;
  }
  const vector<Link>& linksT_out = ndT.links_out;
  for(k = 0; k < linksT_out.size(); k++) {
    float w = get_weight(nodes, b.head, linksT_out[k].head);
    if(w > b.weight) return false;
  }

  return true;
}

/*
 *
 */
static float
get_weight(const vector<Node>& nodes, int ind1, int ind2) {
  unsigned int k;
  float weight = 0.0;
  const vector<Link>& links1 = nodes[ind1].links_in;
  const vector<Link>& links2 = nodes[ind2].links_in;

  for(k = 0; k < links1.size(); k++) {
    int index = links1[k].tail;
    if(index == ind2) {
      weight += links1[k].weight;
      break;
    }
  }
  for(k = 0; k < links2.size(); k++) {
    int index = links2[k].tail;
    if(index == ind1) {
      weight += links2[k].weight;
      break;
    }
  }

  return weight;
}
