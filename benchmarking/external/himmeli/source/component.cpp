/* file: component.cpp
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

#include "componentobject.h"

/*
 *
 */
Component::Component() {
  buffer = new ComponentObject();
}

/*
 *
 */
Component::Component(const Component& comp) {
  buffer = new ComponentObject((ComponentObject*)(comp.buffer));
}

/*
 *
 */
Component::~Component() {
   ComponentObject* co = (ComponentObject*)buffer;
   delete co;
}

/*
 *
 */
void
Component::operator=(const Component& comp) {
  ComponentObject* co = (ComponentObject*)buffer;
  delete co;
  buffer = new ComponentObject((ComponentObject*)(comp.buffer));
}

/*
 *
 */
bool
Component::assign(const string& s1, const string& s2) {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->assign(s1, s2);
}

/*
 *
 */
unsigned int
Component::cardinality() const {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->cardinality();
}

/*
 *
 */
vector<Edge>
Component::chassis() const {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->chassis();
}

/*
 *
 */
vector<Edge>
Component::edges() const {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->edges();
}

/*
 *
 */
unsigned long
Component::simulate(const float limit) {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->simulate(limit);
}

/*
 *
 */
unsigned int
Component::size() const {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->size();
}

/*
 *
 */
void
Component::update(const map<string, vector<float> >& coord) {
  ComponentObject* co = (ComponentObject*)buffer;
  co->update(coord);
}

/*
 *
 */
map<string, Vertex>
Component::vertices() const {
  ComponentObject* co = (ComponentObject*)buffer;
  return co->vertices();
}

/*
 *
 */
string
Component::version() {
  string s = "";
  s += component_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
