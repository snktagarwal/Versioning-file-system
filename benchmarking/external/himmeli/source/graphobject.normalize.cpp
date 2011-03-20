/* file: graphobject.normalize.cpp
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

/*
 *
 */
void
GraphObject::normalize(vector<double>& weights, string tfmode,
		       vector<float> filter) {
  unsigned int i, k;
  unsigned int n = weights.size();
  unsigned int n_edges = 0;
  float w_min = FLT_MAX;
  float w_max = -FLT_MAX;
 
  /* Find limits. */
  if(n < 1) return;
  for(i = 0; i < n; i++) {
    if(weights[i] < w_min) w_min = weights[i];
    if(weights[i] > w_max) w_max = weights[i];
  }

  /* Uniform weights. */
  if(w_min == w_max) {
    for(i = 0; i < n; i++)
      weights[i] = DEFAULT_WEIGHT;
    return;
  }

  /* Apply linear filter. */
  if(filter[0] == ABS_FILTER_code) {
    for(i = 0; i < n; i++) {
      if((weights[i] < filter[1]) || (weights[i] > filter[2]))
	weights[i] = -1.0;
    }
  }

  /* Sort weights and apply fractional filter. */
  vector<double> tmp = weights;
  vector<unsigned int> indices = sortdouble(tmp);
  if(filter[0] == FRAC_FILTER_code) {
    unsigned int ka = (unsigned int)(filter[1]*n);
    unsigned int kb = (unsigned int)(filter[2]*n);
    for(k = 0; (k < ka) && (k < n); k++) {
      i = indices[k];
      weights[i] = -1.0;
    }
    for(k = (kb + 1); k < n; k++) {
      i = indices[k];
      weights[i] = -1.0;
    }
  }

  /* Update limits. */
  w_min = FLT_MAX;
  w_max = -FLT_MAX;
  n_edges = 0;
  for(i = 0; i < n; i++) {
    if(weights[i] < 0.0) continue;
    if(weights[i] < w_min) w_min = weights[i];
    if(weights[i] > w_max) w_max = weights[i];
    n_edges++;
  }
  if(n_edges < 1) return;
  if(w_min == w_max) return;

  /* No edge transform. */
  if(tfmode == "") return;
  if(tfmode == "off") return;

  /* Linear transform. */
  if(w_max <= 0.0) w_max = FLT_EPSILON;
  if(tfmode == "lin") {
    for(i = 0; i < n; i++) {
      if(weights[i] < 0.0) continue;
      weights[i] /= w_max;
    }
    return;
  }

  /* Logarithmic transform. */
  if(tfmode == "log") {
    w_max = log(1.001 + w_max - w_min);
    for(i = 0; i < n; i++) {
      if(weights[i] < 0.0) continue;
      weights[i] = log(1.0 + weights[i] - w_min)/w_max;
    }
    return;
  }

  /* Rank transform. */
  unsigned int counter = 0;
  if(tfmode == "rank") {
    w_max = 1.0*n_edges;
    for(k = 0; k < n; k++) {
      i = indices[k];
      if(weights[i] < 0.0) continue;
      weights[i] = (++counter)/w_max;
    }
    return;
  }

  /* Default transform. */
  counter = 0;
  float w1_max = w_max;
  float w2_max = log(1.001 + w_max - w_min);
  float w3_max = 1.0*n_edges;
  for(k = 0; k < n; k++) {
    i = indices[k];
    if(weights[i] < 0.0) continue;
    float w1 = (weights[i])/w1_max;
    float w2 = log(1.0 + weights[i] - w_min)/w2_max;
    float w3 = (++counter)/w3_max;
    weights[i] = (w1 + w2 + w3)/3.0;
  }
}
