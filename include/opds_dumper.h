/*
 * Copyright 2017 Matthieu Gautier <mgautier@kymeria.fr>
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

#ifndef KIWIX_OPDS_DUMPER_H
#define KIWIX_OPDS_DUMPER_H

#include <time.h>
#include <sstream>
#include <string>

#include <pugixml.hpp>

#include "library.h"
#include "reader.h"

using namespace std;

namespace kiwix
{

/**
 * A tool to dump a `Library` into a opds stream.
 *
 */
class OPDSDumper
{
 public:
  OPDSDumper() = default;
  OPDSDumper(Library* library);
  ~OPDSDumper();

  /**
   * Dump the OPDS feed.
   *
   * @param bookIds the ids of the books to include in the feed
   * @param query the query used to obtain the list of book ids
   * @return The OPDS feed.
   */
  std::string dumpOPDSFeed(const std::vector<std::string>& bookIds, const std::string& query) const;

  /**
   * Dump the OPDS feed.
   *
   * @param bookIds the ids of the books to include in the feed
   * @param query the query used to obtain the list of book ids
   * @return The OPDS feed.
   */
  std::string dumpOPDSFeedV2(const std::vector<std::string>& bookIds, const std::string& query) const;

  /**
   * Dump the categories OPDS feed.
   *
   * @param categories list of category names
   * @return The OPDS feed.
   */
  std::string categoriesOPDSFeed(const std::vector<std::string>& categories) const;

  /**
   * Dump the languages OPDS feed.
   *
   * @return The OPDS feed.
   */
  std::string languagesOPDSFeed() const;

  /**
   * Set the id of the library.
   *
   * @param id the id to use.
   */
  void setLibraryId(const std::string& id) { this->libraryId = id;}

  /**
   * Set the root location used when generating url.
   *
   * @param rootLocation the root location to use.
   */
  void setRootLocation(const std::string& rootLocation) { this->rootLocation = rootLocation; }

  /**
   * Set some informations about the search results.
   *
   * @param totalResult the total number of results of the search.
   * @param startIndex the start index of the result.
   * @param count the number of result of the current set (or page).
   */
  void setOpenSearchInfo(int totalResult, int startIndex, int count);

 protected:
  kiwix::Library* library;
  std::string libraryId;
  std::string rootLocation;
  int m_totalResults;
  int m_startIndex;
  int m_count;
};
}

#endif // KIWIX_OPDS_DUMPER_H
