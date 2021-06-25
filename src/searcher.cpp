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


#include <cmath>

#include "searcher.h"
#include "reader.h"

#include <zim/search.h>

#include <mustache.hpp>
#include "kiwixlib-resources.h"

#define MAX_SEARCH_LEN 140

namespace kiwix
{
class _Result : public Result
{
 public:
  _Result(zim::SearchResultSet::iterator iterator);
  _Result(zim::SuggestionResultSet::iterator iterator);
  virtual ~_Result(){};

  virtual std::string get_url();
  virtual std::string get_title();
  virtual int get_score();
  virtual std::string get_snippet();
  virtual std::string get_content();
  virtual int get_wordCount();
  virtual int get_size();
  virtual std::string get_zimId();

  bool isSuggestion() { return suggestionMode; }

 private:
  zim::SearchResultSet::iterator iterator;
  zim::SuggestionResultSet::iterator suggestionIterator;
  bool suggestionMode;
};

struct SearcherInternal : zim::SearchResultSet {
  explicit SearcherInternal(const zim::SearchResultSet& srs)
    : zim::SearchResultSet(srs)
    , current_iterator(srs.begin())
  {
  }

  zim::SearchResultSet::iterator current_iterator;
};

struct SuggestionInternal : zim::SuggestionResultSet {
  explicit SuggestionInternal(const zim::SuggestionResultSet& suggestionResultSet)
  : zim::SuggestionResultSet(suggestionResultSet),
    current_iterator(suggestionResultSet.begin())
  {}

