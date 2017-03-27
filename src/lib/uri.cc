/**
 * Copyright (C) 2017 NCSOFT Corporation
 */


#include "src/lib/uri.h"

#include <regex>  // NOLINT
#include <sstream>

#include "include/cef_parser.h"


namespace ncstreamer {
Uri::Query::Query(const ParamVector &params)
    : query_string_{ToString(params)},
      params_{} {
  for (const auto &param : params) {
    params_.emplace(param);
  }
}


Uri::Query::Query(const std::wstring &query_string)
    : query_string_{query_string},
      params_{} {
  static const std::wregex kParamPattern{LR"(([\w+%]+)=([^&]*))"};

  for (std::wsregex_iterator i{
      query_string.begin(), query_string.end(), kParamPattern};
      i != std::wsregex_iterator{}; ++i) {
    const auto &elem = *i;
    const std::wstring &key = elem[1].str();
    const std::wstring &value = elem[2].str();
    params_.emplace(key, value);
  }
}


Uri::Query::Query()
    : query_string_{},
      params_{} {
}


Uri::Query::~Query() {
}


std::wstring Uri::Query::ToString(const ParamVector &query) {
  if (query.empty() == true) {
    return L"";
  }

  static const auto to_string = [](const ParamVector::const_iterator &i) {
    std::wstringstream ss;
    ss << i->first << L"=" << Encode(i->second);
    return ss.str();
  };

  std::wstringstream ss;
  auto i = query.begin();
  ss << to_string(i);
  for (++i; i != query.end(); ++i) {
    ss << L"&" << to_string(i);
  }
  return ss.str();
}


const std::wstring &Uri::Query::GetParameter(const std::wstring &key) const {
  static const std::wstring kEmpty{};

  auto i = params_.find(key);
  return (i != params_.end()) ? i->second : kEmpty;
}


std::wstring Uri::Query::Encode(const std::wstring &raw) {
  return ::CefURIEncode(raw, false).c_str();
}


std::size_t Uri::Hasher::operator()(const Uri &uri) const {
  return std::hash<std::wstring>()(uri.uri_string());
}


Uri::Uri(
    const std::wstring &scheme,
    const std::wstring &authority,
    const std::wstring &path,
    const Query &query,
    const std::wstring &fragment)
    : scheme_{scheme},
      authority_{authority},
      path_{path},
      query_{query},
      fragment_{fragment},
      scheme_authority_path_{ToString(scheme, authority, path, {}, L"")},
      uri_string_{ToString(scheme_authority_path_, query, fragment)} {
}


Uri::Uri(
    const std::wstring &scheme,
    const std::wstring &authority,
    const std::wstring &path,
    const Query &query)
    : scheme_{scheme},
      authority_{authority},
      path_{path},
      query_{query},
      fragment_{},
      scheme_authority_path_{ToString(scheme, authority, path, {}, L"")},
      uri_string_{ToString(scheme_authority_path_, query, L"")} {
}


Uri::Uri(
    const std::wstring &scheme,
    const std::wstring &authority,
    const std::wstring &path)
    : scheme_{scheme},
      authority_{authority},
      path_{path},
      query_{},
      fragment_{},
      scheme_authority_path_{ToString(scheme, authority, path, {}, L"")},
      uri_string_{scheme_authority_path_} {
}


Uri::Uri(const std::wstring &uri_string)
    : scheme_{},
      authority_{},
      path_{},
      query_{},
      fragment_{},
      scheme_authority_path_{},
      uri_string_{uri_string} {
  // from https://tools.ietf.org/html/rfc3986#appendix-B
  static const std::wregex kUriPattern{
      LR"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)"};

  std::wsmatch matches;
  bool found = std::regex_search(uri_string, matches, kUriPattern);
  if (found) {
    scheme_ = matches[2];
    authority_ = matches[4];
    path_ = matches[5];
    query_ = Query{matches[7]};
    fragment_ = matches[9];
  }
  scheme_authority_path_ = ToString(scheme_, authority_, path_, {}, L"");
}


Uri::~Uri() {
}


std::wstring Uri::ToString(
    const std::wstring &scheme_authority_path,
    const Query &query,
    const std::wstring &fragment) {
  std::wstringstream ss;
  ss << scheme_authority_path;
  if (query.query_string().empty() == false) {
    ss << L"?" << query.query_string();
  }
  if (fragment.empty() == false) {
    ss << L"#" << fragment;
  }
  return ss.str();
}


std::wstring Uri::ToString(
    const std::wstring &scheme,
    const std::wstring &authority,
    const std::wstring &path,
    const Query &query,
    const std::wstring &fragment) {
  std::wstringstream ss;
  ss << scheme << L"://";
  ss << authority;
  if (path.empty() == false) {
    ss << path;
  }
  return ToString(ss.str(), query, fragment);
}
}  // namespace ncstreamer