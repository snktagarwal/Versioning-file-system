/* file: graphobject.print.cpp
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

static void customize(Artist&, Table&);
static vector<float> get_dimen(map<string, Vertex>&, float);

/*
 *
 */
void
GraphObject::print() {
  vector<float> ldimen(2, 0.0);
  unsigned int i;
  if(components.size() < 1) return;
  if(verbose_mode) printf("\nSaving figures:\n");

  /* Print components to separate files. */
  for(i = 0; i < components.size(); i++) {
    if(i >= figure_limit) break;
    map<string, Vertex> vertices = components[i].vertices();    
    vector<float> dimen = get_dimen(vertices, distance_unit);

    /* Determine canvas size. */
    char buffer[64];
    sprintf(buffer, "%.2f,%.2f", dimen[0], dimen[1]);
    string page_size = string(buffer);

    /* Determine scale factor. */
    double z = 1.0;
    double diam = sqrt(dimen[0]*dimen[0] + dimen[1]*dimen[1]);
    if(diam > 30) z = 30/diam;
    if((z < 0.05) || (z > 50.0)) {
      sprintf(buffer, "Component %d", i);
      cout << "WARNING! Drawing is too small or large for "
	   << string(buffer) << ".\n";
      continue;
    }
    sprintf(buffer, "%.2f", z);
    string scale = string(buffer);

    /* File name. */
    sprintf(buffer, "_%04d.svg", (i + 1));

    /* Create SVG template. */
    Artist art(cfg["GraphName"][1] + string(buffer));
    customize(art, cfg); 
    art.assign("PageSize", page_size);
    art.assign("Scale", scale);
    art.assign("VerboseMode", "off");

    /* Print component. */
    vector<Edge> edges;
    if(!chassis_mode) edges = components[i].edges();
    else edges = components[i].chassis();
    print_links(art, vertices, edges);
    print_nodes(art, vertices); 
    art.close();
    if(verbose_mode && (art.size() > 0)) {
      cout << "\tWrote " << long2text(art.size())
	   << " bytes to '" << art["FileName"] << "'.\n";
    }
  }

  /* Print element info. */
  Artist art(cfg["GraphName"][1] + ".info.svg");
  customize(art, cfg);
  art.assign("VerboseMode", "off");
  if(!print_info(art)) return;
  art.close();
  if(verbose_mode && (art.size() > 0)) {
    cout << "\tWrote " << long2text(art.size())
	 << " bytes to '" << art["FileName"] << "'.\n";
  }
}

/*
 *
 */
static void
customize(Artist& art, Table& cfg) {
  unsigned int i;
  for(i = 0; i < cfg.size(); i++) {
    Row r = cfg[i];
    art.assign(r[0], r[1]);
  }
  art.assign("Creator", ("Himmeli " + string(graph_VERSION)));
}

/*
 *
 */
static vector<float>
get_dimen(map<string, Vertex>& vertices, float u) {
  float bbox[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  vector<float> dimen;

  /* Determine bounding box. */
  map<string, Vertex>::iterator pos;
  for(pos = vertices.begin(); pos != vertices.end(); pos++) {
    float x = (pos->second).x;
    float y = (pos->second).y;
    x *= u;
    y *= u;
    if(x < bbox[0]) bbox[0] = x;
    if(x > bbox[2]) bbox[2] = x;
    if(y < bbox[1]) bbox[1] = y;
    if(y > bbox[3]) bbox[3] = y;
    (pos->second).x = x;
    (pos->second).y = y;
  }
  bbox[0] -= MARGIN_WIDTH;
  bbox[1] -= MARGIN_WIDTH;
  bbox[2] += 1.5*MARGIN_WIDTH;
  bbox[3] += MARGIN_WIDTH;

  /* Remove offsets. */
  for(pos = vertices.begin(); pos != vertices.end(); pos++) {
    (pos->second).x -= bbox[0];
    (pos->second).y -= bbox[1];
  }
  dimen.push_back(bbox[2] - bbox[0]);
  dimen.push_back(bbox[3] - bbox[1]);
  
  return dimen;
}

