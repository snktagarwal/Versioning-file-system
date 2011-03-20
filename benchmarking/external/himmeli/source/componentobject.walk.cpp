/* file: componentobject.walk.cpp
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
#include "walker.h"

static void dfs(walker_vertex_t*, vector<Node>*, int, int);

/*
 *
 */
void
ComponentObject::walk() {
  unsigned int i;
  unsigned int n_nodes = nodes.size();
  if(n_nodes < 2) return;
  if(center < 0) return;
  
  /* Create tree structure and run Walker's algorithm. */
  walker_vertex_t vertices[n_nodes];
  for(i = 0; i < n_nodes; i++) {
    vertices[i].parent = -1;
    vertices[i].child = -1;
    vertices[i].sibling = -1;
    vertices[i].is_centering = 1;
    vertices[i].x = 0.0;
    vertices[i].y = 0.0;
    vertices[i].width = 2.0;
    vertices[i].height = 2.0;
    vertices[i].up_attach = 0.0;
    vertices[i].down_attach = 0.0;
    vertices[i].user_data = NULL;
  }
  dfs(vertices, &nodes, center, 0);
  walker(vertices, n_nodes);

  /* Copy coordinates and find dimensions. */
  float x_min = FLT_MAX;
  float x_max = -FLT_MAX;
  for(i = 0; i < n_nodes; i++) {
    nodes[i].x = vertices[i].x;
    nodes[i].y = vertices[i].y;
    if(nodes[i].x < x_min) x_min = nodes[i].x;
    if(nodes[i].x > x_max) x_max = nodes[i].x;
  }

  /* Warp node positions. */
  float x0 = nodes[center].x;
  float y0 = nodes[center].y;
  float x_arc = (x_max - x_min + 1.0)/2/M_PI;
  float p[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
  for(i = 0; i < n_nodes; i++) {
    float x = (nodes[i].x - x0);
    float y = (nodes[i].y - y0);
    float r = (1.0 + fabs(y));
    float phi = (x_max - x)/x_arc;
    
    r *= log(r + x_arc);
    x = (x0 + r*sin(phi));
    y = (y0 + r*cos(phi));
    if((nodes[i].x = x) < p[0]) p[0] = x;
    if((nodes[i].y = y) < p[1]) p[1] = y;
    nodes[i].z += (i%2)*r;
    nodes[i].z -= ((i + 1)%2)*r;
    if(nodes[i].z < p[2]) p[2] = nodes[i].z;
  }

  /* Remove offsets. */
  for(i = 0; i < n_nodes; i++) {
    nodes[i].x -= p[0];
    nodes[i].y -= p[1];
    nodes[i].z -= p[2];
  }
}

/*
 * Depth-first branching.
 */
static void
dfs(walker_vertex_t* vertices, vector<Node>* nodes, int index, int depth) {

  if(depth++ >= MAX_RC_DEPTH) {
    fprintf(stderr, "ERROR! %s: Max recursion depth reached.\n", __FILE__);
    exit(1);
  }
  vertices[index].y = (depth + 0.5);
  vertices[index].width = 1.0/sqrt(depth + 1.0);

  /* Update neighbors. */
  unsigned int i;
  vector<Link>* links = &((*nodes)[index].links_in);
  for(i = 0; i < (*nodes)[index].tree_degree_in; i++) {
    int child = (*links)[i].tail;
    if(vertices[child].y > 0.0) continue;
    vertices[child].parent = index;
    vertices[child].sibling = vertices[index].child;
    vertices[index].child = child;
  }
  links = &((*nodes)[index].links_out);
  for(i = 0; i < (*nodes)[index].tree_degree_out; i++) {
    int child = (*links)[i].head;
    if(vertices[child].y > 0.0) continue;
    vertices[child].parent = index;
    vertices[child].sibling = vertices[index].child;
    vertices[index].child = child;
  }

  /* Branch to children. */
  links = &((*nodes)[index].links_in);
  for(i = 0; i < (*nodes)[index].tree_degree_in; i++) {
    int child = (*links)[i].tail;
    if(vertices[child].y > 0.0) continue;
    dfs(vertices, nodes, child, depth);
  }
  links = &((*nodes)[index].links_out);
  for(i = 0; i < (*nodes)[index].tree_degree_out; i++) {
    int child = (*links)[i].head;
    if(vertices[child].y > 0.0) continue;
    dfs(vertices, nodes, child, depth);
  }
}

