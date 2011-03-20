/* file: engine_methods.cpp
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

/*
 *
 */
vector<float>
Engine::operator[](const int i) const {
  vector<float> coord(0);
  if(i < 0) return coord;
  if((unsigned int)i >= points.size()) return coord;
  coord.push_back(DISTANCE_UNIT*(points[i].x));
  coord.push_back(DISTANCE_UNIT*(points[i].y));
  return coord;
}

/*
 *
 */
bool
Engine::assign(const string& s, const string& value) {
  if(value == "") return false;
  if(parameters.count(s) < 1) return false;
  parameters[s] = value;
  return true;
}

/*
 *
 */
void
Engine::attract(vector<Point>& g, const vector<Bond>& bonds) {
  unsigned int i;
  for(i = 0; i < bonds.size(); i++) {
    int head = bonds[i].head;
    int tail = bonds[i].tail;
    float amp = bonds[i].weight;
    float dx = (points[head].x - points[tail].x);
    float dy = (points[head].y - points[tail].y);
    float r = sqrt(dx*dx + dy*dy);
    if(r > 4.0) r = (4.0 + log(r - 3.0));
    g[head].x -= amp*dx*r;
    g[head].y -= amp*dy*r;
    g[tail].x += amp*dx*r;
    g[tail].y += amp*dy*r;
  }
}

/*
 *
 */
vector<float>
Engine::bbox() {
  unsigned int i;
  vector<float> bounds(4, 0.0);
  unsigned int n_points = points.size();
  if(n_points < 2) return bounds;

  /* Determine bounding box. */
  float box[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  for(i = 0; i < n_points; i++) {
    if(points[i].x < box[0]) box[0] = points[i].x;
    if(points[i].y < box[1]) box[1] = points[i].y;
    if(points[i].x > box[2]) box[2] = points[i].x;
    if(points[i].y > box[3]) box[3] = points[i].y;
  }
  for(i = 0; i < n_points; i++) {
    points[i].x -= box[0];
    points[i].y -= box[1];
  }
  bounds[2] = (box[2] - box[0]);
  bounds[3] = (box[3] - box[1]);
  bounds[0] = 0.0;
  bounds[1] = 0.0;

  return bounds;
}

/*
 * Neighborhood must not contain home cell.
 */
void
Engine::repel(vector<Point>& g, const vector<vector<int> >& cells,
	      const vector<Point>& landscape, const vector<int>& nhood,
	      int center) {
  unsigned int i, k;

  /* Collect targets. */
  vector<int> home = cells[center];
  unsigned int n_targets = home.size();
  Point targets[n_targets+1];
  Point gradients[n_targets+1];
  for(k = 0; k < n_targets; k++) {
    targets[k] = points[home[k]];
    gradients[k] = g[home[k]];
  }

  /* Collect neighborhood. */
  unsigned int n_neigh = nhood.size();
  Point neighbors[n_neigh+1];
  for(i = 0; i < n_neigh; i++)
    neighbors[i] = landscape[nhood[i]];

  /* Compute gradients between targets. */
  for(i = 0; i < n_targets; i++) {
    float x = targets[i].x;
    float y = targets[i].y;
    float gx = 0.0;
    float gy = 0.0;
    float power = targets[i].power;
    for(k = (i + 1); k < n_targets; k++) {
      float dx = (targets[k].x - x);
      float dy = (targets[k].y - y);
      float r = (dx*dx + dy*dy + FLT_EPSILON);
      float amp = (targets[k].power + power);
      if(r < 1.1) amp = (100.0 + 10*amp);
      gx -= (amp*dx/r);
      gy -= (amp*dy/r);
      gradients[k].x += (amp*dx/r);
      gradients[k].y += (amp*dy/r);
    }
    gradients[i].x += gx;
    gradients[i].y += gy;
  }

  /* Compute gradients between targets and neighbors. */
  for(i = 0; i < n_targets; i++) {
    float x = targets[i].x;
    float y = targets[i].y;
    float gx = 0.0;
    float gy = 0.0;
    float power = targets[i].power;
    for(k = 0; k < n_neigh; k++) {
      float dx = (neighbors[k].x - x);
      float dy = (neighbors[k].y - y);
      float r = (dx*dx + dy*dy + FLT_EPSILON);
      float amp = (neighbors[k].power + power);
      gx -= (amp*dx/r);
      gy -= (amp*dy/r);
    }
    gradients[i].x += gx;
    gradients[i].y += gy;
  }

  /* Update gradients. */
  for(k = 0; k < n_targets; k++)
    g[home[k]] = gradients[k];
}

/*
 *
 */
void
Engine::rotate(const Point p, const float theta) {
  unsigned int i;
  float dx, dy, r, phi;
  for(i = 0; i < points.size(); i++) {
    dx = (points[i].x - p.x);
    dy = (points[i].y - p.y);
    r = sqrt(dx*dx + dy*dy);
    if((dx >= 0.0) && (dx < FLT_EPSILON)) dx = FLT_EPSILON;
    if((dx <= 0.0) && (dx > -FLT_EPSILON)) dx = -FLT_EPSILON;
    phi = atan(dy/dx);
    if(dx < 0.0) phi = (M_PI + phi);
    
    dx = r*cos(phi + theta);
    dy = r*sin(phi + theta);
    points[i].x = (p.x + dx);
    points[i].y = (p.y + dy);
  }
}

/*
 *
 */
vector<Point>
Engine::summarize(const vector<vector<int> >& cells) {
  unsigned int i, k;
  vector<Point> landscape(cells.size());

  for(i = 0; i < cells.size(); i++) {
    const vector<int>& targets = cells[i];
    unsigned int n = targets.size(); 
    if(n < 1) continue;
    landscape[i].x = 0.0;
    landscape[i].y = 0.0;
    landscape[i].power = 0.0;
    for(k = 0; k < n; k++) {
      int index = targets[k];
      float power = points[index].power;
      landscape[i].x += power*(points[index].x);
      landscape[i].y += power*(points[index].y);
      landscape[i].power += power;
    }
    landscape[i].x /= n*(landscape[i].power);
    landscape[i].y /= n*(landscape[i].power);
    landscape[i].power /= n;
  }

  return landscape;
}
