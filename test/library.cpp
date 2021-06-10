/*
 * Copyright (C) 2019 Matthieu Gautier <mgautier@kymeria.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "gtest/gtest.h"
#include <string>


const char * sampleOpdsStream = R"(
<feed xmlns="http://www.w3.org/2005/Atom" xmlns:opds="http://opds-spec.org/2010/catalog">
  <id>00000000-0000-0000-0000-000000000000</id>
  <entry>
    <title>Encyclopédie de la Tunisie</title>
    <id>urn:uuid:0c45160e-f917-760a-9159-dfe3c53cdcdd</id>
    <icon>/meta?name=favicon&amp;content=wikipedia_fr_tunisie_novid_2018-10</icon>
    <updated>2018-10-08T00:00::00:Z</updated>
    <language>fra</language>
    <summary>Le meilleur de Wikipédia sur la Tunisie</summary>
    <tags>wikipedia;novid;_ftindex</tags>
    <link type="text/html" href="/wikipedia_fr_tunisie_novid_2018-10" />
    <author>
      <name>Wikipedia</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/wikipedia/wikipedia_fr_tunisie_novid_2018-10.zim.meta4" length="90030080" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=wikipedia_fr_tunisie_novid_2018-10" />
  </entry>
  <entry>
    <title>Tania Louis</title>
    <id>urn:uuid:0d0bcd57-d3f6-cb22-44cc-a723ccb4e1b2</id>
    <icon>/meta?name=favicon&amp;content=biologie-tout-compris_fr_all_2018-06</icon>
    <updated>2018-06-23T00:00::00:Z</updated>
    <language>fra</language>
    <summary>Tania Louis videos</summary>
    <tags>youtube;_category:category_defined_via_tags_only</tags>
    <link type="text/html" href="/biologie-tout-compris_fr_all_2018-06" />
    <author>
      <name>Tania Louis</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/other/biologie-tout-compris_fr_all_2018-06.zim.meta4" length="2172639232" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=biologie-tout-compris_fr_all_2018-06" />
  </entry>
  <entry>
    <title>Wikiquote</title>
    <id>urn:uuid:0ea1cde6-441d-6c58-f2c7-21c2838e659f</id>
    <icon>/meta?name=favicon&amp;content=wikiquote_fr_all_nopic_2019-06</icon>
    <updated>2019-06-05T00:00::00:Z</updated>
    <language>fra</language>
    <summary>Une page de Wikiquote, le recueil des citations libres.</summary>
    <category>category_defined_via_category_element_only</category>
    <tags>wikiquote;nopic</tags>
    <link type="text/html" href="/wikiquote_fr_all_nopic_2019-06" />
    <author>
      <name>Wikiquote</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/wikiquote/wikiquote_fr_all_nopic_2019-06.zim.meta4" length="21368832" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=wikiquote_fr_all_nopic_2019-06" />
  </entry>
  <entry>
    <title>Géographie par Wikipédia</title>
    <id>urn:uuid:1123e574-6eef-6d54-28fc-13e4caeae474</id>
    <icon>/meta?name=favicon&amp;content=wikipedia_fr_geography_nopic_2019-06</icon>
    <updated>2019-06-02T00:00::00:Z</updated>
    <summary>Une sélection d'articles de Wikipédia sur la géographie</summary>
    <language>fra</language>
    <category>category_element_overrides_tags</category>
    <tags>wikipedia;nopic;_category:tags_override_category_element</tags>
    <link type="text/html" href="/wikipedia_fr_geography_nopic_2019-06" />
    <author>
      <name>Wikipedia</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/wikipedia/wikipedia_fr_geography_nopic_2019-06.zim.meta4" length="157586432" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=wikipedia_fr_geography_nopic_2019-06" />
  </entry>
  <entry>
    <title>Mathématiques</title>
    <id>urn:uuid:14829621-c490-c376-0792-9de558b57efa</id>
    <icon>/meta?name=favicon&amp;content=wikipedia_fr_mathematics_nopic_2019-05</icon>
    <updated>2019-05-13T00:00::00:Z</updated>
    <language>fra</language>
    <summary>Une</summary>
    <tags>wikipedia;nopic;_category:tags_override_category_element</tags>
    <category>category_element_overrides_tags</category>
    <link type="text/html" href="/wikipedia_fr_mathematics_nopic_2019-05" />
    <author>
      <name>Wikipedia</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/wikipedia/wikipedia_fr_mathematics_nopic_2019-05.zim.meta4" length="223368192" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=wikipedia_fr_mathematics_nopic_2019-05" />
  </entry>
<entry>
    <title>Granblue Fantasy Wiki</title>
    <id>urn:uuid:006cbd1b-16d8-b00d-a584-c1ae110a94ed</id>
    <icon>/meta?name=favicon&amp;content=granbluefantasy_en_all_all_nopic_2018-10</icon>
    <updated>2018-10-14T00:00::00:Z</updated>
    <language>eng</language>
    <summary>Granblue Fantasy Wiki</summary>
    <tags>gbf;nopic;_ftindex</tags>
    <link type="text/html" href="/granbluefantasy_en_all_all_nopic_2018-10" />
    <author>
      <name>Wiki</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/other/granbluefantasy_en_all_all_nopic_2018-10.zim.meta4" length="23197696" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=granbluefantasy_en_all_all_nopic_2018-10" />
  </entry>
  <entry>
    <title>Movies &amp; TV Stack Exchange</title>
    <id>urn:uuid:00f37b00-f4da-0675-995a-770f9c72903e</id>
    <icon>/meta?name=favicon&amp;content=movies.stackexchange.com_en_all_2019-02</icon>
    <updated>2019-02-03T00:00::00:Z</updated>
    <language>eng</language>
    <summary>Q&amp;A for movie and tv enthusiasts</summary>
    <tags>stackexchange;_ftindex</tags>
    <link type="text/html" href="/movies.stackexchange.com_en_all_2019-02" />
    <author>
      <name>Movies &amp; TV Stack Exchange</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/stack_exchange/movies.stackexchange.com_en_all_2019-02.zim.meta4" length="859463680" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=movies.stackexchange.com_en_all_2019-02" />
  </entry>
  <entry>
    <title>TED talks - Business</title>
    <id>urn:uuid:0189d9be-2fd0-b4b6-7300-20fab0b5cdc8</id>
    <icon>/meta?name=favicon&amp;content=ted_en_business_2018-07</icon>
    <updated>2018-07-23T00:00::00:Z</updated>
    <language>eng</language>
    <summary>Ideas worth spreading</summary>
    <tags></tags>
    <link type="text/html" href="/ted_en_business_2018-07" />
    <author>
      <name>TED</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/ted/ted_en_business_2018-07.zim.meta4" length="8855827456" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=ted_en_business_2018-07" />
  </entry>
  <entry>
    <title>Mythology &amp; Folklore Stack Exchange</title>
    <id>urn:uuid:028055ac-4acc-1d54-65e0-a96de45e1b22</id>
    <icon>/meta?name=favicon&amp;content=mythology.stackexchange.com_en_all_2019-02</icon>
    <updated>2019-02-03T00:00::00:Z</updated>
    <language>eng</language>
    <summary>Q&amp;A for enthusiasts and scholars of mythology and folklore</summary>
    <tags>stackexchange;_ftindex</tags>
    <link type="text/html" href="/mythology.stackexchange.com_en_all_2019-02" />
    <author>
      <name>Mythology &amp; Folklore Stack Exchange</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/stack_exchange/mythology.stackexchange.com_en_all_2019-02.zim.meta4" length="47005696" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=mythology.stackexchange.com_en_all_2019-02" />
  </entry>
  <entry>
    <title>Islam Stack Exchange</title>
    <id>urn:uuid:02e9c7ff-36fc-9c6e-6ac7-cd7085989029</id>
    <icon>/meta?name=favicon&amp;content=islam.stackexchange.com_en_all_2019-01</icon>
    <updated>2019-01-31T00:00::00:Z</updated>
    <language>eng</language>
    <summary>Q&amp;A for Muslims, experts in Islam, and those interested in learning more about Islam</summary>
    <tags>stackexchange;_ftindex</tags>
    <link type="text/html" href="/islam.stackexchange.com_en_all_2019-01" />
    <author>
      <name>Islam Stack Exchange</name>
    </author>
    <link rel="http://opds-spec.org/acquisition/open-access" type="application/x-zim" href="http://download.kiwix.org/zim/stack_exchange/islam.stackexchange.com_en_all_2019-01.zim.meta4" length="135346176" />
    <link rel="http://opds-spec.org/image/thumbnail" type="image/png" href="/meta?name=favicon&amp;content=islam.stackexchange.com_en_all_2019-01" />
  </entry>
</feed>

)";

const char sampleLibraryXML[] = R"(
<library version="1.0">
  <book
        id="raycharles"
        path="./zimfile.zim"
        url="https://github.com/kiwix/libkiwix/raw/master/test/data/zimfile.zim"
        title="Ray Charles"
        description="Wikipedia articles about Ray Charles"
        language="eng"
        creator="Wikipedia"
        publisher="Kiwix"
        date="2020-03-31"
        name="wikipedia_en_ray_charles"
        tags="wikipedia;_category:wikipedia;_pictures:no"
        articleCount="284"
        mediaCount="2"
        size="556"
      ></book>
  <book
        id="example"
        path="./example.zim"
        title="An example ZIM archive"
        description="An eXaMpLe book added to the catalog via XML"
        language="deu"
        creator="Wikibooks"
        publisher="Kiwix & Some Enthusiasts"
        date="2021-04-11"
        name="wikibooks_de"
        tags="unittest;wikibooks;_category:wikibooks"
        articleCount="12"
        mediaCount="0"
        size="126"
      ></book>
</library>
)";

#include "../include/library.h"
#include "../include/manager.h"
#include "../include/bookmark.h"

namespace
{

class LibraryTest : public ::testing::Test {
 protected:
  typedef kiwix::Library::BookIdCollection BookIdCollection;
  typedef std::vector<std::string> TitleCollection;

  void SetUp() override {
     kiwix::Manager manager(&lib);
     manager.readOpds(sampleOpdsStream, "foo.urlHost");
     manager.readXml(sampleLibraryXML, true, "./test/library.xml", true);
  }

    kiwix::Bookmark createBookmark(const std::string &id) {
        kiwix::Bookmark bookmark;
        bookmark.setBookId(id);
        return bookmark;
    };

  TitleCollection ids2Titles(const BookIdCollection& ids) {
    TitleCollection titles;
    for ( const auto& bookId : ids ) {
      titles.push_back(lib.getBookById(bookId).getTitle());
    }
    std::sort(titles.begin(), titles.end());
    return titles;
  }

  kiwix::Library lib;
};

TEST_F(LibraryTest, getBookMarksTest)
{
    auto bookId1 = lib.getBooksIds()[0];
    auto bookId2 = lib.getBooksIds()[1];

    lib.addBookmark(createBookmark(bookId1));
    lib.addBookmark(createBookmark("invalid-bookmark-id"));
    lib.addBookmark(createBookmark(bookId2));
    auto onlyValidBookmarks = lib.getBookmarks();
    auto allBookmarks = lib.getBookmarks(false);

    EXPECT_EQ(onlyValidBookmarks[0].getBookId(), bookId1);
    EXPECT_EQ(onlyValidBookmarks[1].getBookId(), bookId2);

    EXPECT_EQ(allBookmarks[0].getBookId(), bookId1);
    EXPECT_EQ(allBookmarks[1].getBookId(), "invalid-bookmark-id");
    EXPECT_EQ(allBookmarks[2].getBookId(), bookId2);
}

TEST_F(LibraryTest, sanityCheck)
{
  EXPECT_EQ(lib.getBookCount(true, true), 12U);
  EXPECT_EQ(lib.getBooksLanguages(),
            std::vector<std::string>({"deu", "eng", "fra"})
  );
  EXPECT_EQ(lib.getBooksCreators(), std::vector<std::string>({
      "Islam Stack Exchange",
      "Movies & TV Stack Exchange",
      "Mythology & Folklore Stack Exchange",
      "TED",
      "Tania Louis",
      "Wiki",
      "Wikibooks",
      "Wikipedia",
      "Wikiquote"
  }));
  EXPECT_EQ(lib.getBooksPublishers(), std::vector<std::string>({
      "",
      "Kiwix",
      "Kiwix & Some Enthusiasts"
  }));
}

TEST_F(LibraryTest, categoryHandling)
{
  EXPECT_EQ("", lib.getBookById("0c45160e-f917-760a-9159-dfe3c53cdcdd").getCategory());
  EXPECT_EQ("category_defined_via_tags_only", lib.getBookById("0d0bcd57-d3f6-cb22-44cc-a723ccb4e1b2").getCategory());
  EXPECT_EQ("category_defined_via_category_element_only", lib.getBookById("0ea1cde6-441d-6c58-f2c7-21c2838e659f").getCategory());
  EXPECT_EQ("category_element_overrides_tags", lib.getBookById("1123e574-6eef-6d54-28fc-13e4caeae474").getCategory());
  EXPECT_EQ("category_element_overrides_tags", lib.getBookById("14829621-c490-c376-0792-9de558b57efa").getCategory());
}

TEST_F(LibraryTest, emptyFilter)
{
  const auto bookIds = lib.filter(kiwix::Filter());
  EXPECT_EQ(bookIds, lib.getBooksIds());
}

#define EXPECT_FILTER_RESULTS(f, ...)        \
        EXPECT_EQ(                           \
          ids2Titles(lib.filter(f)),         \
          TitleCollection({ __VA_ARGS__ })   \
        )

TEST_F(LibraryTest, filterLocal)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().local(true),
    "An example ZIM archive",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().local(false),
    "Encyclopédie de la Tunisie",
    "Granblue Fantasy Wiki",
    "Géographie par Wikipédia",
    "Islam Stack Exchange",
    "Mathématiques",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange",
    "TED talks - Business",
    "Tania Louis",
    "Wikiquote"
  );
}

TEST_F(LibraryTest, filterRemote)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().remote(true),
    "Encyclopédie de la Tunisie",
    "Granblue Fantasy Wiki",
    "Géographie par Wikipédia",
    "Islam Stack Exchange",
    "Mathématiques",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange",
    "Ray Charles",
    "TED talks - Business",
    "Tania Louis",
    "Wikiquote"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().remote(false),
    "An example ZIM archive"
  );
}

TEST_F(LibraryTest, filterByLanguage)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().lang("eng"),
    "Granblue Fantasy Wiki",
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange",
    "Ray Charles",
    "TED talks - Business"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("lang:eng"),
    "Granblue Fantasy Wiki",
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange",
    "Ray Charles",
    "TED talks - Business"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("eng"),
    /* no results */
  );
}

