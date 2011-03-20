/* file: himmeli.h
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

#ifndef himmeli_INCLUDED
#define himmeli_INCLUDED

#include <map>
#include <string>
#include <vector>
#include "scriptum.h"

namespace himmeli {

  /*
   * Input format for graph topology. 
   */
  struct Edge {

    /* Name of the sink vertex. */
    std::string head;

    /* Name of the source vertex. */
    std::string tail;

    /* Link strength. */
    float weight;
  };

  /*
   * Output format of node layout.
   */
  struct Vertex {

    /* Number of adjacent edges directed at the vertex. */
    unsigned int degree_in;

    /* Number of adjacent edges directed away from the vertex. */
    unsigned int degree_out;

    /* Sum of inward edge weights. */
    float strength_in;

    /* Sum of outward edge weights. */
    float strength_out;

    /* Vertex coordinates. */
    float x;
    float y;
    float z;
  };

  /*
   * Largest connected subgraph.
   */
  class Component {
  private:
    void* buffer;
  public:

    /* Create an empty component. */
    Component();

    /* Copy constructor. Creates a complete duplicate. */
    Component(const Component&);

    /* Free all resources associated with the component. */
    ~Component();

    /* Copy contents of the argument to the calling object.
       The old contents are discarded. */
    void operator=(const Component&);

    /* Assign runtime parameters: 
       ChassisMode    on/off
       VerboseMode    on/off . */
    bool assign(const std::string&, const std::string&);

    /* Maximum spanning (expanded) tree. */
    std::vector<Edge> backbone(const int) const;

    /* Number of vertices. */
    unsigned int cardinality() const;

    /* Reduced component topology. At least the maximum spanning tree
       is returned to ensure connectivity. */
    std::vector<Edge> chassis() const;

    /* Component topology. */
    std::vector<Edge> edges() const;

    /* Compute vertex layout (time limit in seconds). */
    unsigned long simulate(const float);

    /* Number of edges. */
    unsigned int size() const;

    /* Assing coordinates to vertices. Vertices are identified by their
       name. Coordinates are read from the float vectors as [x][y][z]. */
    void update(const std::map<std::string, std::vector<float> >&);

    /* Return vertex layout. */
    std::map<std::string, Vertex> vertices() const;

    /* Break a graph to its components. The integer argument defines
       the cardinality of the "chassis" of the graph. */
    static std::vector<Component> create(const std::vector<Edge>&,
					 const double);

    /* Version identification. */
    static std::string version();
  };


  /*
   * User interface and miscellaneous program logic.
   */
  class Graph {
  private:
    void* buffer;
  public:

    /* Create an empty graph. */
    Graph();

    /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    Graph(const Graph&);

    /* Create a new graph as instructed in the configuration file.
       The argument specificies the path to the file. */
    Graph(const std::string&);

    /* Free all resources associated with the graph. */
    ~Graph();

    /* Swap the contents of the argument and the calling object. Note that
       this does not correspond to duplicating the argument. */
    void operator=(const Graph&);

    /* Return a new component object with the given index. The indices 
       are equal to the ranking by component cardinality (0 for the largest,
       n-1 for the smallest).  If none is found, an empty component is
       returned. */
    Component operator[](const int);

    /* Number of vertices. */
    unsigned int cardinality();

    /* Graph name. */
    std::string name();

    /* Compute vertex layouts and print results, as instructed by the
       configuration file. */
    void run();

    /* Number of edges. */
    unsigned int size();

    /* Version identification. */
    static std::string version();
  };
};

#endif /* himmeli_INCLUDED */
