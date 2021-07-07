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

#include "book.h"
#include "reader.h"

#include "tools.h"
#include "tools/base64.h"
#include "tools/regexTools.h"
#include "tools/networkTools.h"
#include "tools/otherTools.h"
#include "tools/stringTools.h"
#include "tools/pathTools.h"
#include "tools/archiveTools.h"

#include <zim/archive.h>

#include <pugixml.hpp>

namespace kiwix
{
/* Constructor */
Book::Book() :
  m_pathValid(false),
  m_readOnly(false)
{
}
/* Destructor */
Book::~Book()
{
}

bool Book::update(const kiwix::Book& other)
{
  if (m_readOnly)
    return false;

  if (m_id != other.m_id)
    return false;

  m_readOnly = other.m_readOnly;
  m_path = other.m_path;
  m_pathValid = other.m_pathValid;
  m_title = other.m_title;
  m_description = other.m_description;
  m_language = other.m_language;
  m_creator = other.m_creator;
  m_publisher = other.m_publisher;
  m_date = other.m_date;
  m_url = other.m_url;
  m_name = other.m_name;
  m_flavour = other.m_flavour;
  m_tags = other.m_tags;
  m_category = other.m_category;
  m_origId = other.m_origId;
  m_articleCount = other.m_articleCount;
  m_mediaCount = other.m_mediaCount;
  m_size = other.m_size;
  m_favicon = other.m_favicon;
  m_faviconMimeType = other.m_faviconMimeType;
  m_faviconUrl = other.m_faviconUrl;

  m_downloadId = other.m_downloadId;

  return true;
}

void Book::update(const kiwix::Reader& reader)
{
  update(*reader.getZimArchive());
}

void Book::update(const zim::Archive& archive) {
  m_path = archive.getFilename();
  m_pathValid = true;
  m_id = getArchiveId(archive);
  m_title = getArchiveTitle(archive);
  m_description = getMetaDescription(archive);
  m_language = getMetaLanguage(archive);
  m_creator = getMetaCreator(archive);
  m_publisher = getMetaPublisher(archive);
  m_date = getMetaDate(archive);
  m_name = getMetaName(archive);
  m_flavour = getMetaFlavour(archive);
  m_tags = getMetaTags(archive);
  m_category = getCategoryFromTags();
  m_articleCount = archive.getArticleCount();
  m_mediaCount = getArchiveMediaCount(archive);
  m_size = static_cast<uint64_t>(getArchiveFileSize(archive)) << 10;

  getArchiveFavicon(archive, m_favicon, m_faviconMimeType);
}

#define ATTR(name) node.attribute(name).value()
void Book::updateFromXml(const pugi::xml_node& node, const std::string& baseDir)
{
  m_id = ATTR("id");
  std::string path = ATTR("path");
  if (isRelativePath(path)) {
    path = computeAbsolutePath(baseDir, path);
  }
  m_path = path;
  m_pathValid = fileExists(path);
  m_title = ATTR("title");
  m_description = ATTR("description");
  m_language = ATTR("language");
  m_creator = ATTR("creator");
  m_publisher = ATTR("publisher");
  m_date = ATTR("date");
  m_url = ATTR("url");
  m_name = ATTR("name");
  m_flavour = ATTR("flavour");
  m_tags = ATTR("tags");
  m_origId = ATTR("origId");
  m_articleCount = strtoull(ATTR("articleCount"), 0, 0);
  m_mediaCount = strtoull(ATTR("mediaCount"), 0, 0);
  m_size = strtoull(ATTR("size"), 0, 0) << 10;
  m_favicon = base64_decode(ATTR("favicon"));
  m_faviconMimeType = ATTR("faviconMimeType");
  m_faviconUrl = ATTR("faviconUrl");
  try {
    m_downloadId = ATTR("downloadId");
  } catch(...) {}
  const auto catattr = node.attribute("category");
  m_category = catattr.empty() ? getCategoryFromTags() : catattr.value();
}
#undef ATTR


static std::string fromOpdsDate(const std::string& date)
{
  //The opds date use the standard <YYYY>-<MM>-<DD>T<HH>:<mm>:<SS>Z
  //and we want <YYYY>-<MM>-<DD>. That's easy, let's take the first 10 char
  return date.substr(0, 10);
}


#define VALUE(name) node.child(name).child_value()
void Book::updateFromOpds(const pugi::xml_node& node, const std::string& urlHost)
{
  m_id = VALUE("id");
  if (!m_id.compare(0, 9, "urn:uuid:")) {
    m_id.erase(0, 9);
  }
  // No path on opds.
  m_title = VALUE("title");
  m_description = VALUE("summary");
  m_language = VALUE("language");
  m_creator = node.child("author").child("name").child_value();
  m_publisher = node.child("publisher").child("name").child_value();
  m_date = fromOpdsDate(VALUE("updated"));
  m_name = VALUE("name");
  m_flavour = VALUE("flavour");
  m_tags = VALUE("tags");
  const auto catnode = node.child("category");
  m_category = catnode.empty() ? getCategoryFromTags() : catnode.child_value();
  m_articleCount = strtoull(VALUE("articleCount"), 0, 0);
  m_mediaCount = strtoull(VALUE("mediaCount"), 0, 0);
  for(auto linkNode = node.child("link"); linkNode;
           linkNode = linkNode.next_sibling("link")) {
    std::string rel = linkNode.attribute("rel").value();

    if (rel == "http://opds-spec.org/acquisition/open-access") {
      m_url = linkNode.attribute("href").value();
      m_size = strtoull(linkNode.attribute("length").value(), 0, 0);
    }
    if (rel == "http://opds-spec.org/image/thumbnail") {
      m_faviconUrl = urlHost + linkNode.attribute("href").value();
      m_faviconMimeType = linkNode.attribute("type").value();
    }
 }

}
#undef VALUE

std::string Book::getHumanReadableIdFromPath() const
{
  std::string id = m_path;
  if (!id.empty()) {
    kiwix::removeAccents(id);

#ifdef _WIN32
    id = replaceRegex(id, "", "^.*\\\\");
#else
    id = replaceRegex(id, "", "^.*/");
#endif

    id = replaceRegex(id, "", "\\.zim[a-z]*$");
    id = replaceRegex(id, "_", " ");
    id = replaceRegex(id, "plus", "\\+");
  }
  return id;
}

void Book::setPath(const std::string& path)
{
 m_path = isRelativePath(path)
   ? computeAbsolutePath(getCurrentDirectory(), path)
   : path;
}

const std::string& Book::getFavicon() const {
  if (m_favicon.empty() && !m_faviconUrl.empty()) {
    try {
      m_favicon = download(m_faviconUrl);
    } catch(...) {
      std::cerr << "Cannot download favicon from " << m_faviconUrl;
    }
  }
  return m_favicon;
}

std::string Book::getTagStr(const std::string& tagName) const {
  return getTagValueFromTagList(convertTags(m_tags), tagName);
}

bool Book::getTagBool(const std::string& tagName) const {
  return convertStrToBool(getTagStr(tagName));
}

std::string Book::getCategory() const
{
  return m_category;
}

std::string Book::getCategoryFromTags() const
{
  try
  {
    return getTagStr("category");
  }
  catch ( const std::out_of_range& )
  {
    return "";
  }
}

}