TEST_F(LibraryTest, filterByTags)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"stackexchange"}),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by tags is case and diacritics insensitive
  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"ståckEXÇhange"}),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by tags requires full match of the search term
  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"stackexch"}),
    /* no results */
  );

  // in tags with values (tag:value form) the value is an inseparable
  // part of the tag
  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"_category"}),
    /* no results */
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"_category:category_defined_via_tags_only"}),
    "Tania Louis"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"wikipedia"}),
    "Encyclopédie de la Tunisie",
    "Géographie par Wikipédia",
    "Mathématiques",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"wikipedia", "nopic"}),
    "Géographie par Wikipédia",
    "Mathématiques"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().acceptTags({"wikipedia"}).rejectTags({"nopic"}),
    "Encyclopédie de la Tunisie",
    "Ray Charles"
  );
}


TEST_F(LibraryTest, filterByQuery)
{
  // filtering by query checks the title
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Exchange"),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by query checks the description/summary
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("enthusiasts"),
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by query is case insensitive on titles
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("ExcHANge"),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by query is diacritics insensitive on titles
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("mathematiques"),
    "Mathématiques",
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("èxchângé"),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by query is case insensitive on description/summary
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("enTHUSiaSTS"),
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by query is diacritics insensitive on description/summary
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("selection"),
    "Géographie par Wikipédia"
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("enthúsïåsts"),
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // by default, filtering by query assumes partial query
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Wiki"),
    "Encyclopédie de la Tunisie",
    "Granblue Fantasy Wiki",
    "Géographie par Wikipédia",
    "Ray Charles",
    "Wikiquote"
  );

  // partial query can be disabled
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Wiki", false),
    "Granblue Fantasy Wiki"
  );
}


