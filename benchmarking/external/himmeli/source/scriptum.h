/* file: scriptum.h
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

#ifndef scriptum_INCLUDED
#define scriptum_INCLUDED

#include <string>
#include <vector>

namespace scriptum {

  /*
   * Parameters for graphics primitives.
   */
  struct Style {

    /* Text-anchor: 'left', 'right', 'middle'. */
    std::string anchor;

    /* Rotation angle about the element origin in degrees. */
    double angle;

    /* User-defined style definition. */
    std::string custom;

    /* Fill color in RGB[0, 1]. */
    std::vector<double> fillcolor;

    /* Fill opacity [0, 1]. */
    double fillopacity;

    /* Fill pattern within the current shape. */
    std::string fillpattern;

    /* Font size (cm). */
    double fontsize;

    /* Font family. */
    std::string fontfamily;

    /* Origin of the element coordinate system. */
    std::vector<double> origin;

    /* Stroke color in RGB[0, 1]. */
    std::vector<double> strokecolor;

    /* Stroke opacity [0, 1]. */
    double strokeopacity;

    /* Stroke width (cm). */
    double strokewidth;
   };

  /*
   * Parent class for graphics primitives.
   */
  class Artist {
  protected:
    void* buffer;
  public:
  
    /* Create an empty object. */
    Artist();

     /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    Artist(const Artist&);

    /* Create a new postscript object with the output file and
       format indicated by the argument. Use file name suffix '.svg'
       for Scalable Vector Graphics. */
    Artist(const std::string&);

    /* Free resources and close output file. */
    ~Artist();

    /* Transfer the contents of the argument to the calling object. Note that
       this does not correspond to duplicating the argument. */
    void operator=(const Artist&);

    /* Return the value of a parameter with the given name. If name is
       unknown, an empty string is returned. */
    std::string operator[](const std::string&);

    /* Assign a value (second argument) to a parameter (first argument).
       BackgroundColor        integer
       Creator                string
       FontSize               real
       FontFamily             string
       ForegroundColor        integer
       Format                 string
       PageSize               string
       SafeMode               'on' or 'off'
       Scale                  real
       VerboseMode            'on' or 'off' */
    bool assign(const std::string&, const std::string&);

    /* Close any output streams. */  
    bool close();

    /* Draw curve or shape. If the first and last points are equal, 
       the curve is closed. */
    bool curve(const std::vector<double>&, const std::vector<double>&,
	       const Style&);

    /* Return the output format. */
    std::string format();

    /* Start/stop an element group. If argument is true, a new group
       is started, if false the current active group is closed. The
       number of remaining open groups is returned (negative if
       grouping feature is not available). */
    int group(const bool);

    /* Draw shape. */
    bool shape(const double, const double, const double,
	       const std::string&, const Style&);
    
    /* Return current script length. */
    unsigned long size() const;

    /* Write text. */
    bool text(const double, const double, const std::string&,
	      const Style&);

    /* Version identification. */
    static std::string version();
  };
};

#endif /* scriptum_INCLUDED */

