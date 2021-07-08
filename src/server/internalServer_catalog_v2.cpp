/*
 * Copyright 2021 Veloman Yunkan <veloman.yunkan@gmail.com>
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

#include "internalServer.h"

#include "library.h"
#include "opds_dumper.h"
#include "request_context.h"
#include "response.h"
#include "tools/otherTools.h"
#include "kiwixlib-resources.h"

#include <mustache.hpp>

#include <string>
#include <vector>

namespace kiwix {

std::unique_ptr<Response> InternalServer::handle_catalog_v2(const RequestContext& request)
{
  if (m_verbose.load()) {
    printf("** running handle_catalog_v2");
  }

  std::string url;
  try {
    url  = request.get_url_part(2);
  } catch (const std::out_of_range&) {
    return Response::build_404(*this, request, "", "");
  }

  if (url == "root.xml") {
    return handle_catalog_v2_root(request);
  } else if (url == "searchdescription.xml") {
    const std::string endpoint_root = m_root + "/catalog/v2";
    return ContentResponse::build(*this,
        RESOURCE::catalog_v2_searchdescription_xml,
        kainjow::mustache::object({{"endpoint_root", endpoint_root}}),
        "application/opensearchdescription+xml"
    );
  } else if (url == "entries") {
    return handle_catalog_v2_entries(request);
  } else if (url == "categories") {
    return handle_catalog_v2_categories(request);
  } else if (url == "languages") {
    return handle_catalog_v2_languages(request);
  } else {
    return Response::build_404(*this, request, "", "");
  }
}

std::unique_ptr<Response> InternalServer::handle_catalog_v2_root(const RequestContext& request)
{
  return ContentResponse::build(
             *this,
             RESOURCE::templates::catalog_v2_root_xml,
             kainjow::mustache::object{
               {"date", gen_date_str()},
               {"endpoint_root", m_root + "/catalog/v2"},
               {"feed_id", gen_uuid(m_library_id)},
               {"all_entries_feed_id", gen_uuid(m_library_id + "/entries")},
               {"category_list_feed_id", gen_uuid(m_library_id + "/categories")},
               {"language_list_feed_id", gen_uuid(m_library_id + "/languages")}
             },
             "application/atom+xml;profile=opds-catalog;kind=navigation"
  );
}

std::unique_ptr<Response> InternalServer::handle_catalog_v2_entries(const RequestContext& request)
{
  OPDSDumper opdsDumper(mp_library);
  opdsDumper.setRootLocation(m_root);
  opdsDumper.setLibraryId(m_library_id);
  const auto bookIds = search_catalog(request, opdsDumper);
  const auto opdsFeed = opdsDumper.dumpOPDSFeedV2(bookIds, request.get_query());
  return ContentResponse::build(
             *this,
             opdsFeed,
             "application/atom+xml;profile=opds-catalog;kind=acquisition"
  );
}

std::unique_ptr<Response> InternalServer::handle_catalog_v2_categories(const RequestContext& request)
{
  OPDSDumper opdsDumper(mp_library);
  opdsDumper.setRootLocation(m_root);
  opdsDumper.setLibraryId(m_library_id);
  return ContentResponse::build(
             *this,
             opdsDumper.categoriesOPDSFeed(mp_library->getBooksCategories()),
             "application/atom+xml;profile=opds-catalog;kind=navigation"
  );
}

std::unique_ptr<Response> InternalServer::handle_catalog_v2_languages(const RequestContext& request)
{
  OPDSDumper opdsDumper(mp_library);
  opdsDumper.setRootLocation(m_root);
  opdsDumper.setLibraryId(m_library_id);
  return ContentResponse::build(
             *this,
             opdsDumper.languagesOPDSFeed(),
             "application/atom+xml;profile=opds-catalog;kind=navigation"
  );
}

} // namespace kiwix