TEST_F(LibraryTest, filteringByEmptyQueryReturnsAllEntries)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().query(""),
    "An example ZIM archive",
    "Encyclopédie de la Tunisie",
    "Granblue Fantasy Wiki",
    "Géographie par Wikipédia",
    "Islam Stack Exchange",
    "Mathématiques",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange",
    "Ray Charles",
    "TED talks - Business",
    "Tania Louis",
    "Wikiquote"
  );
}

TEST_F(LibraryTest, filterByCreator)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Wikipedia"),
    "Encyclopédie de la Tunisie",
    "Géographie par Wikipédia",
    "Mathématiques",
    "Ray Charles"
  );

  // filtering by creator requires full match of the search term
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Wiki"),
    "Granblue Fantasy Wiki"
  );

  // filtering by creator is case and diacritics insensitive
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("wIkï"),
    "Granblue Fantasy Wiki"
  );

  // filtering by creator doesn't requires full match of the full creator name
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Stack"),
    "Islam Stack Exchange",
    "Movies & TV Stack Exchange",
    "Mythology & Folklore Stack Exchange"
  );

  // filtering by creator requires a full phrase match (ignoring some non-word terms)
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Movies & TV Stack Exchange"),
    "Movies & TV Stack Exchange"
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Movies & TV"),
    "Movies & TV Stack Exchange"
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("Movies TV"),
    "Movies & TV Stack Exchange"
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("TV & Movies"),
    /* no results */
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().creator("TV Movies"),
    /* no results */
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("creator:Wikipedia"),
    "Encyclopédie de la Tunisie",
    "Géographie par Wikipédia",
    "Mathématiques",
    "Ray Charles"
  );

}

