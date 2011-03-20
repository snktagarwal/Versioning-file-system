/* file: colormap.cpp
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

    WWW: http://www.iki.fi/~vpmakine
*/

#include <stdlib.h>
#include <stdio.h>
#include "medusa.h"

using namespace std;

/*
 * Return a six-digit integer color code: RRGGBB based on a floating
 * point level within [0.0, 1.0].
 */
unsigned int
medusa::colormap(const string& name, const double hue) {
  double value = hue;
  if(value < 0.0) value = 0.0;
  if(value > 1.0) value = 1.0;

  /* Only the default colormap is currently available. */
  if(name != "default") {
    fprintf(stderr, "ERROR! %s: Unknown colormap '%s'.\n",
	    __FILE__, name.c_str());
    exit(1);
  }

  /* Model colors. */
  int clr000[] = {255, 240, 150};
  int clr025[] = {255, 220, 100}; 
  int clr050[] = {255, 170,  20}; 
  int clr075[] = {245,  90,   0}; 
  int clr100[] = {150,   5,   0}; 

  /* Find closest models. */
  double alpha = 0.0;
  double beta = 0.0;
  int* clrA = NULL;
  int* clrB = NULL;
  if(value < 0.25) {
    alpha = value; beta = (0.25 - value);
    clrA = clr000; clrB = clr025;
  } 
  if((value >= 0.25) & (value < 0.50)) {
    alpha = (value - 0.25); beta = (0.50 - value);
    clrA = clr025; clrB = clr050;
  }
  if((value >= 0.50) & (value < 0.75)) {
    alpha = (value - 0.50); beta = (0.75 - value);
    clrA = clr050; clrB = clr075;
  }
  if(value >= 0.75) {
    alpha = (value - 0.75); beta = (1.00 - value);
    clrA = clr075; clrB = clr100;
  }

  /* Interpolate current color. */
  double z = (alpha + beta);
  int clr[3];
  alpha /= z; beta /= z;
  clr[0] = (int)(alpha*clrB[0]/2.56 + beta*clrA[0]/2.56);
  clr[1] = (int)(alpha*clrB[1]/2.56 + beta*clrA[1]/2.56);
  clr[2] = (int)(alpha*clrB[2]/2.56 + beta*clrA[2]/2.56);
  return (clr[0]*10000 + clr[1]*100 + clr[2]);
}
