/* file: walkertools.cpp
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

#define MAX_DEPTH 10000

static void check(tree_t*, int);

/*
 * Reset horizontal and compute vertical positions.
 * The input index refers to the vertex array.
 */
void
walker_dfs(tree_t* tree, int i) {
  int k = tree->n;
  int alpha = -1;
  int* vtx2node = tree->vtx2node;
  int* node2vtx = tree->node2vtx;
  walker_vertex_t* vertices = tree->vertices;  
  node_t* nodes = tree->nodes;

  /* Check vertex data. */
  if(i < 0) return;
  check(tree, i);
  if(tree->gen > tree->depth)
    tree->depth = tree->gen;

  /* Visit children. */
  tree->gen += 1;
  walker_dfs(tree, vertices[i].child);
  tree->gen -= 1;
  k = tree->n;
  
  /* Add new node. */
  nodes[k].parent = -1;
  nodes[k].first_child = -1;
  nodes[k].last_child = -1;
  nodes[k].sibling = -1;
  nodes[k].left_peer = -1;
  nodes[k].right_peer = -1;
  nodes[k].contour = -1;
  nodes[k].gen = tree->gen;
  nodes[k].n_left_sibs = 0;
  nodes[k].x = 0.0;
  nodes[k].xmod = 0.0;
  nodes[k].xshift = 0.0;
  nodes[k].xchange = 0.0;
  nodes[k].width = vertices[i].width;
  vtx2node[i] = k;
  node2vtx[k] = i;
  tree->n += 1;

  /* Visit siblings if first-born. */
  if((alpha = vertices[i].parent) < 0) return;
  if(vertices[alpha].child != i) return;
  while((i = vertices[i].sibling) >= 0)
    walker_dfs(tree, i);
}

/*
 * Return the farthest sibling vertex.
 * The input and output indices refer to the vertex array.
 */
int
walker_branch(tree_t* tree, int i) {
  int k;
  int alpha, beta;
  int* vtx2node = tree->vtx2node;
  walker_vertex_t* vertices = tree->vertices;  
  node_t* nodes = tree->nodes;
  k = vtx2node[i];

  /* Set parent. */
  if((alpha = vertices[i].parent) >= 0)
    nodes[k].parent = vtx2node[alpha];
  
  /* Update edge. */
  alpha = k;
  beta = nodes[k].gen;
  if(tree->edge[beta] != NULL) {
    alpha = (int)(tree->edge[beta] - nodes);
    nodes[k].left_peer = alpha;
    nodes[alpha].right_peer = k;
  }
  tree->edge[beta] = (node_t*)(nodes + k);

  /* Visit children. */
  beta = -1;
  if((alpha = vertices[i].child) >= 0)
    beta = walker_branch(tree, alpha);
  if(alpha >= 0) nodes[k].first_child = vtx2node[alpha];
  if(beta >= 0) nodes[k].last_child = vtx2node[beta];

  /* Visit siblings and count subtrees. */
  beta = i;
  if((alpha = vertices[i].parent) < 0) return beta;
  if(vertices[alpha].child != i) return beta;
  while((alpha = vertices[beta].sibling) >= 0) {
    k = vtx2node[alpha];
    beta = vtx2node[beta];
    nodes[beta].sibling = k;
    nodes[k].n_left_sibs = (nodes[beta].n_left_sibs + 1);
    beta = walker_branch(tree, alpha);
  }

  return beta;
}

/*
 * Find left contour thread.
 * The input index refers to the node array.
 */
int
walker_trace(tree_t* tree, int k, int apex) {
  int alpha = -1;
  int beta = -1;
  int gen = -1;
  node_t* nodes = tree->nodes;
  node_t** edge = tree->edge;

  /* Check existing contour. */
  if((alpha = nodes[k].contour) >= 0)
    return alpha;

  /* Check child. */
  gen = (nodes[k].gen + 1);
  if((alpha = nodes[k].first_child) >= 0) {
    edge[gen] = (node_t*)(nodes + alpha);
    nodes[k].contour = alpha;
    return alpha;
  }

  /* Check edge. */
  if(edge[gen] == NULL)
    return -1;

  /* Trace left from the edge if the left edge peer belongs to the
     same subtree. */
  alpha = (int)(edge[gen] - nodes);
  beta = nodes[apex].left_peer;
  while((alpha > beta) && (alpha >= 0))
    alpha = nodes[alpha].left_peer;

  /* Trace right from the edge until current subtree.
     The contour can also point to the next subtree on the right
     if the current subtree is not deep enough. */
  while((alpha < beta) && (alpha >= 0)) {
    edge[gen] = (node_t*)(nodes + alpha);
    alpha = nodes[alpha].right_peer;
  }
  nodes[k].contour = alpha;

  return alpha;
}

/*
 * Compute distance to left peer. If the closest mutual
 * ancestor is above the current apex, return FLT_MAX.
 * The input index refers to the node array.
 */
