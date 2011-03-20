/* file: graphobject.save.cpp
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

static string get_unique(map<string, int>&, const string&);

/*
 *
 */
void
GraphObject::save() {
  unsigned int i;
  if(components.size() < 1) return;
  if(verbose_mode) cout << "\nSaving graph:\n";

  /* Collect instructions. */
  map<string, string> instr;
  map<string, int> headings;
  Table* t = (Table*)(&cfg);  
  for(i = 0; i < t->size(); i++) {
    Row r = (*t)[i];
    if(r[0] == "") continue;
    if(r[0][0] == '#') continue;
    instr[r[0]] = r[1];
    headings[r[1]] = 1; 
  }

  /* Check that all variables are set. */
  if(instr["EdgeHeadVariable"] == "")
    instr["EdgeHeadVariable"] = get_unique(headings, "HEAD");
  if(instr["EdgeTailVariable"] == "")
    instr["EdgeTailVariable"] = get_unique(headings, "TAIL");
  if(instr["EdgeColorVariable"] == "")
    instr["EdgeColorVariable"] = get_unique(headings, "ECOLOR");
  if(instr["EdgeLabelVariable"] == "")
    instr["EdgeLabelVariable"] = get_unique(headings, "ELABEL");
  if(instr["EdgeWidthVariable"] == "")
    instr["EdgeWidthVariable"] = get_unique(headings, "WIDTH");
  if(instr["EdgeWeightVariable"] == "")
    instr["EdgeWeightVariable"] = get_unique(headings, "WEIGHT");
  if(instr["VertexNameVariable"] == "")
    instr["VertexNameVariable"] = get_unique(headings, "NAME");
  if(instr["VertexColorVariable"] == "")
    instr["VertexColorVariable"] = get_unique(headings, "VCOLOR");
  if(instr["VertexLabelVariable"] == "")
    instr["VertexLabelVariable"] = get_unique(headings, "VLABEL");
  if(instr["VertexPatternVariable"] == "")
    instr["VertexPatternVariable"] = get_unique(headings, "PATTERN");
  if(instr["VertexShapeVariable"] == "")
    instr["VertexShapeVariable"] = get_unique(headings, "SHAPE");
  if(instr["VertexSizeVariable"] == "")
    instr["VertexSizeVariable"] = get_unique(headings, "SIZE");
  if(instr["VertexXVariable"] == "")
    instr["VertexXVariable"] = get_unique(headings, "X");
  if(instr["VertexYVariable"] == "")
    instr["VertexYVariable"] = get_unique(headings, "Y");

  /* Override critical parameters. */
  instr["TimeLimit"] = "0";
  instr["IncrementMode"] = "on";
  instr["Delimiter"] = "tab";
  instr["EdgeWeightFilter"] = "off";
  instr["EdgeWeightMask"] = "off";
  instr["EdgeWeightTransform"] = "off";

  /* Remove info instructions. */
  instr["EdgeColorInfo"] = "";
  instr["EdgeLabelInfo"] = "";
  instr["VertexColorInfo"] = "";
  instr["VertexLabelInfo"] = "";
  instr["VertexPatternInfo"] = "";
  instr["VertexShapeInfo"] = "";

  /* File names. */
  string cfile = (cfg["GraphName"][1] + ".config.txt");
  string vfile = (cfg["GraphName"][1] + ".vertices.txt");
  string efile = (cfg["GraphName"][1] + ".edges.txt");
  string tfile = (cfg["GraphName"][1] + ".chassis.txt");
  instr["EdgeFile"] = efile;
  instr["VertexFile"] = vfile;

  /* Save graph data. */
  unsigned long nc = save_config(cfile, instr);
  unsigned long nv = save_vertices(vfile, instr);
  unsigned long ne = save_edges(efile, instr, false);
 
  /* Save chassis. */
  unsigned long nt = 0;
  if(chassis_mode) nt = save_edges(tfile, instr, true);
  
   /* Show report. */
  if(!verbose_mode) return;
  if(nc > 0)
    cout << "\tWrote " << long2text(nc) << " bytes to '" << cfile << "'.\n";
  if(nv > 0)
    cout << "\tWrote " << long2text(nv) << " bytes to '" << vfile << "'.\n";
  if(ne > 0)
    cout << "\tWrote " << long2text(ne) << " bytes to '" << efile << "'.\n";
  if(nt > 0)
    cout << "\tWrote " << long2text(nt) << " bytes to '" << tfile << "'.\n";
}

/*
 *
 */
static string
get_unique(map<string, int>& headings, const string& original) {
  unsigned int i;
  string name = original;
  char buffer[64];

  /* Append numbers until the name is unique. */
  for(i = 0; (headings.count(name) > 0) && (i < 1000); i++) {
    headings[original] += 1;
    sprintf(buffer, "(%d)", headings[original]);
    name = (original + string(buffer));
  }
  headings[name] = 1;

  return name;
}
