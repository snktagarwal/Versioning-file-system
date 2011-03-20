/* file: graphobject.import.cpp
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
bool
GraphObject::import() {
  unsigned int i;
  vector<Edge> edges;
  map<string, Table>::iterator tpos;
  if(verbose_mode) cout << "\nImporting graph:\n";

  /* Find topology variables. */
  map<string, Locus>& loci = variables[cfg["EdgeFile"][1]];
  int jhead = loci["EdgeHeadVariable"].column;
  int jtail = loci["EdgeTailVariable"].column;
  int jweight = loci["EdgeWeightVariable"].column;

  /* Prepare edge table. */
  Table& edge_table = tables[cfg["EdgeFile"][1]];
  vector<int> key_cols(2);
  key_cols[0] = jhead;  
  key_cols[1] = jtail;  
  edge_table.sort(key_cols);

  /* Create graph representation. */
  string label1 = "";
  string label2 = "";
  double row_NaN = Row::nan();
  for(i = 0; i < edge_table.size(); i++) {
    Row r = edge_table[i];
    if(r.rank() < 1) continue;
    if(r[jhead] == "") continue;
    if(r[jtail] == "") continue;
    if(r[jhead] == r[jtail]) continue;
    if((r[jhead] == label1) && (r[jtail] == label2) ) {
      cout << "WARNING! Duplicate edge '" << label2 
	   << "'->'" << label1 << "' detected in file '"
	   << edge_table.source() << "'.\n";
      continue;
    }
    label1 = r[jhead];
    label2 = r[jtail];

    /* Check weight. */
    double w = r.number(jweight);
    if(jweight < 0) w = 1.0;
    if(w == row_NaN) continue;
    if(!(w > 1e-10)) {
      cout << "WARNING! Edge '" << label2 
	   << "'->'" << label1 << "' rejected due to low weight.\n";
      continue;
    }
    if(!(w < 1e10)) {
      cout << "WARNING! Edge '" << label2 
	   << "'->'" << label1 << "' has excessive weight.\n";
      return false;
    }
 
    /* Add edge. */
    Edge e;
    e.head = label1;
    e.tail = label2;
    e.weight = w;
    edges.push_back(e);
  }

  /* Normalize edge weights and apply weight filter. */
  vector<double> weights(edges.size());
  for(i = 0; i < edges.size(); i++)
    weights[i] = edges[i].weight;
  normalize(weights, cfg["EdgeWeightTransform"][1], edge_weight_filter);
  for(i = 0; i < edges.size(); i++)
    edges[i].weight = weights[i];

  /* Create components. */
  components = Component::create(edges, cfg["ChassisMode"].number(2));
  if(components.size() < 1) {
    cout << "WARNING! Could not find any components in '"
	 << edge_table.source() << "'.\n";
    return false;
  }
  if(verbose_mode) {
    for(i = 0; i < components.size(); i++) {
      cout << "\t[" << (i + 1) << "]\t" << components[i].cardinality()
	   << '\t' << components[i].size() << "\n";
    }
  }

  return true;
}
