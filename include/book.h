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

#ifndef KIWIX_BOOK_H
#define KIWIX_BOOK_H

#include <string>

namespace pugi {
class xml_node;
}

namespace zim {
class Archive;
}

namespace kiwix
{

class OPDSDumper;
class Reader;

/**
 * A class to store information about a book (a zim file)
 */
class Book
{
 public:
  Book();
  ~Book();

  bool update(const Book& other);
  void update(const Reader& reader);
  void update(const zim::Archive& archive);
  void updateFromXml(const pugi::xml_node& node, const std::string& baseDir);
  void updateFromOpds(const pugi::xml_node& node, const std::string& urlHost);
  std::string getHumanReadableIdFromPath() const;

  bool readOnly() const { return m_readOnly; }
  const std::string& getId() const { return m_id; }
  const std::string& getPath() const { return m_path; }
  bool isPathValid() const { return m_pathValid; }
  const std::string& getTitle() const { return m_title; }
  const std::string& getDescription() const { return m_description; }
  const std::string& getLanguage() const { return m_language; }
  const std::string& getCreator() const { return m_creator; }
  const std::string& getPublisher() const { return m_publisher; }
  const std::string& getDate() const { return m_date; }
  const std::string& getUrl() const { return m_url; }
  const std::string& getName() const { return m_name; }
  std::string getCategory() const;
  const std::string& getTags() const { return m_tags; }
  std::string getTagStr(const std::string& tagName) const;
  bool getTagBool(const std::string& tagName) const;
  const std::string& getFlavour() const { return m_flavour; }
  const std::string& getOrigId() const { return m_origId; }
  const uint64_t& getArticleCount() const { return m_articleCount; }
  const uint64_t& getMediaCount() const { return m_mediaCount; }
  const uint64_t& getSize() const { return m_size; }
  const std::string& getFavicon() const;
  const std::string& getFaviconUrl() const { return m_faviconUrl; }
  const std::string& getFaviconMimeType() const { return m_faviconMimeType; }
  const std::string& getDownloadId() const { return m_downloadId; }

  void setReadOnly(bool readOnly) { m_readOnly = readOnly; }
  void setId(const std::string& id) { m_id = id; }
  void setPath(const std::string& path);
  void setPathValid(bool valid) { m_pathValid = valid; }
  void setTitle(const std::string& title) { m_title = title; }
  void setDescription(const std::string& description) { m_description = description; }
  void setLanguage(const std::string& language) { m_language = language; }
  void setCreator(const std::string& creator) { m_creator = creator; }
  void setPublisher(const std::string& publisher) { m_publisher = publisher; }
  void setDate(const std::string& date) { m_date = date; }
  void setUrl(const std::string& url) { m_url = url; }
  void setName(const std::string& name) { m_name = name; }
  void setFlavour(const std::string& flavour) { m_flavour = flavour; }
  void setTags(const std::string& tags) { m_tags = tags; }
  void setOrigId(const std::string& origId) { m_origId = origId; }
  void setArticleCount(uint64_t articleCount) { m_articleCount = articleCount; }
  void setMediaCount(uint64_t mediaCount) { m_mediaCount = mediaCount; }
  void setSize(uint64_t size) { m_size = size; }
  void setFavicon(const std::string& favicon) { m_favicon = favicon; }
  void setFaviconMimeType(const std::string& faviconMimeType) { m_faviconMimeType = faviconMimeType; }
  void setDownloadId(const std::string& downloadId) { m_downloadId = downloadId; }

 private:
  std::string getCategoryFromTags() const;

 protected:
  std::string m_id;
  std::string m_downloadId;
  std::string m_path;
  bool m_pathValid = false;
  std::string m_title;
  std::string m_description;
  std::string m_category;
  std::string m_language;
  std::string m_creator;
  std::string m_publisher;
  std::string m_date;
  std::string m_url;
  std::string m_name;
  std::string m_flavour;
  std::string m_tags;
  std::string m_origId;
  uint64_t m_articleCount = 0;
  uint64_t m_mediaCount = 0;
  bool m_readOnly = false;
  uint64_t m_size = 0;
  mutable std::string m_favicon;
  std::string m_faviconUrl;
  std::string m_faviconMimeType;
};

}

#endif
