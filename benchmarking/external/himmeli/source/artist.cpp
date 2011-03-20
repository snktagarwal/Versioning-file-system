/* file: artist.cpp
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

#include "scriptum.h"
#include "artistobject.h"

using namespace std;
using namespace scriptum;

/*
 *
 */
Artist::Artist() {
  buffer = new ArtistObject("");
}

/*
 *
 */
Artist::Artist(const Artist& a) {
  Artist* pa = (Artist*)(&a);
  buffer = pa->buffer;
  pa->buffer = new ArtistObject("");
}

/*
 *
 */
Artist::Artist(const string& fname) {
  string suffix = ("    " + fname);
  suffix = suffix.substr((suffix.size() - 4), 4);
  if(suffix == ".svg") {
    buffer = new SVGObject(fname);
    SVGObject* po = (SVGObject*)buffer;
    po->assign("Format", SVG_format);
  }
  if(buffer == NULL) buffer = new ArtistObject("");
}

/*
 *
 */
Artist::~Artist() {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    delete psvg;
    return;
  }
  delete po;
}

/*
 *
 */
void
Artist::operator=(const Artist& a) {
  Artist* pa = (Artist*)(&a);
  void* ptr = buffer;
  buffer = pa->buffer;
  pa->buffer = ptr;
}

/*
 *
 */
string
Artist::operator[](const string& s) {
  ArtistObject* po = (ArtistObject*)buffer;
  return (*po)[s];
}

/*
 *
 */
bool
Artist::assign(const string& s, const string& value) {
  ArtistObject* po = (ArtistObject*)buffer;
  if(s == "Format") return false;
  return po->assign(s, value);
}

/*
 *
 */
bool
Artist::close() {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    psvg->finish();
  }
  return po->close();
}

/*
 *
 */
bool
Artist::curve(const vector<double>& vx, const vector<double>& vy,
      const Style& sty) {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    return psvg->curve(vx, vy, sty);
  }
  return false;  
}

/*
 *
 */
int
Artist::group(const bool flag) {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    return psvg->group(flag);
  }
  return -1;
}

/*
 *
 */
bool
Artist::shape(const double x, const double y, const double r,
      const std::string& name, const Style& sty) {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    return psvg->shape(x, y, r, name, sty);
  }
  return false;  
}

/*
 *
 */
unsigned long
Artist::size() const {
  ArtistObject* po = (ArtistObject*)buffer;
  return po->size();
}

/*
 *
 */
bool
Artist::text(const double x, const double y, const std::string& s,
	     const Style& sty) {
  ArtistObject* po = (ArtistObject*)buffer;
  if((*po)["Format"] == SVG_format) {
    SVGObject* psvg = (SVGObject*)buffer;
    return psvg->text(x, y, s, sty);
  }
  return false;
}

/*
 *
 */ 
string
Artist::version() {
  string s = "";
  s += scriptum_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
