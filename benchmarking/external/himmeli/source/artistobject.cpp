/* file: artistobject.cpp
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

#include "artistobject.h"

#define BUFFER_CAP 131072

/*
 *
 */
void
ArtistObject::_init(const string& s) {
  output_size = 0;
  buffer = NULL;
  file_size = 0;
  fname = s;
  output = NULL;
  nnames = 0;
  ngroups = 0;

  /* Default parameters. */
  parameters["BackgroundColor"] = "999999";
  parameters["Creator"] = "";
  parameters["FontSize"] = "0.4";
  parameters["FontFamily"] = "FreeSans";
  parameters["ForegroundColor"] = "000000";
  parameters["Format"] = "";
  parameters["PageSize"] = "";
  parameters["SafeMode"] = "off";
  parameters["Scale"] = "1.0";
  parameters["VerboseMode"] = "off";
}

/*
 *
 */
ArtistObject::~ArtistObject() {
  close();
}

/*
 *
 */
string
ArtistObject::operator[](const string& s) {
  if(s == "FileName") return fname;
  if(parameters.count(s) > 0) return parameters[s];
  return "";
}

/*
 *
 */
bool
ArtistObject::append(const string& s) {
  if((output == NULL) && (file_size == 0)) {
    if((output = fopen(fname.c_str(), "w")) == NULL) {
      printf("WARNING! Cannot open '%s'.\n", fname.c_str());
      return false;
    }
    buffer = (char*)malloc(BUFFER_CAP);
    setvbuf(output, buffer, _IOFBF, BUFFER_CAP);
  }
  if(output == NULL) return false;
  output_size += fprintf(output, "%s", s.c_str());
  return true;
}

/*
 *
 */
bool
ArtistObject::assign(const string& s, const string& value) {
  if((output_size > 0) || (file_size > 0)) {
    printf("WARNING! Parameters cannot be set for existing document.\n");
    return false;
  }
  if(value == "") return false;
  if(parameters.count(s) < 1) return false;
  parameters[s] = value;
  return true;
}

/*
 *
 */
double
ArtistObject::canvas_height() {
  double h = -1.0;
  string s = parameters["PageSize"];
  if(s == "a0") h = A0_HEIGHT;
  if(s == "a1") h = A1_HEIGHT;
  if(s == "a2") h = A2_HEIGHT;
  if(s == "a3") h = A3_HEIGHT;
  if(s == "a4") h = A4_HEIGHT;
  if(s == "a5") h = A5_HEIGHT;
  if(s == "letter") h = LETTER_HEIGHT;
  if(h < 0.0) {
    char* ptr = (char*)(s.c_str());
    if((ptr = strchr(ptr, ',')) != NULL) h = atof(ptr + 1);
  }
  if(h < 1.0) h = 1.0;
  return h;
}

/*
 *
 */
double
ArtistObject::canvas_width() {
  double w = -1.0;
  string s = parameters["PageSize"];
  if(s == "a0") w = A0_WIDTH;
  if(s == "a1") w = A1_WIDTH;
  if(s == "a2") w = A2_WIDTH;
  if(s == "a3") w = A3_WIDTH;
  if(s == "a4") w = A4_WIDTH;
  if(s == "a5") w = A5_WIDTH;
  if(s == "letter") w = LETTER_WIDTH;
  if(w < 0.0) w = atof(s.c_str());
  if(w < 1.0) w = 1.0;
  return w;
}

/*
 *
 */
bool
ArtistObject::close() {
  if(output == NULL) return false;
  file_size = output_size;
  fclose(output);
  free(buffer);
  output_size = 0;
  buffer = NULL;
  output = NULL;
  return true;
}

/*
 *
 */
double
ArtistObject::height() {
  if(parameters["PageOrientation"] == "landscape")
    return canvas_width();
  return canvas_height();
}

/*
 *
 */
unsigned long
ArtistObject::size() const {
  if(output_size == 0) return file_size;
  return output_size;
}

/*
 *
 */
string
ArtistObject::newname() {
  char buf[128];
  sprintf(buf, "e%04d", ++nnames);
  return string(buf);
}

/*
 *
 */
double
ArtistObject::width() {
  if(parameters["PageOrientation"] == "landscape")
    return canvas_height();
  return canvas_width();
}
