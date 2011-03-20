/* file: componentobject.h
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

#ifndef componentobject_INCLUDED
#define componentobject_INCLUDED

#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "medusa.h"
#include "himmeli.h"

#define component_VERSION "1.0.2"
#define MAX_RC_DEPTH      10000

using namespace std;
using namespace himmeli;
using namespace medusa;

struct Link {
  int head;
  int tail;
  float weight;
};

struct Node {
  unsigned int tree_degree_in;
  unsigned int tree_degree_out;
  unsigned int chassis_degree_in;
  unsigned int chassis_degree_out;
  float x;
  float y;
  float z;
  string name;
  vector<Link> links_in;
  vector<Link> links_out;
};

class ComponentObject {
private:
  int center;
  int radius;
  unsigned long n_simu;
  vector<Node> nodes;
  map<string, string> parameters;
  void walk();
public:
  int n_chassis;
  ComponentObject() {};
  ComponentObject(const ComponentObject*);
  ComponentObject(const vector<Link>&, const vector<string>&, const double);
  bool assign(const string&, const string&);
  unsigned int cardinality() const {return nodes.size();}; 
  vector<Edge> chassis() const;
  vector<Edge> edges() const;
  unsigned long simulate(const float);
  unsigned int size() const {
    unsigned int i, n = 0;
    for(i = 0; i < nodes.size(); i++)
      n += (nodes[i].links_in).size();
    return n;
  };
  void update(const map<string, vector<float> >&);
  map<string, Vertex> vertices() const;
};

#endif /* componentobject_INCLUDED */