  zim::SuggestionResultSet::iterator current_iterator;
};

/* Constructor */
Searcher::Searcher()
    : searchPattern(""),
      estimatedResultCount(0),
      resultStart(0),
      resultEnd(0)
{
  loadICUExternalTables();
}

/* Destructor */
Searcher::~Searcher()
{
}

bool Searcher::add_reader(Reader* reader)
{
  if (!reader->hasFulltextIndex()) {
      return false;
  }
  this->readers.push_back(reader);
  return true;
}


Reader* Searcher::get_reader(int readerIndex)
{
  return readers.at(readerIndex);
}

/* Search strings in the database */
void Searcher::search(const std::string& search,
                      unsigned int resultStart,
                      unsigned int resultEnd,
                      const bool verbose)
{
  this->reset();

  if (verbose == true) {
    cout << "Performing query `" << search << "'" << endl;
  }

  this->searchPattern = search;
  this->resultStart = resultStart;
  this->resultEnd = resultEnd;
  /* Try to find results */
  if (resultStart != resultEnd) {
    /* Perform the search */
    string unaccentedSearch = removeAccents(search);
    std::vector<zim::Archive> archives;
    for (auto current = this->readers.begin(); current != this->readers.end();
         current++) {
      if ( (*current)->hasFulltextIndex() ) {
          archives.push_back(*(*current)->getZimArchive());
      }
    }
    zim::Searcher searcher(archives);
    zim::Query query;
    query.setQuery(unaccentedSearch);
    query.setVerbose(verbose);
    zim::Search search = searcher.search(query);
    internal.reset(new SearcherInternal(search.getResults(resultStart, resultEnd)));
    this->estimatedResultCount = search.getEstimatedMatches();
  }

  return;
}


void Searcher::geo_search(float latitude, float longitude, float distance,
                          unsigned int resultStart,
                          unsigned int resultEnd,
                          const bool verbose)
{
  this->reset();

  if (verbose == true) {
    cout << "Performing geo query `" << distance << "&(" << latitude << ";" << longitude << ")'" << endl;
  }

  /* Perform the search */
  std::ostringstream oss;
  oss << "Articles located less than " << distance << " meters of " << latitude << ";" << longitude;
  this->searchPattern = oss.str();
  this->resultStart = resultStart;
  this->resultEnd = resultEnd;

  /* Try to find results */
  if (resultStart == resultEnd) {
    return;
  }

  std::vector<zim::Archive> archives;
  for (auto current = this->readers.begin(); current != this->readers.end();
       current++) {
    archives.push_back(*(*current)->getZimArchive());
  }
  zim::Searcher searcher(archives);
  zim::Query query;
  query.setVerbose(verbose);
  query.setQuery("");
  query.setGeorange(latitude, longitude, distance);
  zim::Search search = searcher.search(query);
  internal.reset(new SearcherInternal(search.getResults(resultStart, resultEnd)));
  this->estimatedResultCount = search.getEstimatedMatches();
}


void Searcher::restart_search()
{
  if (internal.get()) {
    internal->current_iterator = internal->begin();
  }
}

Result* Searcher::getNextResult()
{
  if (internal.get() &&
             internal->current_iterator != internal->end()) {
    Result* result = new _Result(internal->current_iterator);
    internal->current_iterator++;
    return result;
  }
  return NULL;
}

/* Reset the results */
void Searcher::reset()
{
  this->estimatedResultCount = 0;
  this->searchPattern = "";
  return;
}

void Searcher::suggestions(std::string& searchPattern, const bool verbose)
{
  this->reset();

  if (verbose == true) {
    cout << "Performing suggestion query `" << searchPattern << "`" << endl;
  }

  this->searchPattern = searchPattern;
  this->resultStart = 0;
  this->resultEnd = 10;
  string unaccentedSearch = removeAccents(searchPattern);

  std::vector<zim::Archive> archives;
  for (auto current = this->readers.begin(); current != this->readers.end();
       current++) {
    archives.push_back(*(*current)->getZimArchive());
  }

  // MULTIZIM SUGGESTION IS NOT SUPPORTED CURRENTLY! Taking only the first archive.
  zim::SuggestionSearcher suggestionSearcher(archives[0]);
  zim::Query query;
  query.setVerbose(verbose);
  query.setQuery(unaccentedSearch);
  zim::SuggestionSearch suggestionSearch = suggestionSearcher.suggest(query);
  suggestionInternal.reset(new SuggestionInternal(suggestionSearch.getResults(resultStart, resultEnd)));
  this->estimatedResultCount = suggestionSearch.getEstimatedMatches();
}

/* Return the result count estimation */
unsigned int Searcher::getEstimatedResultCount()
{
  return this->estimatedResultCount;
}

_Result::_Result(zim::SearchResultSet::iterator iterator)
    : iterator(iterator),
      suggestionMode(false)
{
}

_Result::_Result(zim::SuggestionResultSet::iterator suggestionIterator)
    : suggestionIterator(suggestionIterator),
      suggestionMode(true)
{
}

/**
 * Some fo the following getFoo methods are not applicable for suggestion results
 * as of now due to the separate libzim API and simple nature of its dereference,
 * but the methods are not altered here in libkiwix for compatibility and for the
 * reason of enforcing dropping the usage of wrapper structures like _Result soon.
 **/

std::string _Result::get_url()
{
  if (suggestionMode) {
    return suggestionIterator->getPath();
  }
  return iterator.getPath();
}
std::string _Result::get_title()
{
  if (suggestionMode) {
    return suggestionIterator->getTitle();
  }
  return iterator.getTitle();
}
int _Result::get_score()
{
  if (suggestionMode) {
    return 0;
  }
  return iterator.getScore();
}
std::string _Result::get_snippet()
{
  if (suggestionMode) {
    return suggestionIterator->getSnippet();
  }
  return iterator.getSnippet();
}
std::string _Result::get_content()
{
  if (suggestionMode) {
    return "";
  }
  return iterator->getItem(true).getData();
}
int _Result::get_size()
{
  if (suggestionMode) {
    return 0;
  }
  return iterator.getSize();
}
int _Result::get_wordCount()
{
  if (suggestionMode) {
    return 0;
  }
  return iterator.getWordCount();
}
std::string _Result::get_zimId()
{
  if (suggestionMode) {
    return "";
  }
  std::ostringstream s;
  s << iterator.getZimId();
  return s.str();
}


}
