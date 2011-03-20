/* file: componentobject.cpp
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

#define ROOT     'R'
#define NORMAL   'N'
#define LINK     'L'
#define REJECTED 'E'

using namespace std;
using namespace himmeli;

class CompareLink {
public:
  bool operator()(const Link& l1, const Link& l2) const {
    return (l1.weight > l2.weight);
  };
};

static int  kruskal(vector<Link>&, vector<Link>&, unsigned int);
static void kruskal_connect(vector<Link>&, unsigned int);
static int  kruskal_join(int, int, vector<int>&, int, int);

/*
 *
 */
ComponentObject::ComponentObject(const ComponentObject* co) {
  center = co->center;
  radius = co->radius;
  n_simu = co->n_simu;
  n_chassis = co->n_chassis;
  nodes = co->nodes;
  parameters = co->parameters;
}

/*
 *
 */
ComponentObject::ComponentObject(const vector<Link>& topology, 
				 const vector<string>& names,
				 const double r_ch) {
  unsigned int i;

  /* Private variables. */
  center = -1;
  radius = -2;
  n_simu = 0;
  n_chassis = -1;

  /* Default parameters. */
  parameters["ChassisMode"] = "off";
  parameters["VerboseMode"] = "on";

  /* Construct spanning tree. */
  vector<Link> links;
  vector<Link> pool = topology;
  unsigned int n_trunk = kruskal(links, pool, names.size());
  if(n_trunk == 0) {  
    fprintf(stderr, "ERROR! %s: Graph is not connected.\n", __FILE__);
    exit(1);
  }

  /* Check chassis size. */
  n_chassis = (int)(r_ch*n_trunk + 0.5);
  if(n_chassis < (int)n_trunk) n_chassis = (int)n_trunk;
  if(n_chassis > (int)(topology.size()))
    n_chassis = (int)(topology.size());

   /* Construct new trees until chassis is complete. */
  while(pool.size() > 0) {
    kruskal(links, pool, names.size());
    if((int)(links.size()) > n_chassis) {
      sort(pool.begin(), pool.end(), CompareLink());
      for(i = 0; i < pool.size(); i++)
	links.push_back(pool[i]);
      break;
    }
  }

  /* Create nodes. */
  for(i = 0; i < names.size(); i++) {
    Node nd;
    nd.tree_degree_in = 0;
    nd.tree_degree_out = 0;
    nd.chassis_degree_in = 0;
    nd.chassis_degree_out = 0;
    nd.x = 0.0;
    nd.y = 0.0;
    nd.z = 0.0;
    nd.name = names[i];
    nd.links_in = vector<Link>(0);
    nd.links_out = vector<Link>(0);
    nodes.push_back(nd);
  }

  /* Update edge lists. */
  for(i = 0; i < links.size(); i++) {
    int head = links[i].head;
    int tail = links[i].tail;
    (nodes[head].links_in).push_back(links[i]);
    (nodes[tail].links_out).push_back(links[i]);
    if((i < n_trunk) || ((int)i < n_chassis)) {
      nodes[head].chassis_degree_in += 1;
      nodes[tail].chassis_degree_out += 1;
    }
    if(i < n_trunk) {
      nodes[head].tree_degree_in += 1;
      nodes[tail].tree_degree_out += 1;
    }
  }

  /* Store node degrees. */
  vector<int> radii(nodes.size(), 0);
  vector<int> array(nodes.size(), 0);
  for(i = 0; i < nodes.size(); i++)
    array[i] = (nodes[i].tree_degree_in + nodes[i].tree_degree_out);

  /* Iteratively tag links that have a leaf terminus until 
     a single link remains. */
  radius = 1;
  Link ln = links[0];
  while(n_trunk > 2) {
    unsigned int n = 0;
    for(i = 0; i < n_trunk; i++) {
      int head = links[i].head;
      int tail = links[i].tail;
      if((array[head] <= 1) || (array[tail] <= 1)) continue;
      radii[head] = radius;
      radii[tail] = radius;
      ln = links[i];
      links[i] = links[n];
      links[n++] = ln;
    }
    
    for(i = n; i < n_trunk; i++) {
      array[links[i].head] -= 1;
      array[links[i].tail] -= 1;
    }
    n_trunk = n;
    radius++;
  }

  /* Select center vertex. */
  if(n_trunk == 1) {
    if(radii[ln.head] > radii[ln.tail]) center = ln.head;
    else center = ln.tail;  
  }
  else {
    if((links[0].head == links[1].head) ||
       (links[0].head == links[1].tail))
      center = links[1].head;
    else
      center = links[1].tail;
  }
}

