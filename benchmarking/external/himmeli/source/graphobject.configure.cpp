/* file: graphobject.configure.cpp
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

static vector<float> get_filter(ConfigTable&, string, bool);
static void detect(map<string, Locus>&, Table&, ConfigTable&, bool);

/*
 *
 */
bool
GraphObject::configure() {
  unsigned int i;

  /* Determine basic flags. */
  arrow_mode = (cfg["ArrowMode"][1] == "on");
  decoration_mode = (cfg["DecorationMode"][1] == "on");
  increment_mode = (cfg["IncrementMode"][1] == "on");
  label_mode = cfg["LabelMode"][1];
  if((label_mode != "on") && (label_mode != "off") && (label_mode != "edge"))
    label_mode = "vertex";
  chassis_mode = (cfg["ChassisMode"][1] == "on");
  verbose_mode = (cfg["VerboseMode"][1] != "off");

  /* Determine global parameters. */
  edge_weight_transform = cfg["EdgeWeightTransform"][1];
  if((edge_weight_transform != "lin") && (edge_weight_transform != "log") &&
     (edge_weight_transform != "rank") && (edge_weight_transform != "off"))
    edge_weight_transform = "auto";
  delimiter = '\t';
  if(cfg["Delimiter"][1].length() > 1) {
    if(cfg["Delimiter"][1] == "tab") delimiter = '\t';
    else if(cfg["Delimiter"][1] == "ws") delimiter = '\0';
    else delimiter = cfg["Delimiter"][1][0];
  }
  distance_unit = 1.0;
  if(arrow_mode) distance_unit *= 1.2;
  if((label_mode == "on") || (label_mode == "edge")) distance_unit *= 1.2;
  if(chassis_mode) distance_unit /= 1.3;
  if(cfg["DistanceUnit"].size() > 1)
    distance_unit = cfg["DistanceUnit"].number(1);
  if(distance_unit < 0.1) distance_unit = 0.1;
  if(distance_unit > 10.0) distance_unit = 10.0;
  figure_limit = (unsigned int)(cfg["FigureLimit"].number(1));
  if(cfg["FigureLimit"].size() < 2) figure_limit = 10;
  if(figure_limit > 9999) figure_limit = 9999;
  if(cfg["TimeLimit"].size() < 2) time_limit = 10.0;
  else time_limit = (float)(cfg["TimeLimit"].number(1));

  /* Display basic configuration. */
  if(verbose_mode) {
    printf("Basic configuration:\n");
    if(arrow_mode) printf("\t%-22s %-16s\n", "ArrowMode", "on");
    else printf("\t%-22s %-16s\n", "ArrowMode", "off");
    if(decoration_mode) printf("\t%-22s %-16s\n", "DecorationMode", "on");
    else printf("\t%-22s %-16s\n", "DecorationMode", "off");
    printf("\t%-22s %.2f\n", "DistanceUnit", distance_unit);
  }
  edge_weight_filter = get_filter(cfg, "EdgeWeightFilter", verbose_mode);
  edge_weight_mask = get_filter(cfg, "EdgeWeightMask", verbose_mode);
  if(verbose_mode) {
    printf("\t%-22s ", "EdgeWeightTransform");
    printf("%s\n", edge_weight_transform.c_str());
    printf("\t%-22s %d\n", "FigureLimit", figure_limit);
    if(increment_mode) printf("\t%-22s %-16s\n", "IncrementMode", "on");
    else printf("\t%-22s %-16s\n", "IncrementMode", "off");
    printf("\t%-22s %-16s\n", "LabelMode", label_mode.c_str());
    printf("\t%-22s %.0fs\n", "TimeLimit", time_limit);
    if(chassis_mode) {
      double chsize = cfg["ChassisMode"].number(2);
      if(chsize == medusa::nan())
	printf("\t%-22s %-16s\n", "ChassisMode", "on");
      else printf("\t%-22s %-16s\t%.2f\n", "ChassisMode", "on", chsize);
    }
    else printf("\t%-22s %-16s\n", "ChassisMode", "off");
  }

  /* Show load message. */
  if(verbose_mode) {
    if(delimiter == '\0')
      cout << "\nLoading white space delimited data:\n";
    else if(delimiter == '\t')
      cout << "\nLoading tabulator delimited data:\n";
    else
      cout << "\nLoading '" << delimiter << "' delimited data:\n";
  }

  /* Copy data from primary graph file. */
  string edge_file = cfg["EdgeFile"][1];
  Table data = Table(edge_file, delimiter);
  if(data.size() < 1) {
    cout << "WARNING! File '" << edge_file
	 << "' is empty or cannot be opened.\n";
    return false;
  }
  if(verbose_mode) {
    cout << "\tCopied " << long2text(data.mem_size()) << " bytes from '"
	 << edge_file << "'.\n";
  }

  /* Check that graph file contains necessary variables. */
  if(data.column(cfg["EdgeHeadVariable"][1]) < 0) {
    cout << "WARNING! Could not find edge head variable in '"
	 << edge_file << "'.\n";
    return false;
  }
  if(data.column(cfg["EdgeTailVariable"][1]) < 0) {
    cout << "WARNING! Could not find edge tail variable in '"
	 << edge_file << "'.\n";
    return false;
  }
  tables[edge_file] = data;

  /* Read variable instructions. */
  vector<Locus> loci;
  loci.push_back(Locus("EdgeHeadVariable", cfg["EdgeHeadVariable"]));
  loci.push_back(Locus("EdgeTailVariable", cfg["EdgeTailVariable"]));
  loci.push_back(Locus("EdgeWeightVariable", cfg["EdgeWeightVariable"]));
  loci.push_back(Locus("VertexNameVariable", cfg["VertexNameVariable"]));
  loci.push_back(Locus("VertexXVariable", cfg["VertexXVariable"]));
  loci.push_back(Locus("VertexYVariable", cfg["VertexYVariable"]));
  loci.push_back(Locus("EdgeColorVariable", cfg["EdgeColorVariable"]));
  loci.push_back(Locus("EdgeLabelVariable", cfg["EdgeLabelVariable"]));
  loci.push_back(Locus("EdgeWidthVariable", cfg["EdgeWidthVariable"]));
  loci.push_back(Locus("VertexColorVariable", cfg["VertexColorVariable"]));
  loci.push_back(Locus("VertexLabelVariable", cfg["VertexLabelVariable"]));
  loci.push_back(Locus("VertexPatternVariable",
		       cfg["VertexPatternVariable"]));
  loci.push_back(Locus("VertexShapeVariable", cfg["VertexShapeVariable"]));
  loci.push_back(Locus("VertexSizeVariable", cfg["VertexSizeVariable"]));

  /* Associate variables with input files. */
  string vertex_file = cfg["VertexFile"][1];
  if(vertex_file == "") vertex_file = edge_file;
  for(i = 0; i < loci.size(); i++) {
    string locus_name = loci[i].name();
    string& file_name = loci[i].file_name;
    if((file_name == "") && (locus_name[0] == 'E'))
      file_name = edge_file;
    if((file_name == "") && (locus_name[0] == 'V'))
      file_name = vertex_file;
    variables[file_name][locus_name] = loci[i];
  }

  /* Copy data from secondary input files. */
  map<string, map<string, Locus> >::iterator pos;
  for(pos = variables.begin(); pos != variables.end(); pos++) {
    if(tables.count(pos->first) > 0) continue;
    data = Table(pos->first, delimiter); 
    if(data.size() < 1) {
      cout << "WARNING! File '" << pos->first
	   << "' is empty or cannot be opened.\n";
    }
    else if(verbose_mode) {
      cout << "\tCopied " << long2text(data.mem_size()) << " bytes from '"
	   << data.source() << "'.\n";
    }
    tables[pos->first] = data;
  }

  /* Detect and check variables from files. */
  map<string, Table>::iterator iter;
  if(verbose_mode) cout << "\nDetecting variables:\n";
  for(iter = tables.begin(); iter != tables.end(); iter++)
    detect(variables[iter->first], iter->second, cfg, verbose_mode);
 
  return true;
}

