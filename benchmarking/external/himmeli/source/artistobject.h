/* file: artistobject.h
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

#ifndef artistobject_INCLUDED
#define artistobject_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "medusa.h"
#include "scriptum.h"

#define scriptum_VERSION    "2.1.0"
#define CM2DOT              28.34645669 /* 72 dpi */
#define A0_WIDTH            84.10 /* cm */
#define A0_HEIGHT          118.90 /* cm */
#define A1_WIDTH            59.40 /* cm */
#define A1_HEIGHT           84.10 /* cm */
#define A2_WIDTH            42.00 /* cm */
#define A2_HEIGHT           59.40 /* cm */
#define A3_WIDTH            29.70 /* cm */
#define A3_HEIGHT           42.00 /* cm */
#define A4_WIDTH            21.00 /* cm */
#define A4_HEIGHT           29.70 /* cm */
#define A5_WIDTH            14.80 /* cm */
#define A5_HEIGHT           21.00 /* cm */
#define LETTER_WIDTH        21.59 /* cm */
#define LETTER_HEIGHT       27.94 /* cm */
#define SVG_format          "SVG"
#define CIRCLE_shape        "circle" /* circle */
#define TRIU_shape          "triup"   /* triangle with tip up */
#define CROSS_shape         "cross"  /* cross */
#define DIAMOND_shape       "diamond"  /* diamond */
#define STAR_shape          "star"    /* star */
#define TRIDOWN_shape       "tridown" /* triangle with tip down */
#define SQUARE_shape        "square"  /* square */
#define RCROSS_shape        "rcross" /* rotated cross */
#define SAFE_TXT_SIZE        16

using namespace std;
using namespace scriptum;
using namespace medusa;

/*
 * Parent implementation.
 */
class ArtistObject {
private:
  unsigned int nnames;
  unsigned long output_size;
  unsigned long file_size;
  char* buffer;
  FILE* output;
  string fname;
  map<string, string> parameters;
  double canvas_height();
  double canvas_width();
protected:
  unsigned int ngroups;
  string output_format;
  void _init(const string&);
  bool append(const string&);
  double height();
  string newname();
  double width();
public:
  ArtistObject() {};
  ArtistObject(const string& fname) {_init(fname);};
  ~ArtistObject();
  string operator[](const string&);
  bool assign(const string&, const string&);
  string retrieve(const string& name) {return parameters[name];};
  bool close();
  unsigned long size() const;
};

/*
 * Scalable Vector Graphics.
 */
class SVGObject: public ArtistObject {
private:
  vector<double> bgcolor();
  vector<double> fgcolor();
  string getpattern(double, double, double, string, string);
  string prolog();
  string style(const Style&);
public:
  SVGObject(const string& fname) {_init(fname);};
  bool curve(const std::vector<double>&, const std::vector<double>&,
	     const Style&);
  bool finish();
  int group(const bool);
  bool shape(const double, const double, const double,
	     const std::string&, const Style&);
  bool text(const double, const double, const std::string&,
	    const Style&);
};

#endif /* artistobject_INCLUDED */
