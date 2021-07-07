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

#ifndef KIWIX_READER_H
#define KIWIX_READER_H

#include <stdio.h>
#include <zim/zim.h>
#include <zim/archive.h>
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include "common.h"
#include "entry.h"

using namespace std;

namespace kiwix
{

/**
 * The SuggestionItem is a helper class that contains the info about a single
 * suggestion item.
 */
class SuggestionItem
{
  // Functions
  // Temporarily making the constructor public until the code move is complete
  public:
    // Create a sugggestion item.
    explicit SuggestionItem(const std::string& title, const std::string& normalizedTitle,
                            const std::string& path, const std::string& snippet = "") :
      title(title),
      normalizedTitle(normalizedTitle),
      path(path),
      snippet(snippet) {}

  public:
    const std::string& getTitle() const           { return title;}
    const std::string& getNormalizedTitle() const { return normalizedTitle;}
    const std::string& getPath() const            { return path;}
    const std::string& getSnippet() const         { return snippet;}

    bool hasSnippet() const                { return !snippet.empty();}

  // Data
  private:
    std::string title;
    std::string normalizedTitle;
    std::string path;
    std::string snippet;

  friend class Reader;
};

/**
 * The Reader class is the class who allow to get an entry content from a zim
 * file.
 */

using SuggestionsList_t = std::vector<SuggestionItem>;
class Reader
{
 public:
  /**
   * Create a Reader to read a zim file specified by zimFilePath.
   *
   * @param zimFilePath The path to the zim file to read.
   *                    The zim file can be splitted (.zimaa, .zimab, ...).
   *                    In this case, the file path must still point to the
   *                    unsplitted path as if the file were not splitted
   *                    (.zim extesion).
   */
  explicit Reader(const string zimFilePath);

   /**
   * Create a Reader to read a zim file given by the Archive.
   *
   * @param archive The shared pointer to the Archive object.
   */
  explicit Reader(const std::shared_ptr<zim::Archive> archive);
#ifndef _WIN32
  explicit Reader(int fd);
  Reader(int fd, zim::offset_type offset, zim::size_type size);
#endif
  ~Reader() = default;

  /**
   * Get the number of "displayable" entries in the zim file.
   *
   * @return If the zim file has a /M/Counter metadata, return the number of
   *         entries with the 'text/html' MIMEtype specified in the metadata.
   *         Else return the number of entries in the 'A' namespace.
   */
  unsigned int getArticleCount() const;

  /**
   * Get the number of media in the zim file.
   *
   * @return If the zim file has a /M/Counter metadata, return the number of
   *         entries with the 'image/jpeg', 'image/gif' and 'image/png' in
   *         the metadata.
   *         Else return the number of entries in the 'I' namespace.
   */
  unsigned int getMediaCount() const;

  /**
   * Get the number of all entries in the zim file.
   *
   * @return Return the number of all the entries, whatever their MIMEtype or
   *         their namespace.
   */
  unsigned int getGlobalCount() const;

  /**
   * Get the path of the zim file.
   *
   * @return the path of the zim file as given in the constructor.
   */
  string getZimFilePath() const;

  /**
   * Get the Id of the zim file.
   *
   * @return The uuid stored in the zim file.
   */
  string getId() const;

  /**
   * Get a random page.
   *
   * @return A random Entry. The entry is picked from all entries in
   *         the 'A' namespace.
   *         The main entry is excluded from the potential results.
   */
  Entry getRandomPage() const;

  /**
   * Get the entry of the main page.
   *
   * @return Entry of the main page as specified in the zim file.
   */
  Entry getMainPage() const;

  /**
   * Get the content of a metadata.
   *
   * @param[in]  name The name of the metadata.
   * @param[out] value The value will be set to the content of the metadata.
   * @return True if it was possible to get the content of the metadata.
   */
  bool getMetadata(const string& name, string& value) const;

  /**
   * Get the name of the zim file.
   *
   * @return The name of the zim file as specified in the zim metadata.
   */
  string getName() const;

  /**
   * Get the title of the zim file.
   *
   * @return The title of zim file as specified in the zim metadata.
   *         If no title has been set, return a title computed from the
   *         file path.
   */
  string getTitle() const;