/*
 *
 */
static vector<float>
get_filter(ConfigTable& cfg, string tag, bool verbose) {
  vector<float> filter(3, 0.0);
  const char* ptr = tag.c_str();
  filter[0] = NULL_FILTER_code;

  Row r = cfg[tag];
  if(r.size() < 4) {
    if(!verbose) return filter;
    printf("\t%-22s off\n", ptr);
    return filter;
  }
  filter[1] = r.number(2);
  filter[2] = r.number(3);

  if(r[1] == "frac") {
    filter[0] = FRAC_FILTER_code;
    if(filter[1] < 0.0) filter[1] = 0.0;
    if(filter[1] > 1.0) filter[1] = 1.0;
    if(filter[2] < filter[1]) filter[2] = filter[1];
    if(filter[2] > 1.0) filter[2] = 1.0;
    if(!verbose) return filter;
    printf("\t%-22s frac %.2f - %.2f\n", ptr, filter[1], filter[2]);
    return filter;
  }
  
  filter[0] = ABS_FILTER_code;
  if(filter[1] < 0.0) filter[1] = 0.0;
  if(filter[2] < filter[1]) filter[2] = filter[1];
  if(!verbose) return filter;
  printf("\t%-22s abs  %.2f - %.2f\n", ptr, filter[1], filter[2]);
  return filter;
}

/*
 *
 */
static void
detect(map<string, Locus>& loci, Table& t, ConfigTable& cfg, bool verbose) {
  bool edge_flag = false;
  bool vertex_flag = false;
  map<string, Locus>::iterator iter;
  
  /* Find columns. */
  for(iter = loci.begin(); iter != loci.end(); iter++) {
    Locus& locus = iter->second;
    locus.column = t.column(locus.heading);
    if(verbose) locus.display();
    if(locus.column < 0) continue;
    string tag = (locus.name() + " ");
    if(tag[0] == 'E') edge_flag = true;
    if(tag[0] == 'V') vertex_flag = true;
  }
  
  /* Check that necessary variables are available. */
  if(edge_flag) {
    if(t.column(cfg["EdgeHeadVariable"][1]) < 0)
      cout << "WARNING! Cannot find edge head variable in '" 
	   << t.source() << "'.\n";
    if(t.column(cfg["EdgeTailVariable"][1]) < 0)
      cout << "WARNING! Cannot find edge tail variable in '" 
	   << t.source() << "'.\n";
  }
  if(vertex_flag) {
    if(t.column(cfg["VertexNameVariable"][1]) < 0)
      cout << "WARNING! Cannot find vertex name variable in '" 
	   << t.source() << "'.\n";
  }
}
