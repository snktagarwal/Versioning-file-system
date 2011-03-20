/* file: statistic.cpp
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

    WWW: http://www.iki.fi/~vpmakine
*/

#include <algorithm>
#include <float.h>
#include <math.h>
#include "medusa.h"

using namespace medusa;
using namespace std;

static double stat_mean(const vector<double>&);
static double stat_median(vector<double>);

/*
 *
 */
double
medusa::statistic(const vector<double>& x, const string& name) {
  if(name == "mean") return stat_mean(x);
  if(name == "median") return stat_median(x);
  return medusa::nan();
}

/*
 *
 */
double
stat_mean(const vector<double>& x) {
  unsigned int i, k;
  unsigned int n_elem = x.size();
  if(n_elem < 1) return medusa::nan();  

  /* Sum all values. */
  double xs = 0.0;
  double not_a_num = medusa::nan();
  for(i = 0, k = 0; i < n_elem; i++) {
    if(x[i] == not_a_num) continue;
    xs += x[i];
    k++; 
  }

  /* Divide by number. */
  if(k < 1) return medusa::nan();
  return xs/k;
}

/*
 *
 */
double
stat_median(vector<double> x) {
  unsigned int i, k;
  unsigned int n_elem = x.size();
  if(n_elem < 1) return medusa::nan();  

  /* Count the number of available data. */
  double not_a_num = medusa::nan();
  for(i = 0, k = 0; i < n_elem; i++) {
    if(x[i] == not_a_num) {
      x[i] = DBL_MAX;
      continue;
    }
    k++; 
  }

  /* Sort array. */
  sort(x.begin(), x.end());

  /* Determine 50% quantile. */
  return x[k/2];
}
