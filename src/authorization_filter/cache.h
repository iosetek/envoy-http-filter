#pragma once

#include <string>

#include <unordered_map>

#include "envoy/thread_local/thread_local.h"
#include "envoy/thread_local/thread_local_object.h"

namespace Envoy {
namespace Http {

struct AuthorizationCacheTLS : public ThreadLocal::ThreadLocalObject, Logger::Loggable<Logger::Id::filter> {
  absl::optional<bool> isAuthorized(const std::string& user);

  void addUserEntry(const std::string& user, bool authorized);

  std::unordered_map<std::string, bool> authorized_users_;
};

} // namespace Http
} // namespace Envoy