  /**
   * Get the creator of the zim file.
   *
   * @return The creator of the zim file as specified in the zim metadata.
   */
  string getCreator() const;

  /**
   * Get the publisher of the zim file.
   *
   * @return The publisher of the zim file as specified in the zim metadata.
   */
  string getPublisher() const;

  /**
   * Get the date of the zim file.
   *
   * @return The date of the zim file as specified in the zim metadata.
   */
  string getDate() const;

  /**
   * Get the description of the zim file.
   *
   * @return The description of the zim file as specified in the zim metadata.
   *         If no description has been set, return the subtitle.
   */
  string getDescription() const;

  /**
   * Get the long description of the zim file.
   *
   * @return The long description of the zim file as specifed in the zim metadata.
   */
  string getLongDescription() const;

  /**
   * Get the language of the zim file.
   *
   * @return The language of the zim file as specified in the zim metadata.
   */
  string getLanguage() const;

  /**
   * Get the license of the zim file.
   *
   * @return The license of the zim file as specified in the zim metadata.
   */
  string getLicense() const;

  /**
   * Get the tags of the zim file.
   *
   * @param original If true, return the original tags as specified in the zim metadata.
   *                 Else, try to convert it to the new 'normalized' format.
   * @return The tags of the zim file.
   */
  string getTags(bool original=false) const;

  /**
   * Get the value (as a string) of a specific tag.
   *
   * According to https://wiki.openzim.org/wiki/Tags
   *
   * @return The value of the specified tag.
   * @throw  std::out_of_range if the specified tag is not found.
   */
  string getTagStr(const std::string& tagName) const;

  /**
   * Get the boolean value of a specific tag.
   *
   * According to https://wiki.openzim.org/wiki/Tags
   *
   * @return The boolean value of the specified tag.
   * @throw  std::out_of_range if the specified tag is not found.
   *         std::domain_error if the value of the tag cannot be convert to bool.
   */
  bool getTagBool(const std::string& tagName) const;

  /**
   * Get the relations of the zim file.
   *
   * @return The relation of the zim file as specified in the zim metadata.
   */
  string getRelation() const;

  /**
   * Get the flavour of the zim file.
   *
   * @return The flavour of the zim file as specified in the zim metadata.
   */
  string getFlavour() const;

  /**
   * Get the source of the zim file.
   *
   * @return The source of the zim file as specified in the zim metadata.
   */
  string getSource() const;

  /**
   * Get the scraper of the zim file.
   *
   * @return The scraper of the zim file as specified in the zim metadata.
   */
  string getScraper() const;

  /**
   * Get the favicon of the zim file.
   *
   * @param[out] content The content of the favicon.
   * @param[out] mimeType The mimeType of the favicon.
   * @return True if a favicon has been found.
   */
  bool getFavicon(string& content, string& mimeType) const;

  /**
   * Get an entry associated to an path.
   *
   * @param path The path of the entry.
   * @return The entry.
   * @throw NoEntry If no entry correspond to the path.
   */
  Entry getEntryFromPath(const std::string& path) const;

  /**
   * Get an entry associated to an url encoded path.
   *
   * Equivalent to `getEntryFromPath(urlDecode(path));`
   *
   * @param path The url encoded path.
   * @return The entry.
   * @throw NoEntry If no entry correspond to the path.
   */
  Entry getEntryFromEncodedPath(const std::string& path) const;

  /**
   * Get un entry associated to a title.
   *
   * @param title The title.
   * @return The entry
   * throw NoEntry If no entry correspond to the url.
   */
  Entry getEntryFromTitle(const std::string& title) const;

  /**
   * Search for entries with title starting with prefix (case sensitive).
   *
   * Suggestions are stored in an internal vector and can be retrieved using
   * `getNextSuggestion` method.
   * This method is not thread safe and is deprecated. Use :
   * bool searchSuggestions(const string& prefix,
   *                        unsigned int suggestionsCount,
   *                        SuggestionsList_t& results);
   *
   * @param prefix The prefix to search.
   * @param suggestionsCount How many suggestions to search for.
   * @param reset If true, remove previous suggestions in the internal vector.
   *              If false, add suggestions to the internal vector
   *              (until internal vector size is suggestionCount (or no more
   *               suggestion))
   * @return True if some suggestions have been added to the internal vector.
   */
  DEPRECATED bool searchSuggestions(const string& prefix,
                         unsigned int suggestionsCount,
                         const bool reset = true);