void
walker_measure(tree_t* tree, int k, int apex) {
  int j;
  int peer = k;
  int alpha = -1;
  float dx = 0.0;
  float lmod = 0.0;
  float rmod = 0.0;
  node_t* nodes = tree->nodes;

  if((peer = nodes[k].left_peer) < 0) return;

  /* Track left apex and update level modifier. */
  alpha = -1;
  j = nodes[peer].parent;
  while((j >= 0) && (j < apex)) {
    lmod += nodes[j].xmod;
    alpha = j;
    j = nodes[j].parent;
  }
  
  /* Check that left subtree is sibling. */
  if(alpha < 0) return;
  if(nodes[alpha].parent != nodes[apex].parent) return;

  /* Track right modifier path. */
  j = nodes[k].parent;
  while((j >= 0) && (j <= apex)) {
    rmod += nodes[j].xmod;
    j = nodes[j].parent;
  }
  
  /* Compute distance. */
  dx = (nodes[k].x - nodes[peer].x - nodes[peer].width);
  dx += (rmod - lmod);

  /* Adjust subtrees. */
  if(dx < 0) {
    k = (nodes[apex].n_left_sibs - nodes[alpha].n_left_sibs);
    nodes[apex].x -= dx;
    nodes[apex].xmod -= dx;
    nodes[apex].xshift -= dx;
    nodes[apex].xchange += dx/k;
    nodes[alpha].xchange -= dx/k; 
  }
}

/*
 * Distribute middle subtrees evenly between the first and last subtrees.
 * The input indices refer to the node array.
 */
void
walker_balance(tree_t* tree, int apex) {
  int k = -1;
  int parent = -1;
  float xA, xB, cA, cB;
  node_t* nodes = tree->nodes;

  xA = nodes[apex].xshift;
  cA = nodes[apex].xchange;	
  xB = 0.0;
  cB = nodes[apex].xchange;
  parent = nodes[apex].parent;

  k = apex;
  while((k = nodes[k].left_peer) >= 0) {
    if(nodes[k].parent != parent) break;
    cA = nodes[k].xchange;
    xB = (xA + xB + cB);
    cB = (cA + cB);
    xA = nodes[k].xshift;
    nodes[k].x += xB;    
    nodes[k].xmod += xB;
  }
}

/*
 * Apply modifiers to the preliminary positions.
 * The input index refers to the node array.
 */
void
walker_fix(tree_t* tree, int k) {
  int alpha = -1;
  node_t* nodes = tree->nodes;

  nodes[k].x += tree->modifier;
  if((alpha = nodes[k].first_child) >= 0) {
    tree->modifier += nodes[k].xmod;
    walker_fix(tree, alpha);
    tree->modifier -= nodes[k].xmod;
  }

  if((alpha = nodes[k].parent) < 0) return;
  if(nodes[alpha].first_child != k) return;
  alpha = k;
  while((alpha = nodes[alpha].sibling) >= 0)
    walker_fix(tree, alpha);
}

/*
 * The input index refers to the vertex array.
 */
static void
check(tree_t* tree, int i) {
  int alpha = -1;
  int* vtx2node = tree->vtx2node;
  walker_vertex_t* vertices = tree->vertices;

  /* Check recursion. */
  if(tree->gen >= MAX_DEPTH) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Maximum recursion depth exceeded at [%d].\n", i);
    exit(1);
  }

  /* Check parent. */
  if((alpha = vertices[i].parent) >= tree->cap) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Invalid index at [%d]->parent.\n", i);
    exit(1);
  }
  if(alpha == i) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Parent to itself at [%d].\n", i);
    exit(1);
  }
  if(alpha >= 0)
    if(vtx2node[alpha] >= 0) {
      fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
      fprintf(stderr, "Cycle detected at [%d]->parent.\n", i);
      exit(1);
    }
  
  /* Check child. */
  if((alpha = vertices[i].child) >= tree->cap) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Invalid index at [%d]->child.\n", i);
    exit(1);
  }
  if(alpha == i) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Child to itself at [%d].\n", i);
    exit(1);
  }
  if(alpha >= 0)
    if(vtx2node[alpha] >= 0) {
      fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
      fprintf(stderr, "Cycle detected at [%d]->child.\n", i);
      exit(1);
    }

  /* Check sibling. */
  if((alpha = vertices[i].sibling) >= tree->cap) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Invalid index at [%d]->sibling.\n", i);
    exit(1);
  }
  if(alpha == i) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Sibling to itself at [%d].\n", i);
    exit(1);
  }
  if(alpha >= 0)
    if(vtx2node[alpha] >= 0) {
      fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
      fprintf(stderr, "Cycle detected at [%d]->sibling.\n", i);
      exit(1);
    }

  /* Check width. */
  if(vertices[i].width < 0.0) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Negative width at [%d].\n", i);
    exit(1);
  }
  if(vertices[i].up_attach < 0.0) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Negative up attachment at [%d].\n", i);
    exit(1);
  }
  if(vertices[i].width < vertices[i].up_attach) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Up attachment out of bounds at [%d].\n", i);
    exit(1);
  }
  if(vertices[i].down_attach < 0.0) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Negative down attachment at [%d].\n", i);
    exit(1);
  }
  if(vertices[i].width < vertices[i].down_attach) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Down attachment out of bounds at [%d].\n", i);
    exit(1);
  }
}
