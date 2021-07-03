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

#include "reader.h"
#include <time.h>

#include <zim/search.h>
#include <zim/item.h>
#include <zim/error.h>

#include "tools.h"
#include "tools/stringTools.h"
#include "tools/otherTools.h"
#include "tools/archiveTools.h"

namespace kiwix
{
/* Constructor */
Reader::Reader(const string zimFilePath)
  :  zimArchive(nullptr),
     zimFilePath(zimFilePath)
{
  string tmpZimFilePath = zimFilePath;

  /* Remove potential trailing zimaa */
  size_t found = tmpZimFilePath.rfind("zimaa");
  if (found != string::npos && tmpZimFilePath.size() > 5
      && found == tmpZimFilePath.size() - 5) {
    tmpZimFilePath.resize(tmpZimFilePath.size() - 2);
  }

  zimArchive.reset(new zim::Archive(tmpZimFilePath));

  /* initialize random seed: */
  srand(time(nullptr));
}

Reader::Reader(const std::shared_ptr<zim::Archive> archive)
  : zimArchive(archive),
    zimFilePath(archive->getFilename())
  {}

#ifndef _WIN32
Reader::Reader(int fd)
  :  zimArchive(new zim::Archive(fd)),
     zimFilePath("")
{
  /* initialize random seed: */
  srand(time(nullptr));
}

Reader::Reader(int fd, zim::offset_type offset, zim::size_type size)
  :  zimArchive(new zim::Archive(fd, offset, size)),
     zimFilePath("")
{
  /* initialize random seed: */
  srand(time(nullptr));
}
#endif // #ifndef _WIN32

zim::Archive* Reader::getZimArchive() const
{
  return zimArchive.get();
}

MimeCounterType Reader::parseCounterMetadata() const
{
  return kiwix::parseArchiveCounter(*zimArchive);
}

/* Get the count of articles which can be indexed/displayed */
unsigned int Reader::getArticleCount() const
{
  std::map<const std::string, unsigned int> counterMap
      = this->parseCounterMetadata();
  unsigned int counter = 0;

  for(auto &pair:counterMap) {
    if (startsWith(pair.first, "text/html")) {
      counter += pair.second;
    }
  }

  return counter;
}

/* Get the count of medias content in the ZIM file */
unsigned int Reader::getMediaCount() const
{
  return kiwix::getArchiveMediaCount(*zimArchive);
}

/* Get the total of all items of a ZIM file, redirects included */
unsigned int Reader::getGlobalCount() const
{
  return zimArchive->getEntryCount();
}

/* Return the UID of the ZIM file */
string Reader::getId() const
{
  return kiwix::getArchiveId(*zimArchive);
}

Entry Reader::getRandomPage() const
{
  try {
    return zimArchive->getRandomEntry();
  } catch(...) {
    throw NoEntry();
  }
}

Entry Reader::getMainPage() const
{
  return zimArchive->getMainEntry();
}

bool Reader::getFavicon(string& content, string& mimeType) const
{
  return kiwix::getArchiveFavicon(*zimArchive, content, mimeType);
}

string Reader::getZimFilePath() const
{
  return zimFilePath;
}
/* Return a metatag value */
bool Reader::getMetadata(const string& name, string& value) const
{
  try {
    value = zimArchive->getMetadata(name);
    return true;
  } catch(zim::EntryNotFound& e) {
    return false;
  }
}

#define METADATA(NAME) std::string v; getMetadata(NAME, v); return v;

string Reader::getName() const
{
  return kiwix::getMetaName(*zimArchive);
}

string Reader::getTitle() const
{
  return kiwix::getArchiveTitle(*zimArchive);
}

string Reader::getCreator() const
{
  return kiwix::getMetaCreator(*zimArchive);
}

string Reader::getPublisher() const
{
  return kiwix::getMetaPublisher(*zimArchive);
}

string Reader::getDate() const
{
  return kiwix::getMetaDate(*zimArchive);
}

string Reader::getDescription() const
{
  return kiwix::getMetaDescription(*zimArchive);
}

string Reader::getLongDescription() const
{
  METADATA("LongDescription")
}

string Reader::getLanguage() const
{
  return kiwix::getMetaLanguage(*zimArchive);
}

string Reader::getLicense() const
{
  METADATA("License")
}

string Reader::getTags(bool original) const
{
  return kiwix::getMetaTags(*zimArchive, original);
}


string Reader::getTagStr(const std::string& tagName) const
{
  string tags_str;
  getMetadata("Tags", tags_str);
  return getTagValueFromTagList(convertTags(tags_str), tagName);
}

bool Reader::getTagBool(const std::string& tagName) const
{
  return convertStrToBool(getTagStr(tagName));
}

string Reader::getRelation() const
{
  METADATA("Relation")
}

string Reader::getFlavour() const
{
  return kiwix::getMetaFlavour(*zimArchive);
}

string Reader::getSource() const
{
  METADATA("Source")
}

string Reader::getScraper() const
{
  METADATA("Scraper")
}
#undef METADATA

string Reader::getOrigId() const
{
  return kiwix::getArchiveOrigId(*zimArchive);
}

Entry Reader::getEntryFromPath(const std::string& path) const
{
  try {
    return kiwix::getEntryFromPath(*zimArchive, path);
  } catch (zim::EntryNotFound& e) {
    throw NoEntry();
  }
}

Entry Reader::getEntryFromEncodedPath(const std::string& path) const
{
  return getEntryFromPath(urlDecode(path, true));
}

Entry Reader::getEntryFromTitle(const std::string& title) const
{
  try {
    return zimArchive->getEntryByTitle(title);
  } catch(zim::EntryNotFound& e) {
    throw NoEntry();
  }
}

bool Reader::pathExists(const string& path) const
{
  return zimArchive->hasEntryByPath(path);
}

/* Does the ZIM file has a fulltext index */
bool Reader::hasFulltextIndex() const
{
  return zimArchive->hasFulltextIndex();
}

/* Search titles by prefix */

bool Reader::searchSuggestions(const string& prefix,
                               unsigned int suggestionsCount,
                               const bool reset)
{
  /* Reset the suggestions otherwise check if the suggestions number is less
   * than the suggestionsCount */
  if (reset) {
    this->suggestions.clear();
    this->suggestionsOffset = this->suggestions.begin();
  } else {
    if (this->suggestions.size() > suggestionsCount) {
      return false;
    }
  }

  auto ret =  searchSuggestions(prefix, suggestionsCount, this->suggestions);

  /* Set the cursor to the begining */
  this->suggestionsOffset = this->suggestions.begin();

  return ret;
}


bool Reader::searchSuggestions(const string& prefix,
                               unsigned int suggestionsCount,
                               SuggestionsList_t& results)
{
  bool retVal = false;

  /* Return if no prefix */
  if (prefix.size() == 0) {
    return false;
  }

  for (auto& entry: zimArchive->findByTitle(prefix)) {
    if (results.size() >= suggestionsCount) {
      break;
    }
    /* Extract the interesting part of article title & url */
    std::string normalizedArticleTitle
        = kiwix::normalize(entry.getTitle());

    // Get the final path.
    auto item = entry.getItem(true);
    std::string articleFinalUrl = item.getPath();

    /* Go through all already found suggestions and skip if this
       article is already in the suggestions list (with an other
       title) */
    bool insert = true;
    std::vector<SuggestionItem>::iterator suggestionItr;
    for (suggestionItr = results.begin();
         suggestionItr != results.end();
         suggestionItr++) {
      int result = normalizedArticleTitle.compare((*suggestionItr).getNormalizedTitle());
      if (result == 0 && articleFinalUrl.compare((*suggestionItr).getPath()) == 0) {
        insert = false;
        break;
      } else if (result < 0) {
        break;
      }
    }

    /* Insert if possible */
    if (insert) {
      SuggestionItem suggestion(entry.getTitle(), normalizedArticleTitle, articleFinalUrl);
      results.insert(suggestionItr, suggestion);
    }

    /* Suggestions where found */
    retVal = true;
  }

  return retVal;
}

std::vector<std::string> Reader::getTitleVariants(
    const std::string& title) const
{
  return kiwix::getTitleVariants(title);
}


bool Reader::searchSuggestionsSmart(const string& prefix,
                                    unsigned int suggestionsCount)
{
  this->suggestions.clear();
  this->suggestionsOffset = this->suggestions.begin();

  auto ret = searchSuggestionsSmart(prefix, suggestionsCount, this->suggestions);

  this->suggestionsOffset = this->suggestions.begin();

  return ret;
}

/* Try also a few variations of the prefix to have better results */
bool Reader::searchSuggestionsSmart(const string& prefix,
                                    unsigned int suggestionsCount,
                                    SuggestionsList_t& results)
{
  std::vector<std::string> variants = this->getTitleVariants(prefix);
  bool retVal = false;

  /* Try to search in the title using fulltext search database */

  auto suggestionSearcher = zim::Searcher(*zimArchive);
  zim::Query suggestionQuery;
  suggestionQuery.setQuery(prefix, true);
  auto suggestionSearch = suggestionSearcher.search(suggestionQuery);

  if (suggestionSearch.getEstimatedMatches()) {
    const auto suggestions = suggestionSearch.getResults(0, suggestionsCount);
    for (auto current = suggestions.begin();
         current != suggestions.end();
         current++) {
      SuggestionItem suggestion(current.getTitle(), kiwix::normalize(current.getTitle()),
                                current.getPath(), current.getSnippet());
      results.push_back(suggestion);
    }
    retVal = true;
  } else {
    for (std::vector<std::string>::iterator variantsItr = variants.begin();
         variantsItr != variants.end();
         variantsItr++) {
      retVal = this->searchSuggestions(*variantsItr, suggestionsCount, results)
               || retVal;
    }
  }

  return retVal;
}

/* Get next suggestion */
bool Reader::getNextSuggestion(string& title)
{
  if (this->suggestionsOffset != this->suggestions.end()) {
    /* title */
    title = (*(this->suggestionsOffset)).getTitle();

    /* increment the cursor for the next call */
    this->suggestionsOffset++;

    return true;
  }

  return false;
}

bool Reader::getNextSuggestion(string& title, string& url)
{
  if (this->suggestionsOffset != this->suggestions.end()) {
    /* title */
    title = (*(this->suggestionsOffset)).getTitle();
    url = (*(this->suggestionsOffset)).getPath();

    /* increment the cursor for the next call */
    this->suggestionsOffset++;

    return true;
  }

  return false;
}

/* Check if the file has as checksum */
bool Reader::canCheckIntegrity() const
{
  return zimArchive->hasChecksum();
}

/* Return true if corrupted, false otherwise */
bool Reader::isCorrupted() const
{
  try {
    if (zimArchive->check() == true) {
      return false;
    }
  } catch (exception& e) {
    cerr << e.what() << endl;
    return true;
  }

  return true;
}

/* Return the file size, works also for splitted files */
unsigned int Reader::getFileSize() const
{
  return kiwix::getArchiveFileSize(*zimArchive);
}

}
