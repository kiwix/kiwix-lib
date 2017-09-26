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

inline char hi(char v)
{
  char hex[] = "0123456789abcdef";
  return hex[(v >> 4) & 0xf];
}

inline char lo(char v)
{
  char hex[] = "0123456789abcdef";
  return hex[v & 0xf];
}

std::string hexUUID(std::string in)
{
  std::ostringstream out;
  for (unsigned n = 0; n < 4; ++n) {
    out << hi(in[n]) << lo(in[n]);
  }
  out << '-';
  for (unsigned n = 4; n < 6; ++n) {
    out << hi(in[n]) << lo(in[n]);
  }
  out << '-';
  for (unsigned n = 6; n < 8; ++n) {
    out << hi(in[n]) << lo(in[n]);
  }
  out << '-';
  for (unsigned n = 8; n < 10; ++n) {
    out << hi(in[n]) << lo(in[n]);
  }
  out << '-';
  for (unsigned n = 10; n < 16; ++n) {
    out << hi(in[n]) << lo(in[n]);
  }
  std::string op = out.str();
  return op;
}

namespace kiwix
{
/* Constructor */
Reader::Reader(const string zimFilePath) : zimFileHandler(NULL)
{
  string tmpZimFilePath = zimFilePath;

  /* Remove potential trailing zimaa */
  size_t found = tmpZimFilePath.rfind("zimaa");
  if (found != string::npos && tmpZimFilePath.size() > 5
      && found == tmpZimFilePath.size() - 5) {
    tmpZimFilePath.resize(tmpZimFilePath.size() - 2);
  }

  this->zimFileHandler = new zim::File(tmpZimFilePath);

  if (this->zimFileHandler != NULL) {
    this->firstArticleOffset
        = this->zimFileHandler->getNamespaceBeginOffset('A');
    this->lastArticleOffset = this->zimFileHandler->getNamespaceEndOffset('A');
    this->currentArticleOffset = this->firstArticleOffset;
    this->nsACount = this->zimFileHandler->getNamespaceCount('A');
    this->nsICount = this->zimFileHandler->getNamespaceCount('I');
    this->zimFilePath = zimFilePath;
  }

  /* initialize random seed: */
  srand(time(NULL));
}

/* Destructor */
Reader::~Reader()
{
  if (this->zimFileHandler != NULL) {
    delete this->zimFileHandler;
  }
}

zim::File* Reader::getZimFileHandler() const
{
  return this->zimFileHandler;
}
/* Reset the cursor for GetNextArticle() */
void Reader::reset()
{
  this->currentArticleOffset = this->firstArticleOffset;
}
std::map<const std::string, unsigned int> Reader::parseCounterMetadata() const
{
  std::map<const std::string, unsigned int> counters;
  string mimeType, item, counterString;
  unsigned int counter;

  zim::Article article = this->zimFileHandler->getArticle('M', "Counter");

  if (article.good()) {
    stringstream ssContent(article.getData());

    while (getline(ssContent, item, ';')) {
      stringstream ssItem(item);
      getline(ssItem, mimeType, '=');
      getline(ssItem, counterString, '=');
      if (!counterString.empty() && !mimeType.empty()) {
        sscanf(counterString.c_str(), "%u", &counter);
        counters.insert(pair<string, int>(mimeType, counter));
      }
    }
  }

  return counters;
}

/* Get the count of articles which can be indexed/displayed */
unsigned int Reader::getArticleCount() const
{
  std::map<const std::string, unsigned int> counterMap
      = this->parseCounterMetadata();
  unsigned int counter = 0;

  if (counterMap.empty()) {
    counter = this->nsACount;
  } else {
    auto it = counterMap.find("text/html");
    if (it != counterMap.end()) {
      counter = it->second;
    }
  }

  return counter;
}

/* Get the count of medias content in the ZIM file */
unsigned int Reader::getMediaCount() const
{
  std::map<const std::string, unsigned int> counterMap
      = this->parseCounterMetadata();
  unsigned int counter = 0;

  if (counterMap.empty()) {
    counter = this->nsICount;
  } else {
    auto it = counterMap.find("image/jpeg");
    if (it != counterMap.end()) {
      counter += it->second;
    }

    it = counterMap.find("image/gif");
    if (it != counterMap.end()) {
      counter += it->second;
    }

    it = counterMap.find("image/png");
    if (it != counterMap.end()) {
      counter += it->second;
    }
  }
  return counter;
}

/* Get the total of all items of a ZIM file, redirects included */
unsigned int Reader::getGlobalCount() const
{
  return this->zimFileHandler->getCountArticles();
}

/* Return the UID of the ZIM file */
string Reader::getId() const
{
  std::ostringstream s;
  s << this->zimFileHandler->getFileheader().getUuid();
  return s.str();
}

/* Return a page url from a title */
bool Reader::getPageUrlFromTitle(const string& title, string& url) const
{
  /* Extract the content from the zim file */
  zim::Article article = this->zimFileHandler->getArticleByTitle('A', title);

  if (!article.good()) {
    return false;
  }

  unsigned int loopCounter = 0;
  while (article.isRedirect() && loopCounter++ < 42) {
    article = article.getRedirectArticle();
  }

  url = article.getLongUrl();
  return true;
}

/* Return an URL from a title */
string Reader::getRandomPageUrl() const
{
  zim::Article article;
  zim::size_type idx;
  std::string mainPageUrl = this->getMainPageUrl();

  do {
    idx = this->firstArticleOffset
          + (zim::size_type)((double)rand() / ((double)RAND_MAX + 1)
                             * this->nsACount);
    article = zimFileHandler->getArticle(idx);
  } while (article.getLongUrl() == mainPageUrl);

  return article.getLongUrl();
}

/* Return the welcome page URL */
string Reader::getMainPageUrl() const
{
  string url = "";

  if (this->zimFileHandler->getFileheader().hasMainPage()) {
    zim::Article article = zimFileHandler->getArticle(
        this->zimFileHandler->getFileheader().getMainPage());
    url = article.getLongUrl();

    if (url.empty()) {
      url = getFirstPageUrl();
    }
  } else {
    url = getFirstPageUrl();
  }

  return url;
}

bool Reader::getFavicon(string& content, string& mimeType) const
{
  unsigned int contentLength = 0;
  string title;

  this->getContentByUrl("/-/favicon.png", content, title, contentLength, mimeType);

  if (content.empty()) {
    this->getContentByUrl("/I/favicon.png", content, title, contentLength, mimeType);

    if (content.empty()) {
      this->getContentByUrl("/I/favicon", content, title, contentLength, mimeType);

      if (content.empty()) {
        this->getContentByUrl("/-/favicon", content, title, contentLength, mimeType);
      }
    }
  }

  return content.empty() ? false : true;
}

string Reader::getZimFilePath() const
{
  return this->zimFilePath;
}
/* Return a metatag value */
bool Reader::getMetatag(const string& name, string& value) const
{
  unsigned int contentLength = 0;
  string contentType = "";
  string title;

  return this->getContentByUrl("/M/" + name, value, title, contentLength, contentType);
}

string Reader::getTitle() const
{
  string value;
  this->getMetatag("Title", value);
  if (value.empty()) {
    value = getLastPathElement(zimFileHandler->getFilename());
    std::replace(value.begin(), value.end(), '_', ' ');
    size_t pos = value.find(".zim");
    value = value.substr(0, pos);
  }
  return value;
}

string Reader::getName() const
{
  string value;
  this->getMetatag("Name", value);
  return value;
}

string Reader::getTags() const
{
  string value;
  this->getMetatag("Tags", value);
  return value;
}

string Reader::getDescription() const
{
  string value;
  this->getMetatag("Description", value);

  /* Mediawiki Collection tends to use the "Subtitle" name */
  if (value.empty()) {
    this->getMetatag("Subtitle", value);
  }

  return value;
}

string Reader::getLanguage() const
{
  string value;
  this->getMetatag("Language", value);
  return value;
}

string Reader::getDate() const
{
  string value;
  this->getMetatag("Date", value);
  return value;
}

string Reader::getCreator() const
{
  string value;
  this->getMetatag("Creator", value);
  return value;
}

string Reader::getPublisher() const
{
  string value;
  this->getMetatag("Publisher", value);
  return value;
}

string Reader::getOrigId() const
{
  string value;
  this->getMetatag("startfileuid", value);
  if (value.empty()) {
    return "";
  }
  std::string id = value;
  std::string origID;
  std::string temp = "";
  unsigned int k = 0;
  char tempArray[16] = "";
  for (unsigned int i = 0; i < id.size(); i++) {
    if (id[i] == '\n') {
      tempArray[k] = atoi(temp.c_str());
      temp = "";
      k++;
    } else {
      temp += id[i];
    }
  }
  origID = hexUUID(tempArray);
  return origID;
}

/* Return the first page URL */
string Reader::getFirstPageUrl() const
{
  zim::size_type firstPageOffset = zimFileHandler->getNamespaceBeginOffset('A');
  zim::Article article = zimFileHandler->getArticle(firstPageOffset);
  return article.getLongUrl();
}

bool Reader::parseUrl(const string& url, char* ns, string& title) const
{
  /* Offset to visit the url */
  unsigned int urlLength = url.size();
  unsigned int offset = 0;

  /* Ignore the '/' */
  while ((offset < urlLength) && (url[offset] == '/')) {
    offset++;
  }

  /* Get namespace */
  while ((offset < urlLength) && (url[offset] != '/')) {
    *ns = url[offset];
    offset++;
  }

  /* Ignore the '/' */
  while ((offset < urlLength) && (url[offset] == '/')) {
    offset++;
  }

  /* Get content title */
  unsigned int titleOffset = offset;
  while (offset < urlLength) {
    offset++;
  }

  /* unescape title */
  title = url.substr(titleOffset, offset - titleOffset);

  return true;
}

/* Return article by url */
bool Reader::getArticleObjectByDecodedUrl(const string& url,
                                          zim::Article& article) const
{
  if (this->zimFileHandler == NULL) {
    return false;
  }

  /* Parse the url */
  char ns = 0;
  string urlStr;
  this->parseUrl(url, &ns, urlStr);

  /* Main page */
  if (urlStr.empty() && ns == 0) {
    this->parseUrl(this->getMainPageUrl(), &ns, urlStr);
  }

  /* Extract the content from the zim file */
  article = zimFileHandler->getArticle(ns, urlStr);
  return article.good();
}

/* Return the mimeType without the content */
bool Reader::getMimeTypeByUrl(const string& url, string& mimeType) const
{
  if (this->zimFileHandler == NULL) {
    return false;
  }

  zim::Article article;
  if (this->getArticleObjectByDecodedUrl(url, article)) {
    try {
      mimeType = article.getMimeType();
    } catch (exception& e) {
      cerr << "Unable to get the mimetype for " << url << ":" << e.what()
           << endl;
      mimeType = "application/octet-stream";
    }
    return true;
  } else {
    mimeType = "";
    return false;
  }
}

/* Get a content from a zim file */
bool Reader::getContentByUrl(const string& url,
                             string& content,
                             string& title,
                             unsigned int& contentLength,
                             string& contentType) const
{
  return this->getContentByEncodedUrl(url, content, title, contentLength, contentType);
}

bool Reader::getContentByEncodedUrl(const string& url,
                                    string& content,
                                    string& title,
                                    unsigned int& contentLength,
                                    string& contentType,
                                    string& baseUrl) const
{
  return this->getContentByDecodedUrl(
      kiwix::urlDecode(url), content, title, contentLength, contentType, baseUrl);
}

bool Reader::getContentByEncodedUrl(const string& url,
                                    string& content,
                                    string& title,
                                    unsigned int& contentLength,
                                    string& contentType) const
{
  std::string stubRedirectUrl;
  return this->getContentByEncodedUrl(kiwix::urlDecode(url),
                                      content,
                                      title,
                                      contentLength,
                                      contentType,
                                      stubRedirectUrl);
}

bool Reader::getContentByDecodedUrl(const string& url,
                                    string& content,
                                    string& title,
                                    unsigned int& contentLength,
                                    string& contentType) const
{
  std::string stubRedirectUrl;
  return this->getContentByDecodedUrl(kiwix::urlDecode(url),
                                      content,
                                      title,
                                      contentLength,
                                      contentType,
                                      stubRedirectUrl);
}

bool Reader::getContentByDecodedUrl(const string& url,
                                    string& content,
                                    string& title,
                                    unsigned int& contentLength,
                                    string& contentType,
                                    string& baseUrl) const
{
  content = "";
  contentType = "";
  contentLength = 0;

  zim::Article article;
  if (!this->getArticleObjectByDecodedUrl(url, article)) {
    return false;
  }

  /* If redirect */
  unsigned int loopCounter = 0;
  while (article.isRedirect() && loopCounter++ < 42) {
    article = article.getRedirectArticle();
  }

  if (loopCounter < 42) {
    /* Compute base url (might be different from the url if redirects */
    baseUrl
        = "/" + std::string(1, article.getNamespace()) + "/" + article.getUrl();

    /* Get the content mime-type */
    try {
      contentType
          = string(article.getMimeType().data(), article.getMimeType().size());
    } catch (exception& e) {
      cerr << "Unable to get the mimetype for " << baseUrl << ":" << e.what()
           << endl;
      contentType = "application/octet-stream";
    }

    /* Get the data */
    content = string(article.getData().data(), article.getArticleSize());
    title = article.getTitle();
  }

  /* Try to set a stub HTML header/footer if necesssary */
  if (contentType.find("text/html") != string::npos
      && content.find("<body") == std::string::npos
      && content.find("<BODY") == std::string::npos) {
    content = "<html><head><title>" + article.getTitle() +
              "</title><meta http-equiv=\"Content-Type\" content=\"text/html; "
              "charset=utf-8\" /></head><body>" +
              content + "</body></html>";
  }

  /* Get the data length */
  contentLength = article.getArticleSize();

  return true;
}

/* Check if an article exists */
bool Reader::urlExists(const string& url) const
{
  char ns = 0;
  string titleStr;
  this->parseUrl(url, &ns, titleStr);
  titleStr = "/" + titleStr;
  zim::File::const_iterator findItr = zimFileHandler->find(ns, titleStr);
  return findItr != zimFileHandler->end() && findItr->getUrl() == titleStr;
}

/* Does the ZIM file has a fulltext index */
bool Reader::hasFulltextIndex() const
{
  return ( this->urlExists("/Z/fulltextIndex/xapian")
        && !zimFileHandler->is_multiPart() );
}

/* Search titles by prefix */
bool Reader::searchSuggestions(const string& prefix,
                               unsigned int suggestionsCount,
                               const bool reset)
{
  bool retVal = false;
  zim::File::const_iterator articleItr;

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

  /* Return if no prefix */
  if (prefix.size() == 0) {
    return false;
  }

  for (articleItr = zimFileHandler->findByTitle('A', prefix);
       articleItr != zimFileHandler->end()
       && articleItr->getTitle().compare(0, prefix.size(), prefix) == 0
       && this->suggestions.size() < suggestionsCount;
       ++articleItr) {
    /* Extract the interesting part of article title & url */
    std::string normalizedArticleTitle
        = kiwix::normalize(articleItr->getTitle());
    std::string articleFinalUrl = "/A/" + articleItr->getUrl();
    if (articleItr->isRedirect()) {
      zim::Article article = *articleItr;
      unsigned int loopCounter = 0;
      while (article.isRedirect() && loopCounter++ < 42) {
        article = article.getRedirectArticle();
      }
      articleFinalUrl = "/A/" + article.getUrl();
    }

    /* Go through all already found suggestions and skip if this
       article is already in the suggestions list (with an other
       title) */
    bool insert = true;
    std::vector<std::vector<std::string>>::iterator suggestionItr;
    for (suggestionItr = this->suggestions.begin();
         suggestionItr != this->suggestions.end();
         suggestionItr++) {
      int result = normalizedArticleTitle.compare((*suggestionItr)[2]);
      if (result == 0 && articleFinalUrl.compare((*suggestionItr)[1]) == 0) {
        insert = false;
        break;
      } else if (result < 0) {
        break;
      }
    }

    /* Insert if possible */
    if (insert) {
      std::vector<std::string> suggestion;
      suggestion.push_back(articleItr->getTitle());
      suggestion.push_back(articleFinalUrl);
      suggestion.push_back(normalizedArticleTitle);
      this->suggestions.insert(suggestionItr, suggestion);
    }

    /* Suggestions where found */
    retVal = true;
  }

  /* Set the cursor to the begining */
  this->suggestionsOffset = this->suggestions.begin();

  return retVal;
}

std::vector<std::string> Reader::getTitleVariants(
    const std::string& title) const
{
  std::vector<std::string> variants;
  variants.push_back(title);
  variants.push_back(kiwix::ucFirst(title));
  variants.push_back(kiwix::lcFirst(title));
  variants.push_back(kiwix::toTitle(title));
  return variants;
}

/* Try also a few variations of the prefix to have better results */
bool Reader::searchSuggestionsSmart(const string& prefix,
                                    unsigned int suggestionsCount)
{
  std::vector<std::string> variants = this->getTitleVariants(prefix);
  bool retVal;

  this->suggestions.clear();
  this->suggestionsOffset = this->suggestions.begin();
  /* Try to search in the title using fulltext search database */
  const zim::Search* suggestionSearch
      = this->getZimFileHandler()->suggestions(prefix, 0, suggestionsCount);

  if (suggestionSearch->get_matches_estimated()) {
    for (auto current = suggestionSearch->begin();
         current != suggestionSearch->end();
         current++) {
      std::vector<std::string> suggestion;
      suggestion.push_back(current->getTitle());
      suggestion.push_back("/A/" + current->getUrl());
      suggestion.push_back(kiwix::normalize(current->getTitle()));
      this->suggestions.push_back(suggestion);
    }
    this->suggestionsOffset = this->suggestions.begin();
    retVal = true;
  } else {
    for (std::vector<std::string>::iterator variantsItr = variants.begin();
         variantsItr != variants.end();
         variantsItr++) {
      retVal = this->searchSuggestions(*variantsItr, suggestionsCount, false)
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
    title = (*(this->suggestionsOffset))[0];

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
    title = (*(this->suggestionsOffset))[0];
    url = (*(this->suggestionsOffset))[1];

    /* increment the cursor for the next call */
    this->suggestionsOffset++;

    return true;
  }

  return false;
}

/* Check if the file has as checksum */
bool Reader::canCheckIntegrity() const
{
  return this->zimFileHandler->getChecksum() != "";
}

/* Return true if corrupted, false otherwise */
bool Reader::isCorrupted() const
{
  try {
    if (this->zimFileHandler->verify() == true) {
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
  zim::File* file = this->getZimFileHandler();
  zim::offset_type size = 0;

  if (file != NULL) {
    size = file->getFilesize();
  }

  return (size / 1024);
}
}
