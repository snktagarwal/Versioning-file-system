/* file: component.create.cpp
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

using namespace std;
using namespace himmeli;

class CompareComponent {
public:
  bool operator()(const Component& c1, const Component& c2) const {
    return (c1.cardinality() > c2.cardinality());
  };
};

static void  dfs(vector<int>*, vector<int>*, vector<vector<int> >*, int, int);
static void* init(vector<int>&, vector<vector<Link> >&,
                  vector<string>&, double);

/*
 *
 */
vector<Component>
Component::create(const std::vector<Edge>& graph, const double r_ch) {
  unsigned int i;
  vector<Component> components;
  
  /* Collect vertex names. */
  map<string, int> name2index;
  for(i = 0; i < graph.size(); i++) {
    if(graph[i].head == "") continue;
    if(graph[i].tail == "") continue;
    if(graph[i].head == graph[i].tail) continue;
    if(graph[i].weight < FLT_EPSILON) continue;
    name2index[graph[i].head] = -1;
    name2index[graph[i].tail] = -1;
  }
  if(name2index.size() < 1) return components;

  /* Enumerate vertices. */
  vector<string> names;
  map<string, int>::iterator pos;
  for(pos = name2index.begin(); pos != name2index.end(); pos++) {
    pos->second = names.size();
    names.push_back(pos->first);
  }

  /* Create adjacency matrix. */
  vector<vector<int> > adj(names.size(), vector<int>(0));
  vector<vector<Link> > inputs(names.size(), vector<Link>(0));
  for(i = 0; i < graph.size(); i++) {
    if(graph[i].head == "") continue;
    if(graph[i].tail == "") continue;
    if(graph[i].head == graph[i].tail) continue;
    if(graph[i].weight < FLT_EPSILON) continue;
    int head = name2index[graph[i].head];
    int tail = name2index[graph[i].tail];
    Link ln = {head, tail, graph[i].weight};
    adj[head].push_back(tail);
    adj[tail].push_back(head);
    inputs[head].push_back(ln);
  }

  /* Isolate connected subgraphs. */
  vector<int> locks(names.size(), -1);
  for(i = 0; i < names.size(); i++) {
    if(locks[i] >= 0) continue;
    vector<int> visits;
    dfs(&visits, &locks, &adj, i, 0);

    Component c;
    delete (ComponentObject*)(c.buffer);
    c.buffer = init(visits, inputs, names, r_ch);
    components.push_back(c);
  }
  sort(components.begin(), components.end(), CompareComponent());

  return components;
}

/*
 *
 */
static void
dfs(vector<int>* visits, vector<int>* locks, vector<vector<int> >* adj,
    int index, int depth) {

  if(depth++ >= MAX_RC_DEPTH) {
    fprintf(stderr, "ERROR! %s: Max recursion depth reached.\n", __FILE__);
    exit(1);
  }
  visits->push_back(index);
  (*locks)[index] = index; 

  /* Reserve free neighbors. */
  unsigned int i;
  vector<int>& neighbors = (*adj)[index];
  for(i = 0; i < neighbors.size(); i++) {
    int target = neighbors[i];
    if(target == index) continue;
    if((*locks)[target] >= 0) continue;
    (*locks)[target] = index; 
  }

  /* Descend to available neighbors. */
  for(i = 0; i < neighbors.size(); i++) {
    int target = neighbors[i];
    if(target == index) continue;
    if((*locks)[target] != index) continue;
    dfs(visits, locks, adj, target, depth);
  }
}

/*
 *
 */
static void*
init(vector<int>& subgraph, vector<vector<Link> >& inputs,
     vector<string>& names, double r_ch) {
  unsigned int i, k; 
  vector<Link> links;
 
  /* One component consumes all vertices. */
  if(subgraph.size() == names.size()) {
    for(i = 0; i < subgraph.size(); i++) {
      vector<Link>& edges = inputs[subgraph[i]];
      for(k = 0; k < edges.size(); k++)
	links.push_back(edges[k]);
    }
    return new ComponentObject(links, names, r_ch);
  } 
 
  /* Create new vertex indexing. */
  map<int, int> addr2index;
  vector<string> labels(subgraph.size());
  for(i = 0; i < subgraph.size(); i++) {
    addr2index[subgraph[i]] = i;
    labels[i] = names[subgraph[i]]; 
  }

  /* Create links. */
  for(i = 0; i < subgraph.size(); i++) {
    vector<Link>& edges = inputs[subgraph[i]];
    for(k = 0; k < edges.size(); k++) {
      Link ln = edges[k];
      ln.head = addr2index[ln.head];
      ln.tail = addr2index[ln.tail];
      links.push_back(ln);
    }
  }

  return new ComponentObject(links, labels, r_ch);
}
