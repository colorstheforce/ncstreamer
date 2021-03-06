/**
* Copyright (C) 2017 NCSOFT Corporation
*/


#ifndef NCSTREAMER_CEF_SRC_STREAMING_SERVICE_TWITCH_H_
#define NCSTREAMER_CEF_SRC_STREAMING_SERVICE_TWITCH_H_


#include <mutex>  // NOLINT
#include <string>
#include <vector>

#include "include/cef_life_span_handler.h"
#include "include/cef_request_handler.h"

#include "ncstreamer_cef/src/lib/cef_fit_client.h"
#include "ncstreamer_cef/src/lib/http_request_service.h"
#include "ncstreamer_cef/src/lib/uri.h"
#include "ncstreamer_cef/src/streaming_service/streaming_service_provider.h"
#include "ncstreamer_cef/src/streaming_service/twitch_chat.h"


namespace ncstreamer {
class Twitch : public StreamingServiceProvider {
 public:
  Twitch();
  virtual ~Twitch();

  void LogIn(
      HWND parent,
      const std::wstring &locale,
      const OnFailed &on_failed,
      const OnLoggedIn &on_logged_in) override;

  void LogOut(
      const OnFailed &on_failed,
      const OnLoggedOut &on_logged_out) override;

  void PostLiveVideo(
      const std::string &stream_server,
      const std::string &user_page_id,
      const std::string &privacy,
      const std::string &title,
      const std::string &description,
      const std::string &app_attribution_tag,
      const OnFailed &on_failed,
      const OnLiveVideoPosted &on_live_video_posted) override;

  void GetComments(
      const std::string &created_time,
      const OnFailed &on_failed,
      const OnCommentsGot &on_comments_got) override;

  void GetLiveVideoViewers(
      const OnFailed &on_failed,
      const OnLiveVideoViewers &on_live_video_viewers) override;

  void StopLiveVideo() override;

 private:
  using OnUserGotten = std::function<void(
      const std::string &id,
      const std::string &name,
      const std::string &user_page)>;

  using OnChannelGotten = std::function<void(
      const std::string &channel_id,
      const std::string &post_url,
      const std::string &stream_key)>;

  using OnServerListGotten = std::function<void(
      const std::vector<StreamServer>)>;

  class LoginClient;

  void GetUser(
      const OnFailed &on_failed,
      const OnUserGotten &on_user_gotten);

  void GetChannel(
      const OnFailed &on_failed,
      const OnChannelGotten &on_channel_gotten);

  void UpdateChannel(
      const std::string &channel_id,
      const std::string &description,
      const std::string &game,
      const std::string &stream_server,
      const std::string &stream_key,
      const std::string &post_url,
      const OnFailed &on_failed,
      const OnLiveVideoPosted &on_live_video_posted);

  void GetStreamServers(
      const OnFailed &on_failed,
      const OnServerListGotten &on_server_list_gotten);

  void OnLoginSuccess(
      const std::string &access_token,
      const OnFailed &on_failed,
      const OnLoggedIn &on_logged_in);

  std::string GetAccessToken() const;
  void SetAccessToken(const std::string &access_token);

  std::string GetNickName() const;
  void SetNickName(const std::string &nick_name);

  std::string GetAccountNameLowerCase() const;
  void SetAccountName(const std::string &account_name);

  std::string GetChannelId() const;
  void SetChannelId(const std::string &channel_id);

  CefRefPtr<LoginClient> login_client_;
  HttpRequestService http_request_service_;

  // TODO(khpark): refactoring this by AccessToken class.
  mutable std::mutex access_token_mutex_;
  std::string access_token_;

  mutable std::mutex nick_name_mutex_;
  std::string nick_name_;

  mutable std::mutex account_name_mutex_;
  std::string account_name_;

  mutable std::mutex channel_id_mutex_;
  std::string channel_id_;

  TwitchChat chat_;
};


class Twitch::LoginClient
    : public CefFitClient,
      public CefLifeSpanHandler,
      public CefRequestHandler {

 public:
  LoginClient(
      Twitch *const owner,
      const HWND &base_window,
      const OnFailed &on_failed,
      const OnLoggedIn &on_logged_in);
  virtual ~LoginClient();

 protected:
  // overrides CefClient
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefRequestHandler> GetRequestHandler() override;

  // overrides CefLifeSpanHandler
  void OnAfterCreated(
      CefRefPtr<CefBrowser> browser) override;

  // overrides CefRequestHandler
  bool OnBeforeBrowse(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_redirect) override;

 private:
  bool OnLoginRedirected(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_redirect,
      const Uri &uri);

  Twitch *const owner_;
  const HWND base_window_;

  OnFailed on_failed_;
  OnLoggedIn on_logged_in_;

  IMPLEMENT_REFCOUNTING(LoginClient);
};
}  // namespace ncstreamer


#endif  // NCSTREAMER_CEF_SRC_STREAMING_SERVICE_TWITCH_H_
