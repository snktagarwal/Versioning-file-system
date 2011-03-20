Himmeli v3.2 API

The source code is organized into a few independent modules that communicate
through streamlined object interfaces. For many classes there is one
simplified definition that calls a more complicated hidden object. This is to
avoid cluttering the header files with unnecessary member declarations.

The naming of the files reveals their purpose. For example, 'himmeli.h'
contains the simplified class definition for 'Graph', but the actual class
'GraphObject' that does most of the work is defined in 'graphobject.h'. 
A member method is designated by dot, e.g. 'graphobject.configure.cpp'
contains the implementation of 'configure' method of 'GraphObject'. 
Sometimes it is convenient to collect several methods to one file, denoted by
'_methods.cpp' suffix. Similarly, non-member functions are denoted by
'_functions.cpp'.

Below are the important header files that are documented:

'himmeli.h'
namespaces:    himmeli
structs:       Edge, Vertex
classes:       Component, Graph

'tablet.h'
namespaces:    tablet
structs:
classes:       Row, Table, ConfigTable

'scriptum.h'
namespaces:    scriptum
structs:       Style
classes:       Artist

Overall, the code should be legible, although the documentation is mostly on
the micromanagerial level. If anyone is enthusiastic enough to modify, improve
or clarify the software structure, I would be most happy to hear from it.

-- Ville Makinen
