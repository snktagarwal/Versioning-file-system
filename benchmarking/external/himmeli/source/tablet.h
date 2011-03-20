/* file: tablet.h
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

#ifndef tablet_INCLUDED
#define tablet_INCLUDED

#include <string>
#include <vector>

namespace tablet {

  /*
   * Basic data record.
   */
  class Row {
  protected:
    std::vector<char> buffer;
  public:

    /* Compare two rows character by character. */
    bool operator==(const Row&) const;
    bool operator!=(const Row&) const;

    /* Return the ith field. If argument is less than zero or more than
       (size() - 1) empty string is returned.  */
    std::string operator[](const int) const;

    /* Number of characters. */
    unsigned int mem_size() const;

    /* Floating point value of ith field. If the value is
       illegible, a constant is returned (see nan()). Both the
       comma ',' and period '.' are interpreted as the radix point. */
    double number(const int) const;

    /* Row index in the original file. */
    int rank() const;

    /* Number of fields. */
    unsigned int size() const;

    /* Not-a-number constant. */
    static double nan();
  };

  /*
   * Database representation of a text file.
   */
  class Table {
  protected:
    void* buffer;
  public:

    /* Create empty table. */
    Table();

    /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    Table(const Table&);

    /* Create a new table from the file pointed by the first argument.
       The second argument specifies the delimiter, use '\0' for
       white space. */
    Table(const std::string&, const char);

    /* Free resources. */
    ~Table();

   /* Transfer the contents of the argument to the calling object. Note that
      this does not correspond to duplicating the argument. */
    void operator=(const Table&);

    /* Return the ith row. */
    Row operator[](const int) const;

    /* Find the column with the given heading (i.e. field in the
       first row). If none found, a negative values is returned. */
    int column(const std::string&) const;

    /* Find the first occurrence of the first argument in the column
       indicated by the second argument. Note that the table must be sorted
       by the given column. If none found, a negative value is returned. */
    int find(const std::string&, const int) const;

    /* Find the first occurence of a row that matches the fields given by
       the first argument, in the columns given by the second argument.
       Note that the table must be properly sorted. */
    std::vector<int> find(const std::vector<std::string>&,
			  const std::vector<int>&) const;

    /* Return the total numer of characters consumed by the table. */
    unsigned int mem_size() const;

    /* Unique identification. */
    unsigned int id() const;

    /* Save contents into a text file. Use the empty string to print to
       standard output. The second argument specifies the delimiter. */
    unsigned long print(const std::string, const char) const;

    /* Number of rows. */
    unsigned int size() const;

    /* Sort by every column, starting from the first. */
    void sort();

    /* Sort by columns indicated by the argument.
       Invalid indices are ignored. */
    void sort(const std::vector<int>&);

    /* Name of the source file. */
    std::string source() const;

    /* String comparison function used by the sort routines. This does
       not work in the same way as the standard C/C++ comparators in every
       situation. */
    static int strcmp(const std::string&, const std::string&);

    /* Version identification. */
    static std::string version();
  };

  /*
   * Database representation of a text file, tailored for
   * configuration files.
   */
  class ConfigTable: public Table {
  public:

    /* Create an empty table. */
    ConfigTable();

    /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    ConfigTable(const ConfigTable&);

    /* Create a new table from the file pointed by the first argument.
       The second argument specifies the delimiter, use '\0' for
       white space.*/
    ConfigTable(const std::string&, const char);

    /* Return the row with the argument as first field. If none found, 
       an empty row is returned. */
    Row operator[](const std::string&) const;

    /* Return the jth row with the first argument as the first
       field (the first match has j = 0). If none found, an empty string is
       returned. */
    Row get(const std::string&, const int) const;

    /* Find which alternative of a list of strings matches with the
      jth field of the row pointed by the first argument. The second argument
      specifies the number of strings that are listed by subsequent
      arguments. If no match is found, a negative value is returned.*/
    int ordinal(const std::string&, const int, ...) const;
  };

  /*
   * Database representation of a character string.
   */
  class StringTable: public Table {
  public:
    /* Create a new table from a string (first argument). The second
       argument specifies the delimiter, use '\0' for white space.*/
    StringTable(const std::string&, const char);
  };
};

#endif /* tablet_INCLUDED */




