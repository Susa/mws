/*

Copyright (C) 2010-2014 KWARC Group <kwarc.info>

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
  * @brief Test if <m:ci> token renaming maintains data integrity
  *
  * @file ci_renaming.cpp
  * @author Radu Hambasan
  * @date 07 Apr 2014
  *
  * License: GPL v3
  *
  */

// System includes

#include <sys/types.h>      // Primitive System datatypes
#include <sys/stat.h>       // POSIX File characteristics
#include <fcntl.h>          // File control operations
#include <libxml/parser.h>  // LibXML parser header
#include <stdlib.h>

#include <string>
using std::string;
#include <utility>

// Local includes

#include "mws/dbc/MemCrawlDb.hpp"
using mws::dbc::MemCrawlDb;
#include "mws/dbc/MemFormulaDb.hpp"
using mws::dbc::MemFormulaDb;
#include "mws/index/MeaningDictionary.hpp"
using mws::index::MeaningDictionary;
#include "mws/index/IndexBuilder.hpp"
using mws::index::IndexBuilder;
using mws::index::ExpressionEncoder;
using mws::parser::HarvestResult;
#include "mws/index/TmpIndex.hpp"
using mws::index::TmpIndex;
#include "mws/xmlparser/xmlparser.hpp"
using mws::parser::initxmlparser;
#include "mws/dbc/MemCrawlDb.hpp"
#include "mws/dbc/MemFormulaDb.hpp"
#include "common/utils/compiler_defs.h"

#include "build-gen/config.h"

// Namespaces

/*
 * In the test harvest there are 2 <expr>'s:
 * x + y
 * and
 * a + b
 * we expect to get 8 expressions
 * and the index size to be only 4
 */

struct Tester {
    static inline bool ci_renaming_successful() {
        MemCrawlDb crawlDb;
        MemFormulaDb formulaDb;
        TmpIndex data;
        MeaningDictionary meaningDictionary;
        ExpressionEncoder::Config indexingOptions;
        indexingOptions.renameCi = true;
        HarvestResult result;
        IndexBuilder indexBuilder(&formulaDb, &crawlDb, &data,
                                         &meaningDictionary, indexingOptions);
        const string harvest_path =
            (string)MWS_TESTDATA_PATH + "/ci_renaming.harvest";
        int fd;
        FAIL_ON(initxmlparser() != 0);
        FAIL_ON((fd = open(harvest_path.c_str(), O_RDONLY)) < 0);
        result = loadHarvestFromFd(&indexBuilder, fd);
        // Fail if the parsing was not sucessful
        FAIL_ON(result.status != 0);
        // Fail if we have not indexed all expresions
        FAIL_ON(result.numExpressions != 8);
        // Fail if the index is not as compressed as it should
        FAIL_ON(data.mRoot->children.size() != 4);
        (void)close(fd);

        return true;

    fail:
        return false;
    }
};

int main() {
    if (Tester::ci_renaming_successful()) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
