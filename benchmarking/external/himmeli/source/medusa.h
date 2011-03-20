/* file: medusa.h
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

#ifndef medusa_INCLUDED
#define medusa_INCLUDED

#include <string>
#include <vector>

namespace medusa {

  /* Basic statistics. */
  extern double statistic(const std::vector<double>&, const std::string&);
  
  /* Preprocessing. */
  //bool standize(std::vector<double>&, const std::string&);
  
  /* Value conversions. */
  extern std::string long2text(const long unsigned int);
  extern std::string double2string(const double);
  extern std::string string2safe(const std::string&, const unsigned int);
  extern double string2double(const std::string&);
  
  /* Sorting functions. */
  extern std::vector<unsigned int> sortdouble(std::vector<double>&);
  extern std::vector<unsigned int> sortstring(std::vector<std::string>&);
  //std::vector<unsigned int> uniq_doubles(std::vector<double>&);
  extern std::vector<unsigned int> uniqstring(std::vector<std::string>&);
  
  /* Color functions. */
  extern unsigned int colormap(const std::string&, const double);
  //std::string color2string(const unsigned int);
  extern std::vector<double> color2rgb(const int);
  
  /* Miscellaneous functions. */
  extern double nan();
  extern bool isempty(const std::vector<double>&);
  //std::string time2string();
};

#endif /* medusa_INCLUDED */
