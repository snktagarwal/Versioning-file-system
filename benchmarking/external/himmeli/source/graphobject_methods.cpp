/* file: graphobject_methods.cpp
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
Component
GraphObject::operator[](const int i) {
  int n = components.size();
  if(i < 0) return Component();
  if(i >= n) return Component();
  return components[n];
}

/*
 *
 */
unsigned int
GraphObject::cardinality() {
  unsigned int i;
  unsigned int n = 0;
  for(i = 0; i < components.size(); i++) 
    n += components[i].cardinality();
  return n;
}

/*
 *
 */
void
GraphObject::run() {
  unsigned int i;
  unsigned int n_links = size();
  time_t start = time(NULL);

  /* Copy existing coordinates if any. */
  if(n_links < 1) return;
  for(i = 0; i < components.size(); i++)
    if(!increment_mode) components[i].simulate(-1);
    else components[i].update(coordinates);
  if(time_limit < FLT_EPSILON) return;
  
  /* Configure components. */
  for(i = 0; i < components.size(); i++) {
    if(chassis_mode) components[i].assign("ChassisMode", "on");
    else components[i].assign("ChassisMode", "off");
    if(verbose_mode) components[i].assign("VerboseMode", "on");
    else components[i].assign("VerboseMode", "off");
  }

  /* Compute layout. */
  if(verbose_mode) cout << "\nComputing layout:\n";
  for(i = 0; i < components.size(); i++) {
    time_t now = time(NULL);
    float grace = time_limit*(components[i].size())/n_links;
    unsigned long n = components[i].simulate(grace);
    if(verbose_mode) { 
      cout << "\t[" << (i + 1) << "]\t" << long2text(n) << '\t' 
	   << "iterations in " << difftime(time(NULL), now) << "s\n";
    }
  }
  if(verbose_mode) { 
    cout << "\tLayout computed in "
	 << difftime(time(NULL), start) << "s.\n";
  }
}

/*
 *
 */
unsigned int
GraphObject::size() {
  unsigned int i;
  unsigned int n_links = 0;
  for(i = 0; i < components.size(); i++) 
    n_links += components[i].size();
  return n_links;
}
