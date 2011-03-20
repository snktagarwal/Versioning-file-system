/* file: graphobject.h
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

#ifndef graphobject_INCLUDED
#define graphobject_INCLUDED

#include <map>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include "medusa.h"
#include "himmeli.h"
#include "tablet.h"
#include "scriptum.h"

#define graph_VERSION     "3.3.8"
#define MARGIN_WIDTH      1.5
#define NULL_FILTER_code  -1
#define ABS_FILTER_code   0
#define FRAC_FILTER_code  1
#define DEFAULT_WEIGHT    1.0
#define DEFAULT_WIDTH     0.09
#define DEFAULT_RADIUS    0.28
#define DEFAULT_SHAPE     "circle"
#define DEFAULT_COLORMAP  "default"

using namespace std;
using namespace himmeli;
using namespace tablet;
using namespace scriptum;
using namespace medusa;

struct Emblem {
  int color;
  float radius;
  string label;
  string pattern;
  string shape;
};

struct Arc {
  int color;
  float width;
  float weight;
  string label;
};

class CompareEdge {
public:
  bool operator()(const Edge& e1, const Edge& e2) const {
    return (e1.weight < e2.weight);
  };
};

class Locus {
private:
  string l_name;
public:
  int column;
  string heading;
  string file_name;
public:
  Locus() {
    l_name = "";
    heading = "";
    column = -1;
    file_name = "";
  };
  Locus(const string& tag, const Row& r) {
    l_name = tag;
    heading = r[1];
    column = -1;
    file_name = r[2];
    if(file_name.size() < 1) return;
    if(file_name[0] == '#') file_name = "";
  };
  void display() const {
    printf("\t%-22s ", l_name.c_str());
    if(heading.length() > 0) {
      string s = ("'" + file_name + "'");
      printf("%-16s ", s.c_str());
      s = ("'" + heading + "'");
      printf("%-16s ", s.c_str());
      if(column >= 0) printf("[%2d]\n", (column + 1));
      else printf("not found\n");
    }
    else
      printf("empty\n");
  };
  string name() const {return l_name;};
};

class GraphObject {
private:
  bool arrow_mode;
  bool decoration_mode;
  bool increment_mode;
  string label_mode;
  bool chassis_mode;
  bool verbose_mode;
  char delimiter;
  unsigned int figure_limit;
  float distance_unit;
  float time_limit;
  string edge_weight_transform;
  vector<float> edge_weight_filter;
  vector<float> edge_weight_mask;
  ConfigTable cfg;
  vector<Component> components;
  map<string, Arc> arcs;
  map<string, Emblem> emblems;
  map<string, Table> tables;
  map<string, vector<float> > coordinates;
  map<string, map<string, Locus> > variables;
  bool configure();
  bool decorate();
  bool import();
  bool print_info(Artist&);
  void print_links(Artist&, map<string, Vertex>&, vector<Edge>&);
  void print_nodes(Artist&, map<string, Vertex>&);
  unsigned long save_config(const string&, map<string, string>&);
  unsigned long save_edges(const string&, map<string, string>&, bool);
  unsigned long save_vertices(const string&, map<string, string>&);
  void normalize(vector<double>&, string, vector<float>);
public:
  GraphObject();
  GraphObject(const string&);
  Component operator[](const int);
  unsigned int cardinality();
  void print();
  void run();
  std::string name() {return cfg["GraphName"][1];};
  void save();
  unsigned int size();
};

#endif /* graphobject_INCLUDED */
