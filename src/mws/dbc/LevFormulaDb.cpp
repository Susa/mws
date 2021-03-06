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
  * @file LevFormulaDb.cpp
  * @brief Formula Memory Database implementation
  * @author Radu Hambasan
  * @date 11 Dec 2013
  */

#include <stdlib.h>

#include <memory>
using std::unique_ptr;
#include <stdexcept>
using std::runtime_error;
#include <string>
using std::string;
using std::to_string;
#include <leveldb/db.h>
using leveldb::DestroyDB;
using leveldb::DB;
using leveldb::Options;
using leveldb::Status;
using leveldb::ReadOptions;
using leveldb::Iterator;

#include "common/types/Parcelable.hpp"
using common::types::ParcelAllocator;
using common::types::ParcelEncoder;
using common::types::ParcelDecoder;
#include "mws/dbc/LevFormulaDb.hpp"

namespace mws {
namespace dbc {

LevFormulaDb::LevFormulaDb() : mDatabase(nullptr), mCounter(0) {}

LevFormulaDb::~LevFormulaDb() { delete mDatabase; }

/** @throw runtime_error */
void LevFormulaDb::open(const char* path) {
    Options options;
    options.create_if_missing = false;
    Status status = DB::Open(options, path, &mDatabase);

    if (!status.ok()) {
        throw runtime_error(status.ToString());
    }
}

/** @throw runtime_error */
void LevFormulaDb::create_new(const char* path, bool deleteIfExists) {
    if (deleteIfExists) {
        (void)DestroyDB(path, Options());
    }

    Options options;
    options.error_if_exists = true;
    options.create_if_missing = true;
    Status status = DB::Open(options, path, &mDatabase);
    if (!status.ok()) {
        throw runtime_error(status.ToString());
    }
}

int LevFormulaDb::insertFormula(const types::FormulaId& formulaId,
                                const CrawlId& crawlId,
                                const types::FormulaPath& formulaPath) {
    ++mCounter;

    std::string crawlId_str = std::to_string(crawlId);
    ParcelAllocator allocator;
    allocator.reserve(crawlId_str);
    allocator.reserve(formulaPath);
    ParcelEncoder encoder(allocator);
    encoder.encode(crawlId_str);
    encoder.encode(formulaPath);

    std::string key =
        std::to_string(formulaId) + "!" + std::to_string(mCounter);
    std::string value(encoder.getData(), encoder.getSize());

    leveldb::Status status =
        mDatabase->Put(leveldb::WriteOptions(), key, value);

    if (!status.ok()) return -1;

    return 0;
}

int LevFormulaDb::queryFormula(const types::FormulaId& formulaId,
                               unsigned limitMin, unsigned limitSize,
                               QueryCallback queryCallback) {
    unique_ptr<Iterator> it(mDatabase->NewIterator(ReadOptions()));
    string fmId = to_string(formulaId) + "!";
    it->Seek(fmId);
    for (unsigned i = 0; i < limitMin; i++) {
        if (!it->Valid()) return 0;
        it->Next();
    }

    if (!it->Valid()) return 0;

    std::string maxKey = fmId + "\xff";
    for (unsigned i = 0;
         i < limitSize && it->Valid() && it->key().ToString() < maxKey;
         i++, it->Next()) {
        std::string retrieved = it->value().ToString();

        ParcelDecoder decoder(retrieved.data(), retrieved.size());

        std::string crawlId_str;
        decoder.decode(&crawlId_str);

        CrawlId crawlId = strtoul(crawlId_str.data(), nullptr, 0);
        types::FormulaPath formulaPath;
        decoder.decode(&formulaPath);

        if (queryCallback(crawlId, formulaPath) != 0) return -1;
    }

    return 0;
}

}  // namespace dbc
}  // namespace mws
