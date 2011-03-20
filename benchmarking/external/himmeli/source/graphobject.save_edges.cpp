/* file: graphobject.save_edges.cpp
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
unsigned long
GraphObject::save_edges(const string& fname, map<string, string>& instr,
			bool chassis_flag) {
  unsigned int i, k;
  unsigned long n = 0;

  /* Delimiter is needed to find arcs. */
  char delim = delimiter;
  if(delim == '\0') delim = ' ';

  /* Open file. */
  char iobuf[131072];
  FILE* output = fopen(fname.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", fname.c_str());
    return 0;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Print header. */
  n += fprintf(output, "%s\t", instr["EdgeTailVariable"].c_str());
  n += fprintf(output, "%s\tCOMP\t", instr["EdgeHeadVariable"].c_str());
  n += fprintf(output, "%s\t", instr["EdgeWeightVariable"].c_str());
  n += fprintf(output, "%s\t", instr["EdgeColorVariable"].c_str());
  n += fprintf(output, "%s\t", instr["EdgeWidthVariable"].c_str());
  n += fprintf(output, "%s\n", instr["EdgeLabelVariable"].c_str());

  /* Save full topology. */
  for(i = 0; i < components.size(); i++) {
    vector<Edge> edges;
    if(chassis_flag) edges = components[i].chassis();
    else edges = components[i].edges();
    for(k = 0; k < edges.size(); k++) {
      string head = edges[k].head;
      string tail = edges[k].tail;
      n += fprintf(output, "%s\t%s", tail.c_str(), head.c_str());
      n += fprintf(output, "\t%d", (i + 1));

      Arc a = arcs[head + delim + tail];
      n += fprintf(output, "\t%g", a.weight);
      n += fprintf(output, "\t%06d", a.color);
      n += fprintf(output, "\t%7.2f", a.width);
      n += fprintf(output, "\t%s", (a.label).c_str());
      n += fprintf(output, "\n");
    }
  }
  fclose(output);

  return n;
}