TEST_F(LibraryTest, filterByPublisher)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().publisher("Kiwix"),
    "An example ZIM archive",
    "Ray Charles"
  );

  // filtering by publisher requires full match of the search term
  EXPECT_FILTER_RESULTS(kiwix::Filter().publisher("Kiwi"),
    /* no results */
  );

  // filtering by publisher requires a full phrase match
  EXPECT_FILTER_RESULTS(kiwix::Filter().publisher("Kiwix & Some Enthusiasts"),
    "An example ZIM archive"
  );
  EXPECT_FILTER_RESULTS(kiwix::Filter().publisher("Some Enthusiasts & Kiwix"),
    /* no results */
  );

  // filtering by publisher is case and diacritics insensitive
  EXPECT_FILTER_RESULTS(kiwix::Filter().publisher("kîWIx"),
    "An example ZIM archive",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("publisher:kiwix"),
    "An example ZIM archive",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("kiwix"),
    /* no results */
  );
}

TEST_F(LibraryTest, filterByName)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().name("wikibooks_de"),
    "An example ZIM archive"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("name:wikibooks_de"),
    "An example ZIM archive"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("wikibooks_de"),
    /* no results */
  );
}

TEST_F(LibraryTest, filterByCategory)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().category("category_element_overrides_tags"),
    "Géographie par Wikipédia",
    "Mathématiques"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("category:category_element_overrides_tags"),
    "Géographie par Wikipédia",
    "Mathématiques"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("category_element_overrides_tags"),
    /* no results */
  );
}

