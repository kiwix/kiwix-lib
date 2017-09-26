/*
 * Copyright 2011 Emmanuel Engelhart <kelson@kiwix.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef KIWIX_SEARCHER_H
#define KIWIX_SEARCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <unicode/putil.h>
#include <algorithm>
#include <cctype>
#include <locale>
#include <string>
#include <vector>
#include <vector>
#include "common/pathTools.h"
#include "common/stringTools.h"
#include "kiwix_config.h"

using namespace std;

namespace kiwix
{
class Reader;
class Result
{
 public:
  virtual ~Result(){};
  virtual std::string get_url() = 0;
  virtual std::string get_title() = 0;
  virtual int get_score() = 0;
  virtual std::string get_snippet() = 0;
  virtual std::string get_content() = 0;
  virtual int get_wordCount() = 0;
  virtual int get_size() = 0;
  virtual int get_readerIndex() = 0;
};

struct SearcherInternal;
class Searcher
{
 public:
  Searcher();
  Searcher(const string& xapianDirectoryPath,
           Reader* reader,
           const string& humanReadableName);
  ~Searcher();

  void add_reader(Reader* reader, const std::string& humanReaderName);
  void search(std::string& search,
              unsigned int resultStart,
              unsigned int resultEnd,
              const bool verbose = false);
  void suggestions(std::string& search, const bool verbose = false);
  Result* getNextResult();
  void restart_search();
  unsigned int getEstimatedResultCount();
  bool setProtocolPrefix(const std::string prefix);
  bool setSearchProtocolPrefix(const std::string prefix);
  void reset();

#ifdef ENABLE_CTPP2
  string getHtml();
#endif

 protected:
  std::string beautifyInteger(const unsigned int number);
  void closeIndex();
  void searchInIndex(string& search,
                     const unsigned int resultStart,
                     const unsigned int resultEnd,
                     const bool verbose = false);

  std::vector<Reader*> readers;
  std::vector<std::string> humanReaderNames;
  SearcherInternal* internal;
  std::string searchPattern;
  std::string protocolPrefix;
  std::string searchProtocolPrefix;
  unsigned int resultCountPerPage;
  unsigned int estimatedResultCount;
  unsigned int resultStart;
  unsigned int resultEnd;
  std::string contentHumanReadableId;
};
}

#endif
