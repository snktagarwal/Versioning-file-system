/* file: graphobject.save_config.cpp
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
GraphObject::save_config(const string& fname, map<string, string>& instr) {
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

  /* Make date stamp. */
  time_t now = time(NULL);
  n += fprintf(output, "# Created by Himmeli %s\n", graph_VERSION);
  n += fprintf(output, "# %s\n", ctime(&now));

  /* Save instructions. */
  map<string, string>::iterator pos;
  for(pos = instr.begin(); pos != instr.end(); pos++) {
    if(pos->second == "") continue;
    n += fprintf(output, "%-22s ", (pos->first).c_str());
    n += fprintf(output, "%-16s\n", (pos->second).c_str());
  }
  n += fprintf(output, "\n");

  /* Save legend info. */
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("EdgeColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue;
    n += fprintf(output, "%-22s ", row[0].c_str());
    n += fprintf(output, "%-16s ", row[1].c_str());
    n += fprintf(output, "%-16s\n", row[2].c_str());
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    n += fprintf(output, "%-22s ", row[0].c_str());
    n += fprintf(output, "%-16s ", row[1].c_str());
    n += fprintf(output, "%-16s\n", row[2].c_str());
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexPatternInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    n += fprintf(output, "%-22s ", row[0].c_str());
    n += fprintf(output, "%-16s ", row[1].c_str());
    n += fprintf(output, "%-16s\n", row[2].c_str());
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexShapeInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    n += fprintf(output, "%-22s ", row[0].c_str());
    n += fprintf(output, "%-16s ", row[1].c_str());
    n += fprintf(output, "%-16s\n", row[2].c_str());
  }
  fclose(output);

  return n;
}
