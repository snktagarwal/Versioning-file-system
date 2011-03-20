/* file: graphobject.save_vertices.cpp
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
GraphObject::save_vertices(const string& fname, map<string, string>& instr) {
  unsigned int i;
  unsigned long n = 0;

  /* Open file. */
  char iobuf[131072];
  FILE* output = fopen(fname.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", fname.c_str());
    return 0;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Print header. */
  n += fprintf(output, "%s\tCOMP\t", instr["VertexNameVariable"].c_str());
  n += fprintf(output, "DGR_IN\tDGR_OUT\tDEGREE\tSTN_IN\tSTN_OUT\tSTRENG\t");
  n += fprintf(output, "%s\t", instr["VertexXVariable"].c_str());
  n += fprintf(output, "%s\t", instr["VertexYVariable"].c_str());
  n += fprintf(output, "%s\t", instr["VertexSizeVariable"].c_str());
  n += fprintf(output, "%s\t", instr["VertexColorVariable"].c_str());
  n += fprintf(output, "%s\t", instr["VertexShapeVariable"].c_str());
  n += fprintf(output, "%s\t", instr["VertexPatternVariable"].c_str());
  n += fprintf(output, "%s\n", instr["VertexLabelVariable"].c_str());

  /* Save topology. */
  for(i = 0; i < components.size(); i++) {
    map<string, Vertex>::iterator pos;
    map<string, Vertex> vertices = components[i].vertices();
    for(pos = vertices.begin(); pos != vertices.end(); pos++) {
      Vertex v = pos->second;
      n += fprintf(output, "%s\t%d", (pos->first).c_str(), (i + 1));
      n += fprintf(output, "\t%d", v.degree_in);
      n += fprintf(output, "\t%d", v.degree_out);
      n += fprintf(output, "\t%d", (v.degree_in + v.degree_out));
      n += fprintf(output, "\t%7.2f", v.strength_in);
      n += fprintf(output, "\t%7.2f", v.strength_out);
      n += fprintf(output, "\t%7.2f", (v.strength_in + v.strength_out));
      n += fprintf(output, "\t%7.2f", v.x);
      n += fprintf(output, "\t%7.2f", v.y);

      Emblem e = emblems[pos->first];
      n += fprintf(output, "\t%7.2f", e.radius);
      n += fprintf(output, "\t%06d", e.color);
      n += fprintf(output, "\t%s", (e.shape).c_str());
      n += fprintf(output, "\t%s", (e.pattern).c_str());
      n += fprintf(output, "\t%s", (e.label).c_str());
      n += fprintf(output, "\n");
    }
  }
  fclose(output);

  return n;
}
