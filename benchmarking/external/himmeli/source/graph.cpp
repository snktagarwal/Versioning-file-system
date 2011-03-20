/* file: graph.cpp
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
Graph::Graph() {
  buffer = new GraphObject();
}

/*
 *
 */
Graph::Graph(const Graph& g) {
  Graph* pg = (Graph*)(&g);
  buffer = pg->buffer;
  pg->buffer = new GraphObject();
}

/*
 *
 */
Graph::Graph(const string& fname) {
  buffer = new GraphObject(fname);
}

/*
 *
 */
Graph::~Graph() {
   GraphObject* po = (GraphObject*)buffer;
   delete po;
}

/*
 *
 */
void
Graph::operator=(const Graph& g) {
  Graph* pg = (Graph*)(&g);
  void* ptr = buffer;
  buffer = pg->buffer;
  pg->buffer = ptr;
};

/*
 *
 */
Component
Graph::operator[](const int i) {
  GraphObject* po = (GraphObject*)buffer;
  return (*po)[i];
}

/*
 *
 */
unsigned int
Graph::cardinality() {
  GraphObject* po = (GraphObject*)buffer;
  return po->cardinality();
}

/*
 *
 */
string
Graph::name() {
  GraphObject* po = (GraphObject*)buffer;
  return po->name();
}

/*
 *
 */
void
Graph::run() {
  GraphObject* po = (GraphObject*)buffer;
  if(po->cardinality() < 2) return; 
  po->run();
  po->print();
  po->save();
}

/*
 *
 */
unsigned int
Graph::size() {
  GraphObject* po = (GraphObject*)buffer;
  return po->size();
}

/*
 *
 */
string
Graph::version() {
  string s = "";
  s += graph_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
