/**
 * Copyright (C) 2017 NCSOFT Corporation
 */


#include "ncstreamer_cef/src/local_storage.h"

#include <cassert>
#include <fstream>

#include "boost/property_tree/json_parser.hpp"


namespace ncstreamer {
void LocalStorage::SetUp(const std::wstring &storage_path) {
  assert(!static_instance);
  static_instance = new LocalStorage{storage_path};
}


void LocalStorage::ShutDown() {
  assert(static_instance);
  delete static_instance;
  static_instance = nullptr;
}


LocalStorage *LocalStorage::Get() {
  assert(static_instance);
  return static_instance;
}


std::string LocalStorage::GetUserPage() const {
  return storage_.get(kUserPage, "");
}


std::string LocalStorage::GetPrivacy() const {
  return storage_.get(kPrivacy, "");
}


std::string LocalStorage::GetStreamServer() const {
  return storage_.get(kStreamServer, "");
}


std::string LocalStorage::GetDesignatedUser() const {
  return storage_.get(kDesignatedUser, "");
}


std::string LocalStorage::GetDescription() const {
  return storage_.get(kDescription, "");
}

bool LocalStorage::GetWebcamUse() const {
  return storage_.get<bool>(kWebcamUse, false);
}


boost::optional<Position<int>> LocalStorage::GetWindowPosition() const {
  boost::optional<const boost::property_tree::ptree &> pos =
      storage_.get_child_optional(kWindowPosition);
  if (!pos) {
    return boost::none;
  }
  return Position<int>{
      pos->get(kWindowPositionX, 0),
      pos->get(kWindowPositionY, 0)};
}


std::string LocalStorage::GetYouTubePrivacy() const {
  return storage_.get(kYouTubePrivacy, "");
}


void LocalStorage::SetUserPage(const std::string &user_page) {
  SetValue(kUserPage, user_page);
}


void LocalStorage::SetPrivacy(const std::string &privacy) {
  SetValue(kPrivacy, privacy);
}


void LocalStorage::SetStreamServer(const std::string &stream_server) {
  SetValue(kStreamServer, stream_server);
}


void LocalStorage::SetDesignatedUser(const std::string &designated_user) {
  SetValue(kDesignatedUser, designated_user);
}


void LocalStorage::SetDescription(const std::string &description) {
  SetValue(kDescription, description);
}

void LocalStorage::SetWebcamUse(const bool &use) {
  SetValue(kWebcamUse, use);
}


void LocalStorage::SetWindowPosition(const Position<int> &window_position) {
  boost::property_tree::ptree pos;
  pos.put(kWindowPositionX, window_position.x());
  pos.put(kWindowPositionY, window_position.y());

  SetValue(kWindowPosition, pos);
}


void LocalStorage::SetYouTubePrivacy(const std::string &privacy) {
  SetValue(kYouTubePrivacy, privacy);
}


LocalStorage::LocalStorage(const std::wstring &storage_path)
    : storage_path_{storage_path},
      storage_{LoadFromFile(storage_path)} {
}


LocalStorage::~LocalStorage() {
}


boost::property_tree::ptree LocalStorage::LoadFromFile(
    const std::wstring &file_path) {
  boost::property_tree::ptree tree;

  if (file_path.empty()) {
    return tree;
  }

  std::ifstream fs{file_path};
  try {
    boost::property_tree::json_parser::read_json(fs, tree);
  } catch (...) {
    tree.clear();
  }
  return tree;
}


void LocalStorage::SaveToFile(
    const boost::property_tree::ptree &tree,
    const std::wstring &file_path) {
  if (file_path.empty()) {
    return;
  }
  std::ofstream fs{file_path};
  boost::property_tree::write_json(fs, tree);
}


template<typename T>
    void LocalStorage::SetValue(const std::string &key, const T &value) {
  storage_.put(key, value);
  SaveToFile(storage_, storage_path_);
}


template
void LocalStorage::SetValue<std::string>(
    const std::string &key, const std::string &value);
template
void LocalStorage::SetValue<int>(
    const std::string &key, const int &value);
template
void LocalStorage::SetValue<bool>(
    const std::string &key, const bool &value);


void LocalStorage::SetValue(
    const std::string &key,
    const boost::property_tree::ptree &value) {
  storage_.put_child(key, value);
  SaveToFile(storage_, storage_path_);
}


const char *LocalStorage::kUserPage{"userPage"};
const char *LocalStorage::kPrivacy{"privacy"};
const char *LocalStorage::kStreamServer{"streamServer"};
const char *LocalStorage::kDesignatedUser{"designatedUser"};
const char *LocalStorage::kDescription{"description"};
const char *LocalStorage::kWebcamUse{"webcamUse"};
const char *LocalStorage::kWindowPosition{"windowPosition"};
const char *LocalStorage::kWindowPositionX{"x"};
const char *LocalStorage::kWindowPositionY{"y"};
const char *LocalStorage::kYouTubePrivacy{"YouTubePrivacy"};


LocalStorage *LocalStorage::static_instance{nullptr};
}  // namespace ncstreamer
