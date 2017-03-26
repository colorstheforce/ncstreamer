/**
 * Copyright (C) 2017 NCSOFT Corporation
 */


#ifndef SRC_STREAMING_SERVICE_FACEBOOK_API_H_
#define SRC_STREAMING_SERVICE_FACEBOOK_API_H_


#include <string>
#include <vector>

#include "src/lib/uri.h"


namespace ncstreamer {
class FacebookApi {
 public:
  class Login;
  class Graph;

 private:
  static const wchar_t *kScheme;
  static const wchar_t *kVersion;
};


class FacebookApi::Login {
 public:
  class Oauth;
  class Redirect;

 private:
  static const wchar_t *kAuthority;
};


class FacebookApi::Login::Oauth {
 public:
  static Uri BuildUri(
      const std::wstring &client_id,
      const Uri &redirect_uri,
      const std::wstring &response_type,
      const std::wstring &display,
      const std::vector<std::wstring> &scope);

 private:
  static const std::wstring &static_path();
};


class FacebookApi::Login::Redirect {
 public:
  static const Uri &static_uri();

  static std::wstring ExtractAccessToken(const Uri::Query &query);
};


class FacebookApi::Graph {
 public:
  class Me;

 private:
  static const wchar_t *kAuthority;
};


class FacebookApi::Graph::Me {
 public:
  static const Uri &static_uri();
  static Uri BuildUri(const std::wstring &access_token);

 private:
  static const std::wstring &static_path();
};
}  // namespace ncstreamer


#endif  // SRC_STREAMING_SERVICE_FACEBOOK_API_H_
