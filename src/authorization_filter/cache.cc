#include "cache.h"

namespace Envoy {
namespace Http {

absl::optional<bool> AuthorizationCacheTLS::isAuthorized(const std::string& user) {
  ENVOY_LOG(trace, "Checking cache storage for user '{}'", user);
  auto entry = authorized_users_.find(user);
  if (entry == nullptr) {
    ENVOY_LOG(trace, "User '{}' not found in cache storage", user);
    return absl::nullopt;
  }
  ENVOY_LOG(trace, "User '{}' found in cache storage. Authorization: {}", user, entry->second);
  return entry->second;
}

void AuthorizationCacheTLS::addUserEntry(const std::string& user, bool authorized) {
  ENVOY_LOG(trace, "Adding user '{}' to cache storage with authorization status: {}", user,
            authorized);
  authorized_users_[user] = authorized;
}

} // namespace Http
} // namespace Envoy
