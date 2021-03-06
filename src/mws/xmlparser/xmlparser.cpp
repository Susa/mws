/*

Copyright (C) 2010-2013 KWARC Group <kwarc.info>

This file is part of MathWebSearch.

MathWebSearch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MathWebSearch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MathWebSearch.  If not, see <http://www.gnu.org/licenses/>.

*/
/**
  * @file xmlparser.cpp
  * @author Corneliu-Claudiu Prodescu
  * @date 05 May 2011
  *
  *
  */

#include <stdlib.h>
#include <libxml/parser.h>

#include "mws/xmlparser/xmlparser.hpp"

namespace mws {
namespace parser {

int initxmlparser() {
    // Initializing the library and checking potential ABI mismatches between
    // the version it was compiled for and the actual shared library used.
    LIBXML_TEST_VERSION;

    // Register xmlCleanupParser to be called at program exit
    return atexit(xmlCleanupParser);
}

}  // namespace parser
}  // namespace mws
