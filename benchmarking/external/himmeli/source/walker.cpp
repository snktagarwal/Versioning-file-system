/* file: walker.cpp
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

#include "walkertools.h"

#define VERTEX_CAP 1000000

/*
 * Walker II's algorithm.
 */
void
walker(walker_vertex_t* vertices, int n) {
  int i, k;
  int first, last, left, left_sib;
  size_t size;
  float height = 1.0;
  char* buf = NULL;
  int* node2vtx;
  node_t* nodes;
  tree_t tree[1];

  if(vertices == NULL) return;
  if(n < 1) return;
  if(n >= VERTEX_CAP) {
    printf("WARNING! %s: Vertex capacity exceeded.\n", __FILE__);
    printf("         %s: The first %d vertices imported successfully.\n",
	   __FILE__, VERTEX_CAP);
    n = VERTEX_CAP;
  }

  /* Allocate local buffer. */
  size = (n + 1)*(2*sizeof(int) + sizeof(node_t) + sizeof(node_t*));
  buf = (char*)malloc(size);
  tree->vtx2node = (int*)buf;
  tree->node2vtx = (int*)(tree->vtx2node + n + 1);
  tree->vertices = vertices;
  tree->nodes = (node_t*)(tree->node2vtx + n + 1);
  tree->edge = (node_t**)(tree->nodes + n + 1);
  for(i = 0; i <= n; i++) {
    tree->vtx2node[i] = -1;
    tree->node2vtx[i] = -1;
    tree->edge[i] = NULL;
  }

  /* Sort nodes according to their position in the tree and
     add links according to vertices. */
  tree->n = 0;
  tree->cap = n;
  tree->gen = 0;
  tree->depth = 0;
  for(i = 0; i < n; i++) {
    if(vertices[i].height > height) height = vertices[i].height;
    if(vertices[i].parent >= 0) continue;
    if(tree->depth > 0) {
      fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
      fprintf(stderr, "Second root at [%d].\n", i);
      exit(1);
    }
    walker_dfs(tree, i);
    walker_branch(tree, i);
  }

  /* Check that tree is connected. */
  if(tree->n == 0) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "No root.\n");
    exit(1);
  }
  if(tree->n != n) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Graph is not connected.\n");
    exit(1);
  }

  /* First part of the tree algorithm. */
  nodes = tree->nodes;
  node2vtx = tree->node2vtx;
  for(k = 0; k < tree->n; k++) {
    first = nodes[k].first_child;
    last = nodes[k].last_child;
    left = nodes[k].left_peer;

    /* Check left sibling. */
    left_sib = -1;
    if(left >= 0)
      if(nodes[left].sibling == k)
	left_sib = left;

    /* Find the children to center above. */
    while(first >= 0) {
      i = node2vtx[first];
      if(vertices[i].is_centering) break;
      if((first = nodes[first].sibling) < 0) {
	fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
	fprintf(stderr, "Parent at [%d] cannot be centered.\n",
		tree->node2vtx[k]);
	exit(1);
      }
    }
    while(last >= 0) {
      i = node2vtx[last];
      if(vertices[i].is_centering) break;
      last = nodes[last].left_peer;
    }

    /* Default position. */
    if(left_sib >= 0) {
      nodes[k].x = nodes[left_sib].x;
      nodes[k].x += nodes[left_sib].width; 
    }

    /* Center above children and compute the distance to
       left sibling branches. */
    if(first >= 0) {
      i = node2vtx[k];
      if(left_sib < 0) {
	nodes[k].x += (nodes[first].x + nodes[last].x)/2.0;
	nodes[k].x -= vertices[i].down_attach;
	first = node2vtx[first];
	last = node2vtx[last];
	nodes[k].x += (vertices[first].up_attach)/2.0;
	nodes[k].x += (vertices[last].up_attach)/2.0;
      }
      else {
	nodes[k].xmod = (nodes[k].x + vertices[i].down_attach);
	nodes[k].xmod -= (nodes[first].x + nodes[last].x)/2.0;
	first = node2vtx[first];
	last = node2vtx[last];
	nodes[k].xmod -= (vertices[first].up_attach)/2.0;
	nodes[k].xmod -= (vertices[last].up_attach)/2.0;
      }
      
      /* Eliminate overlap with the left subtrees. */
      first = walker_trace(tree, k, k);
      while((first < k) && (first >= 0)) {
	walker_measure(tree, first, k);
	first = walker_trace(tree, first, k);
      }
    }

    /* Balance subtrees. */
    if((left_sib >= 0) && (nodes[k].sibling < 0))
      walker_balance(tree, k);
  }
  
  /* Second part of the tree algorithm. */
  tree->modifier = 0.0;
  nodes = tree->nodes;
  for(k = 0; k < tree->n; k++) {
    if(nodes[k].parent >= 0) continue;
    walker_fix(tree, k);
  }

  /* Copy coordinates. */
  node2vtx = tree->node2vtx;
  nodes = tree->nodes;
  for(k = 0; k < tree->n; k++) {
    i = node2vtx[k];
    vertices[i].x = nodes[k].x;
    vertices[i].y = height*(tree->depth - nodes[k].gen);
  }

  /* Free local buffer. */
  free(buf);
}