/*
 *
 */
static int
kruskal(vector<Link>& links, vector<Link>& pool, unsigned int n_nodes) {
  unsigned int i;

  /* Ensure connectivity and sort edges by weight. */
  vector<Link> sorted = pool;
  kruskal_connect(sorted, n_nodes);
  sort(sorted.begin(), sorted.end(), CompareLink());

  /* Create index maps. */
  vector<int> array(n_nodes);
  vector<int> forest(n_nodes, -1);
  vector<int> tags((sorted.size() + 1), 0); 
  for(i = 0; i < array.size(); i++)
    array[i] = i;

  /* Kruskal's algorithm. */
  unsigned int n_roots = array.size();
  unsigned int n_sorted = sorted.size();
  while(n_sorted > 0) {
    unsigned int n = 0;
    unsigned int n_trees = 0;

    for(i = 0; i < n_sorted; i++) {
      int head = array[sorted[i].head];
      int tail = array[sorted[i].tail];

      /* Move connectors to front. */
      tags[i] = kruskal_join(head, tail, forest, n_roots, n_trees);
      if(tags[i] == ROOT) n_trees++;
      if(tags[i] == LINK) {
	Link tmp = sorted[n];
	sorted[n] = sorted[i];
	sorted[i] = tmp;
	tags[i] = tags[n];
	tags[n] = LINK; 
	n++;
	continue;
      }
    }
    n_sorted = n;
    
    /* Update mapping and reset forest. */
    for(i = 0; i < array.size(); i++)
      array[i] = forest[array[i]];
    for(i = 0; i < n_trees; i++)
      forest[i] = -1;
    n_roots = n_trees;
  }
  
  /* Reorganize edges. Note that dummy edges are ignored. */
  pool.clear();
  unsigned int n_trunk = 0;
  for(i = 0; i < sorted.size(); i++) {
    if(sorted[i].weight <= 0.0) continue;
    if(tags[i] == REJECTED) {
      pool.push_back(sorted[i]);
      continue;
    }
    links.push_back(sorted[i]);
    n_trunk++;
  }

  /* Check if graph is connected. */
  if(n_trunk < (n_nodes - 1)) return 0;
  return n_trunk;
}

/*
 *
 */
static void
kruskal_connect(vector<Link>& pool, unsigned int n_nodes) {
  unsigned int i;
  for(i = 1; i < n_nodes; i++) {
    Link e;
    e.head = 0;
    e.tail = i;
    e.weight = 0.0;
    pool.push_back(e);
  }
}

/*
 *
 */
static int
kruskal_join(int head, int tail, vector<int>& forest, int n_forest,
	     int layer) {
  if(head < 0) return -1;
  if(tail < 0) return -1;
  if(head >= n_forest) return -1;
  if(tail >= n_forest) return -1;
  if((forest[head] < 0) && (forest[tail] < 0)) {
    forest[head] = layer;
    forest[tail] = layer;
    return ROOT;
  }
  if(forest[head] < 0) {
    forest[head] = forest[tail];
    return NORMAL;
  }
  if(forest[tail] < 0) {
    forest[tail] = forest[head];
    return NORMAL;
  }
  if(forest[head] != forest[tail])
    return LINK;
  return REJECTED;
}
