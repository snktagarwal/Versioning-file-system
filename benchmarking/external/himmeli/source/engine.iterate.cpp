/* file: engine.iterate.cpp
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

#include <unistd.h>

/*
 *
 */
void
Engine::iterate(const float temp) {
  unsigned int i;
  unsigned int n_points = points.size();
  float cell_size = log(1.0 + bonds.size());
  if(cell_size < MIN_CELLSIZE) cell_size = MIN_CELLSIZE;
  if(n_points < 2) return;

  /* Determine cell size and neighborhood radius. */
  vector<float> box = bbox();
  float width = box[2];
  float height = box[3];

  /* Create cell structure. Note that a buffer of empty cells is
     left on the perimeter. */
  int m_rows = (int)(width/cell_size + 1.5);
  int m_cols = (int)(height/cell_size + 1.5);
  vector<vector<int> > cells(m_rows*m_cols, vector<int>(0));
  for(i = 0; i < points.size(); i++) {
    int k = (int)((points[i].x)/cell_size + 0.5);
    int j = (int)((points[i].y)/cell_size + 0.5);
    cells[k*m_cols+j].push_back(i);
  }
  vector<Point> landscape = summarize(cells);

  /* Initial gradients. */
  vector<Point> g(points.size());
  for(i = 0; i < points.size(); i++) {
    g[i].x = 0.0;
    g[i].y = 0.0;
  }

  /* Compute repulsive forces. */
  vector<int> nhood;
  double rnd1 = (1.0*rand())/RAND_MAX;
  double rnd2 = (1.0*rand())/RAND_MAX;
  int r_nhood = (int)(1.0 + 0.1*(rnd1*m_rows + rnd2*m_cols));
  for(i = 0; i < cells.size(); i++) {
    if(cells[i].size() < 1) continue;
    int row = i/m_cols;
    int col = i%m_cols;
    for(int k = (row - r_nhood); k <= (row + r_nhood); k++) {
      for(int j = (col - r_nhood); j <= (col + r_nhood); j++) {
	unsigned int rank = (k*m_cols + j);
	if(rank < 0) continue;
	if(rank == i) continue;
	if(rank >= cells.size()) continue;
	if(cells[rank].size() < 1) continue; 
	nhood.push_back(rank);
      }
    }
    repel(g, cells, landscape, nhood, i);
    nhood.clear();
  }
  
  /* Compute attractive forces. */ 
  attract(g, bonds);

  /* Update positions and compute energy. */
  for(i = 0; i < points.size(); i++) {
    float r = sqrt((g[i].x)*(g[i].x) + (g[i].y)*(g[i].y));
    if(r > temp) {
      g[i].x *= temp/r;
      g[i].y *= temp/r;
    }
    points[i].x += (0.8 + (0.4*rand())/RAND_MAX)*(g[i].x);
    points[i].y += (0.8 + (0.4*rand())/RAND_MAX)*(g[i].y);
  }
  rotate(points[0], rand()*M_PI/RAND_MAX);
}
