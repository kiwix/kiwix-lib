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

#include "library.h"
#include "book.h"
#include "reader.h"
#include "libxml_dumper.h"

#include "tools.h"
#include "tools/base64.h"
#include "tools/regexTools.h"
#include "tools/pathTools.h"
#include "tools/stringTools.h"

#include <pugixml.hpp>
#include <algorithm>
#include <set>
#include <unicode/locid.h>
#include <xapian.h>

namespace kiwix
{

namespace
{

std::string iso639_3ToXapian(const std::string& lang) {
  return icu::Locale(lang.c_str()).getLanguage();
};

std::string normalizeText(const std::string& text)
{
  return removeAccents(text);
}

} // unnamed namespace

class Library::BookDB : public Xapian::WritableDatabase
{
public:
  BookDB() : Xapian::WritableDatabase("", Xapian::DB_BACKEND_INMEMORY) {}
};

/* Constructor */
Library::Library()
  : m_bookDB(new BookDB)
{
}

Library::Library(Library&& ) = default;

Library& Library::operator=(Library&& ) = default;

/* Destructor */
Library::~Library()
{
}


bool Library::addBook(const Book& book)
{
  /* Try to find it */
  updateBookDB(book);
  try {
    auto& oldbook = m_books.at(book.getId());
    oldbook.update(book);
    return false;
  } catch (std::out_of_range&) {
    m_books[book.getId()] = book;
    return true;
  }
}

void Library::addBookmark(const Bookmark& bookmark)
{
  m_bookmarks.push_back(bookmark);
}

bool Library::removeBookmark(const std::string& zimId, const std::string& url)
{
  for(auto it=m_bookmarks.begin(); it!=m_bookmarks.end(); it++) {
    if (it->getBookId() == zimId && it->getUrl() == url) {
      m_bookmarks.erase(it);
      return true;
    }
  }
  return false;
}



bool Library::removeBookById(const std::string& id)
{
  m_bookDB->delete_document("Q" + id);
  m_readers.erase(id);
  m_archives.erase(id);
  return m_books.erase(id) == 1;
}

const Book& Library::getBookById(const std::string& id) const
{
  return m_books.at(id);
}

Book& Library::getBookById(const std::string& id)
{
  const Library& const_self = *this;
  return const_cast<Book&>(const_self.getBookById(id));
}

const Book& Library::getBookByPath(const std::string& path) const
{
  for(auto& it: m_books) {
    auto& book = it.second;
    if (book.getPath() == path)
      return book;
  }
  std::ostringstream ss;
  ss << "No book with path " << path << " in the library." << std::endl;
  throw std::out_of_range(ss.str());
}

Book& Library::getBookByPath(const std::string& path)
{
  const Library& const_self = *this;
  return const_cast<Book&>(const_self.getBookByPath(path));
}

std::shared_ptr<Reader> Library::getReaderById(const std::string& id)
{
  try {
    return m_readers.at(id);
  } catch (std::out_of_range& e) {}

  try {
    auto reader = make_shared<Reader>(m_archives.at(id));
    m_readers[id] = reader;
    return reader;
  } catch (std::out_of_range& e) {}

  auto book = getBookById(id);
  if (!book.isPathValid())
    return nullptr;

  auto archive = make_shared<zim::Archive>(book.getPath());
  m_archives[id] = archive;
  auto reader = make_shared<Reader>(archive);
  m_readers[id] = reader;
  return reader;
}

std::shared_ptr<zim::Archive> Library::getArchiveById(const std::string& id)
{
  try {
    return m_archives.at(id);
  } catch (std::out_of_range& e) {}

  auto book = getBookById(id);
  if (!book.isPathValid())
    return nullptr;

  auto sptr = make_shared<zim::Archive>(book.getPath());
  m_archives[id] = sptr;
  return sptr;
}

unsigned int Library::getBookCount(const bool localBooks,
                                   const bool remoteBooks) const
{
  unsigned int result = 0;
  for (auto& pair: m_books) {
    auto& book = pair.second;
    if ((!book.getPath().empty() && localBooks)
        || (book.getPath().empty() && remoteBooks)) {
      result++;
    }
  }
  return result;
}

bool Library::writeToFile(const std::string& path) const
{
  auto baseDir = removeLastPathElement(path);
  LibXMLDumper dumper(this);
  dumper.setBaseDir(baseDir);
  return writeTextFile(path, dumper.dumpLibXMLContent(getBooksIds()));
}

bool Library::writeBookmarksToFile(const std::string& path) const
{
  LibXMLDumper dumper(this);
  return writeTextFile(path, dumper.dumpLibXMLBookmark());
}

Library::AttributeCounts Library::getBookAttributeCounts(BookStrPropMemFn p) const
{
  AttributeCounts propValueCounts;

  for (const auto& pair: m_books) {
    const auto& book = pair.second;
    if (book.getOrigId().empty()) {
      propValueCounts[(book.*p)()] += 1;
    }
  }
  return propValueCounts;
}

std::vector<std::string> Library::getBookPropValueSet(BookStrPropMemFn p) const
{
  std::vector<std::string> result;
  for ( const auto& kv : getBookAttributeCounts(p) ) {
    result.push_back(kv.first);
  }
  return result;
}

std::vector<std::string> Library::getBooksLanguages() const
{
  return getBookPropValueSet(&Book::getLanguage);
}

Library::AttributeCounts Library::getBooksLanguagesWithCounts() const
{
  return getBookAttributeCounts(&Book::getLanguage);
}

std::vector<std::string> Library::getBooksCategories() const
{
  std::set<std::string> categories;

  for (const auto& pair: m_books) {
    const auto& book = pair.second;
    const auto& c = book.getCategory();
    if ( !c.empty() ) {
      categories.insert(c);
    }
  }

  return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> Library::getBooksCreators() const
{
  return getBookPropValueSet(&Book::getCreator);
}

std::vector<std::string> Library::getBooksPublishers() const
{
  return getBookPropValueSet(&Book::getPublisher);
}

const std::vector<kiwix::Bookmark> Library::getBookmarks(bool onlyValidBookmarks) const
{
  if (!onlyValidBookmarks) {
    return m_bookmarks;
  }
  std::vector<kiwix::Bookmark> validBookmarks;
  auto booksId = getBooksIds();
  for(auto& bookmark:m_bookmarks) {
    if (std::find(booksId.begin(), booksId.end(), bookmark.getBookId()) != booksId.end()) {
      validBookmarks.push_back(bookmark);
    }
  }
  return validBookmarks;
}

Library::BookIdCollection Library::getBooksIds() const
{
  BookIdCollection bookIds;

  for (auto& pair: m_books) {
    bookIds.push_back(pair.first);
  }

  return bookIds;
}

Library::BookIdCollection Library::filter(const std::string& search) const
{
  if (search.empty()) {
    return getBooksIds();
  }

  return filter(Filter().query(search));
}


void Library::updateBookDB(const Book& book)
{
  Xapian::Stem stemmer;
  Xapian::TermGenerator indexer;
  const std::string lang = book.getLanguage();
  try {
    stemmer = Xapian::Stem(iso639_3ToXapian(lang));
    indexer.set_stemmer(stemmer);
    indexer.set_stemming_strategy(Xapian::TermGenerator::STEM_SOME);
  } catch (...) {}
  Xapian::Document doc;
  indexer.set_document(doc);

  const std::string title = normalizeText(book.getTitle());
  const std::string desc = normalizeText(book.getDescription());

  // Index title and description without prefixes for general search
  indexer.index_text(title);
  indexer.increase_termpos();
  indexer.index_text(desc);

  // Index all fields for field-based search
  indexer.index_text(title, 1, "S");
  indexer.index_text(desc,  1, "XD");
  indexer.index_text(lang,  1, "L");
  indexer.index_text(normalizeText(book.getCreator()),   1, "A");
  indexer.index_text(normalizeText(book.getPublisher()), 1, "XP");
  indexer.index_text(normalizeText(book.getName()),      1, "XN");
  indexer.index_text(normalizeText(book.getCategory()),  1, "XC");

  for ( const auto& tag : split(normalizeText(book.getTags()), ";") )
    doc.add_boolean_term("XT" + tag);

  const std::string idterm = "Q" + book.getId();
  doc.add_boolean_term(idterm);

  doc.set_data(book.getId());

  m_bookDB->replace_document(idterm, doc);
}

namespace
{

bool willSelectEverything(const Xapian::Query& query)
{
  return query.get_type() == Xapian::Query::LEAF_MATCH_ALL;
}


Xapian::Query buildXapianQueryFromFilterQuery(const Filter& filter)
{
  if ( !filter.hasQuery() || filter.getQuery().empty() ) {
    // This is a thread-safe way to construct an equivalent of
    // a Xapian::Query::MatchAll query
    return Xapian::Query(std::string());
  }

  Xapian::QueryParser queryParser;
  queryParser.set_default_op(Xapian::Query::OP_AND);
  queryParser.add_prefix("title", "S");
  queryParser.add_prefix("description", "XD");
  queryParser.add_prefix("name", "XN");
  queryParser.add_prefix("category", "XC");
  queryParser.add_prefix("lang", "L");
  queryParser.add_prefix("publisher", "XP");
  queryParser.add_prefix("creator", "A");
  queryParser.add_prefix("tag", "XT");
  const auto partialQueryFlag = filter.queryIsPartial()
                              ? Xapian::QueryParser::FLAG_PARTIAL
                              : 0;
  // Language assumed for the query is not known for sure so stemming
  // is not applied
  //queryParser.set_stemmer(Xapian::Stem(iso639_3ToXapian(???)));
  //queryParser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
  const auto flags = Xapian::QueryParser::FLAG_PHRASE
                   | Xapian::QueryParser::FLAG_BOOLEAN
                   | Xapian::QueryParser::FLAG_BOOLEAN_ANY_CASE
                   | Xapian::QueryParser::FLAG_LOVEHATE
                   | Xapian::QueryParser::FLAG_WILDCARD
                   | partialQueryFlag;
  return queryParser.parse_query(normalizeText(filter.getQuery()), flags);
}

Xapian::Query nameQuery(const std::string& name)
{
  return Xapian::Query("XN" + normalizeText(name));
}

Xapian::Query categoryQuery(const std::string& category)
{
  return Xapian::Query("XC" + normalizeText(category));
}

Xapian::Query langQuery(const std::string& lang)
{
  return Xapian::Query("L" + normalizeText(lang));
}

Xapian::Query publisherQuery(const std::string& publisher)
{
  Xapian::QueryParser queryParser;
  queryParser.set_default_op(Xapian::Query::OP_OR);
  queryParser.set_stemming_strategy(Xapian::QueryParser::STEM_NONE);
  const auto flags = 0;
  const auto q = queryParser.parse_query(normalizeText(publisher), flags, "XP");
  return Xapian::Query(Xapian::Query::OP_PHRASE, q.get_terms_begin(), q.get_terms_end(), q.get_length());
}

Xapian::Query creatorQuery(const std::string& creator)
{
  Xapian::QueryParser queryParser;
  queryParser.set_default_op(Xapian::Query::OP_OR);
  queryParser.set_stemming_strategy(Xapian::QueryParser::STEM_NONE);
  const auto flags = 0;
  const auto q = queryParser.parse_query(normalizeText(creator), flags, "A");
  return Xapian::Query(Xapian::Query::OP_PHRASE, q.get_terms_begin(), q.get_terms_end(), q.get_length());
}

Xapian::Query tagsQuery(const Filter::Tags& acceptTags, const Filter::Tags& rejectTags)
{
  Xapian::Query q = Xapian::Query(std::string());
  if (!acceptTags.empty()) {
    for ( const auto& tag : acceptTags )
      q &= Xapian::Query("XT" + normalizeText(tag));
  }

  if (!rejectTags.empty()) {
    for ( const auto& tag : rejectTags )
      q = Xapian::Query(Xapian::Query::OP_AND_NOT, q, "XT" + normalizeText(tag));
  }
  return q;
}

Xapian::Query buildXapianQuery(const Filter& filter)
{
  auto q = buildXapianQueryFromFilterQuery(filter);
  if ( filter.hasName() ) {
    q = Xapian::Query(Xapian::Query::OP_AND, q, nameQuery(filter.getName()));
  }
  if ( filter.hasCategory() ) {
    q = Xapian::Query(Xapian::Query::OP_AND, q, categoryQuery(filter.getCategory()));
  }
  if ( filter.hasLang() ) {
    q = Xapian::Query(Xapian::Query::OP_AND, q, langQuery(filter.getLang()));
  }
  if ( filter.hasPublisher() ) {
    q = Xapian::Query(Xapian::Query::OP_AND, q, publisherQuery(filter.getPublisher()));
  }
  if ( filter.hasCreator() ) {
    q = Xapian::Query(Xapian::Query::OP_AND, q, creatorQuery(filter.getCreator()));
  }
  if ( !filter.getAcceptTags().empty() || !filter.getRejectTags().empty() ) {
    const auto tq = tagsQuery(filter.getAcceptTags(), filter.getRejectTags());
    q = Xapian::Query(Xapian::Query::OP_AND, q, tq);;
  }
  return q;
}

} // unnamed namespace

Library::BookIdCollection Library::filterViaBookDB(const Filter& filter) const
{
  const auto query = buildXapianQuery(filter);

  if ( willSelectEverything(query) )
    return getBooksIds();

  BookIdCollection bookIds;

  Xapian::Enquire enquire(*m_bookDB);
  enquire.set_query(query);
  const auto results = enquire.get_mset(0, m_books.size());
  for ( auto it = results.begin(); it != results.end(); ++it  ) {
    bookIds.push_back(it.get_document().get_data());
  }

  return bookIds;
}

Library::BookIdCollection Library::filter(const Filter& filter) const
{
  BookIdCollection result;
  for(auto id : filterViaBookDB(filter)) {
    if(filter.accept(m_books.at(id))) {
      result.push_back(id);
    }
  }
  return result;
}

template<supportedListSortBy SORT>
struct KEY_TYPE {
  typedef std::string TYPE;
};

template<>
struct KEY_TYPE<SIZE> {
  typedef size_t TYPE;
};

template<supportedListSortBy sort>
class Comparator {
  private:
    const Library* const lib;
    const bool     ascending;

    inline typename KEY_TYPE<sort>::TYPE get_key(const std::string& id);

  public:
    Comparator(const Library* lib, bool ascending) : lib(lib), ascending(ascending) {}
    inline bool operator() (const std::string& id1, const std::string& id2) {
      if (ascending) {
        return get_key(id1) < get_key(id2);
      } else {
        return get_key(id2) < get_key(id1);
      }
    }
};

template<>
std::string Comparator<TITLE>::get_key(const std::string& id)
{
  return lib->getBookById(id).getTitle();
}

template<>
size_t Comparator<SIZE>::get_key(const std::string& id)
{
  return lib->getBookById(id).getSize();
}

template<>
std::string Comparator<DATE>::get_key(const std::string& id)
{
  return lib->getBookById(id).getDate();
}

template<>
std::string Comparator<CREATOR>::get_key(const std::string& id)
{
  return lib->getBookById(id).getCreator();
}

template<>
std::string Comparator<PUBLISHER>::get_key(const std::string& id)
{
  return lib->getBookById(id).getPublisher();
}

void Library::sort(BookIdCollection& bookIds, supportedListSortBy sort, bool ascending) const
{
  switch(sort) {
    case TITLE:
      std::sort(bookIds.begin(), bookIds.end(), Comparator<TITLE>(this, ascending));
      break;
    case SIZE:
      std::sort(bookIds.begin(), bookIds.end(), Comparator<SIZE>(this, ascending));
      break;
    case DATE:
      std::sort(bookIds.begin(), bookIds.end(), Comparator<DATE>(this, ascending));
      break;
    case CREATOR:
      std::sort(bookIds.begin(), bookIds.end(), Comparator<CREATOR>(this, ascending));
      break;
    case PUBLISHER:
      std::sort(bookIds.begin(), bookIds.end(), Comparator<PUBLISHER>(this, ascending));
      break;
    default:
      break;
  }
}


Library::BookIdCollection Library::listBooksIds(
    int mode,
    supportedListSortBy sortBy,
    const std::string& search,
    const std::string& language,
    const std::string& creator,
    const std::string& publisher,
    const std::vector<std::string>& tags,
    size_t maxSize) const {

  Filter _filter;
  if (mode & LOCAL)
    _filter.local(true);
  if (mode & NOLOCAL)
    _filter.local(false);
  if (mode & VALID)
    _filter.valid(true);
  if (mode & NOVALID)
    _filter.valid(false);
  if (mode & REMOTE)
    _filter.remote(true);
  if (mode & NOREMOTE)
    _filter.remote(false);
  if (!tags.empty())
    _filter.acceptTags(tags);
  if (maxSize != 0)
    _filter.maxSize(maxSize);
  if (!language.empty())
    _filter.lang(language);
  if (!publisher.empty())
    _filter.publisher(publisher);
  if (!creator.empty())
    _filter.creator(creator);
  if (!search.empty())
    _filter.query(search);

  auto bookIds = filter(_filter);

  sort(bookIds, sortBy, true);
  return bookIds;
}

Filter::Filter()
  : activeFilters(0),
    _maxSize(0)
{};

#define FLAG(x) (1 << x)
enum filterTypes {
  NONE = 0,
  _LOCAL = FLAG(0),
  _REMOTE = FLAG(1),
  _NOLOCAL = FLAG(2),
  _NOREMOTE = FLAG(3),
  _VALID = FLAG(4),
  _NOVALID = FLAG(5),
  ACCEPTTAGS = FLAG(6),
  REJECTTAGS = FLAG(7),
  LANG = FLAG(8),
  _PUBLISHER = FLAG(9),
  _CREATOR = FLAG(10),
  MAXSIZE = FLAG(11),
  QUERY = FLAG(12),
  NAME = FLAG(13),
  CATEGORY = FLAG(14),
};

Filter& Filter::local(bool accept)
{
  if (accept) {
    activeFilters |= _LOCAL;
    activeFilters &= ~_NOLOCAL;
  } else {
    activeFilters |= _NOLOCAL;
    activeFilters &= ~_LOCAL;
  }
  return *this;
}

Filter& Filter::remote(bool accept)
{
  if (accept) {
    activeFilters |= _REMOTE;
    activeFilters &= ~_NOREMOTE;
  } else {
    activeFilters |= _NOREMOTE;
    activeFilters &= ~_REMOTE;
  }
  return *this;
}

Filter& Filter::valid(bool accept)
{
  if (accept) {
    activeFilters |= _VALID;
    activeFilters &= ~_NOVALID;
  } else {
    activeFilters |= _NOVALID;
    activeFilters &= ~_VALID;
  }
  return *this;
}

Filter& Filter::acceptTags(const Tags& tags)
{
  _acceptTags = tags;
  activeFilters |= ACCEPTTAGS;
  return *this;
}

Filter& Filter::rejectTags(const Tags& tags)
{
  _rejectTags = tags;
  activeFilters |= REJECTTAGS;
  return *this;
}

Filter& Filter::category(std::string category)
{
  _category = category;
  activeFilters |= CATEGORY;
  return *this;
}

Filter& Filter::lang(std::string lang)
{
  _lang = lang;
  activeFilters |= LANG;
  return *this;
}

Filter& Filter::publisher(std::string publisher)
{
  _publisher = publisher;
  activeFilters |= _PUBLISHER;
  return *this;
}

Filter& Filter::creator(std::string creator)
{
  _creator = creator;
  activeFilters |= _CREATOR;
  return *this;
}

Filter& Filter::maxSize(size_t maxSize)
{
  _maxSize = maxSize;
  activeFilters |= MAXSIZE;
  return *this;
}

Filter& Filter::query(std::string query, bool partial)
{
  _query = query;
  _queryIsPartial = partial;
  activeFilters |= QUERY;
  return *this;
}

Filter& Filter::name(std::string name)
{
  _name = name;
  activeFilters |= NAME;
  return *this;
}

#define ACTIVE(X) (activeFilters & (X))
#define FILTER(TAG, TEST) if (ACTIVE(TAG) && !(TEST)) { return false; }
bool Filter::hasQuery() const
{
  return ACTIVE(QUERY);
}

bool Filter::hasName() const
{
  return ACTIVE(NAME);
}

bool Filter::hasCategory() const
{
  return ACTIVE(CATEGORY);
}

bool Filter::hasLang() const
{
  return ACTIVE(LANG);
}

bool Filter::hasPublisher() const
{
  return ACTIVE(_PUBLISHER);
}

bool Filter::hasCreator() const
{
  return ACTIVE(_CREATOR);
}

bool Filter::accept(const Book& book) const
{
  auto local = !book.getPath().empty();
  FILTER(_LOCAL, local)
  FILTER(_NOLOCAL, !local)

  auto valid = book.isPathValid();
  FILTER(_VALID, valid)
  FILTER(_NOVALID, !valid)

  auto remote = !book.getUrl().empty();
  FILTER(_REMOTE, remote)
  FILTER(_NOREMOTE, !remote)

  FILTER(MAXSIZE, book.getSize() <= _maxSize)

  return true;
}

}