TEST_F(LibraryTest, filterByMaxSize)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().maxSize(200000),
    "An example ZIM archive"
  );
}

TEST_F(LibraryTest, filterByMultipleCriteria)
{
  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Wiki").creator("Wikipedia"),
    "Encyclopédie de la Tunisie",
    "Géographie par Wikipédia",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Wiki").creator("Wikipedia").maxSize(100000000UL),
    "Encyclopédie de la Tunisie",
    "Ray Charles"
  );

  EXPECT_FILTER_RESULTS(kiwix::Filter().query("Wiki").creator("Wikipedia").maxSize(100000000UL).local(false),
    "Encyclopédie de la Tunisie"
  );
}

TEST_F(LibraryTest, getBookByPath)
{
  auto& book = lib.getBookById(lib.getBooksIds()[0]);
#ifdef _WIN32
  auto path = "C:\\some\\abs\\path.zim";
#else
  auto path = "/some/abs/path.zim";
#endif
  book.setPath(path);
  EXPECT_EQ(lib.getBookByPath(path).getId(), book.getId());
  EXPECT_THROW(lib.getBookByPath("non/existant/path.zim"), std::out_of_range);
}

TEST_F(LibraryTest, removeBookByIdRemovesTheBook)
{
  const auto initialBookCount = lib.getBookCount(true, true);
  ASSERT_GT(initialBookCount, 0U);
  EXPECT_NO_THROW(lib.getBookById("raycharles"));
  lib.removeBookById("raycharles");
  EXPECT_EQ(initialBookCount - 1, lib.getBookCount(true, true));
  EXPECT_THROW(lib.getBookById("raycharles"), std::out_of_range);
};

TEST_F(LibraryTest, removeBookByIdDropsTheReader)
{
  EXPECT_NE(nullptr, lib.getReaderById("raycharles"));
  lib.removeBookById("raycharles");
  EXPECT_THROW(lib.getReaderById("raycharles"), std::out_of_range);
};

TEST_F(LibraryTest, removeBookByIdUpdatesTheSearchDB)
{
  kiwix::Filter f;
  f.local(true).valid(true).query(R"(title:"ray charles")", false);

  EXPECT_NO_THROW(lib.getBookById("raycharles"));
  EXPECT_EQ(1U, lib.filter(f).size());

  lib.removeBookById("raycharles");

  EXPECT_THROW(lib.getBookById("raycharles"), std::out_of_range);
  EXPECT_EQ(0U, lib.filter(f).size());

  // make sure that Library::filter() doesn't add an empty book with
  // an id surviving in the search DB
  EXPECT_THROW(lib.getBookById("raycharles"), std::out_of_range);
};

};
