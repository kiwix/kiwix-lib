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
#include <memory>
#include <vector>
#include "tools/pathTools.h"
#include "tools/stringTools.h"

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
  virtual std::string get_zimId() = 0;
};

struct SearcherInternal;
struct SuggestionInternal;
/**
 * The Searcher class is reponsible to do different kind of search using the
 * fulltext index.
 */
class Searcher
{
 public:
  /**
   * The default constructor.
   */
  Searcher();

  ~Searcher();

  /**
   * Add a reader (containing embedded fulltext index) to the search.
   *
   * @param reader The Reader for the zim containing the fulltext index.
   * @return true if the reader has been added.
   *         false if the reader cannot be added (no embedded fulltext index present)
   */
  bool add_reader(Reader* reader);


  Reader* get_reader(int index);

  /**
   * Start a search on the zim associated to the Searcher.
   *
   * Search results should be retrived using the getNextResult method.
   *
   * @param search The search query.
   * @param resultStart the start offset of the search results (used for pagination).
   * @param resultEnd the end offset of the search results (used for pagination).
   * @param verbose print some info on stdout if true.
   */
  void search(const std::string& search,
              unsigned int resultStart,
              unsigned int resultEnd,
              const bool verbose = false);

  /**
   * Start a geographique search.
   * The search return result for entry in a disc of center latitude/longitude
   * and radius distance.
   *
   * Search results should be retrived using the getNextResult method.
   *
   * @param latitude The latitude of the center point.
   * @param longitude The longitude of the center point.
   * @param distance The radius of the disc.
   * @param resultStart the start offset of the search results (used for pagination).
   * @param resultEnd the end offset of the search results (used for pagination).
   * @param verbose print some info on stdout if true.
   */
  void geo_search(float latitude, float longitude, float distance,
                  unsigned int resultStart,
                  unsigned int resultEnd,
                  const bool verbose = false);

  /**
   * Start a suggestion search.
   * The search made depend of the "version" of the embedded index.
   *  - If the index is newer enough and have a title namespace, the search is
   *    made in the titles only.
   *  - Else the search is made on the whole article content.
   * In any case, the search is made "partial" (as adding '*' at the end of the query)
   *
   * @param search The search query.
   * @param verbose print some info on stdout if true.
   */
  void suggestions(std::string& search, const bool verbose = false);

  /**
   * Get the next result of a started search.
   * This is the method to use to loop hover the search results.
   */
  Result* getNextResult();

  /**
   * Restart the previous search.
   * Next call to getNextResult will return the first result.
   */
  void restart_search();

  /**
   * Get a estimation of the result count.
   */
  unsigned int getEstimatedResultCount();

  unsigned int getResultStart() { return resultStart; }
  unsigned int getResultEnd() { return resultEnd; }

 protected:
  std::string beautifyInteger(const unsigned int number);
  void closeIndex();
  void searchInIndex(string& search,
                     const unsigned int resultStart,
                     const unsigned int resultEnd,
                     const bool verbose = false);

  std::vector<Reader*> readers;
  std::unique_ptr<SearcherInternal> internal;
  std::unique_ptr<SuggestionInternal> suggestionInternal;
  std::string searchPattern;
  unsigned int estimatedResultCount;
  unsigned int resultStart;
  unsigned int resultEnd;

 private:
  void reset();

};


}

#endif