  /**
   * Search for entries with title starting with prefix (case sensitive).
   *
   * Suggestions are added to the `result` vector.
   *
   * @param prefix The prefix to search.
   * @param suggestionsCount How many suggestions to search for.
   * @param result The vector where to store the suggestions.
   * @return True if some suggestions have been added to the vector.
   */

  bool searchSuggestions(const string& prefix,
                         unsigned int suggestionsCount,
                         SuggestionsList_t& resuls);

  /**
   * Search for entries for the given prefix.
   *
   * If the zim file has a internal fulltext index, the suggestions will be
   * searched using it.
   * Else the suggestions will be search using `searchSuggestions` while trying
   * to be smart about case sensitivity (using `getTitleVariants`).
   *
   * In any case, suggestions are stored in an internal vector and can be
   * retrieved using `getNextSuggestion` method.
   * The internal vector will be reset.
   * This method is not thread safe and is deprecated. Use :
   * bool searchSuggestionsSmart(const string& prefix,
   *                             unsigned int suggestionsCount,
   *                             SuggestionsList_t& results);
   *
   * @param prefix The prefix to search for.
   * @param suggestionsCount How many suggestions to search for.
   */
  DEPRECATED bool searchSuggestionsSmart(const string& prefix,
                              unsigned int suggestionsCount);

  /**
   * Search for entries for the given prefix.
   *
   * If the zim file has a internal fulltext index, the suggestions will be
   * searched using it.
   * Else the suggestions will be search using `searchSuggestions` while trying
   * to be smart about case sensitivity (using `getTitleVariants`).
   *
   * In any case, suggestions are stored in an internal vector and can be
   * retrieved using `getNextSuggestion` method.
   * The internal vector will be reset.
   *
   * @param prefix The prefix to search for.
   * @param suggestionsCount How many suggestions to search for.
   * @param results The vector where to store the suggestions
   * @return True if some suggestions have been added to the results.
   */
   bool searchSuggestionsSmart(const string& prefix,
                              unsigned int suggestionsCount,
                              SuggestionsList_t& results);


  /**
   * Check if the path exists in the zim file.
   *
   * @param path the path to check.
   * @return True if the path exists in the zim file.
   */
  bool pathExists(const string& path) const;

  /**
   * Check if the zim file has a embedded fulltext index.
   *
   * @return True if the zim file has a embedded fulltext index
   *         and is not split (else the fulltext is not accessible).
   */
  bool hasFulltextIndex() const;

  /**
   * Get potential case title variations for a title.
   *
   * @param title a title.
   * @return the list of variantions.
   */
  std::vector<std::string> getTitleVariants(const std::string& title) const;

  /**
   * Get the next suggestion title.
   *
   * @param[out] title the title of the suggestion.
   * @return True if title has been set.
   */
  DEPRECATED bool getNextSuggestion(string& title);

  /**
   * Get the next suggestion title and url.
   *
   * @param[out] title the title of the suggestion.
   * @param[out] url the url of the suggestion.
   * @return True if title and url have been set.
   */
  DEPRECATED bool getNextSuggestion(string& title, string& url);

  /**
   * Get if we can check zim file integrity (has a checksum).
   *
   * @return True if zim file have a checksum.
   */
  bool canCheckIntegrity() const;

  /**
   * Check is zim file is corrupted.
   *
   * @return True if zim file is corrupted.
   */
  bool isCorrupted() const;

  /**
   * Return the total size of the zim file.
   *
   * If zim file is split, return the sum of all parts' size.
   *
   * @return Size of the size file is KiB.
   */
  unsigned int getFileSize() const;

  /**
   * Get the zim file handler.
   *
   * @return The libzim file handler.
   */
  zim::Archive* getZimArchive() const;

 protected:
  std::shared_ptr<zim::Archive> zimArchive;
  std::string zimFilePath;

  SuggestionsList_t suggestions;
  SuggestionsList_t::iterator suggestionsOffset;

 private:
  std::map<const std::string, unsigned int> parseCounterMetadata() const;
};
}

#endif
