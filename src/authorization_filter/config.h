#pragma once

#include <string>

#include "cache.h"

#include "src/authorization_filter/config.pb.h"
#include "envoy/server/factory_context.h"

namespace Envoy::Http {

class AuthorizationFilterConfig {
public:
  AuthorizationFilterConfig(const authorization::Config& proto_config,
                            Envoy::Server::Configuration::FactoryContext& context);

  const std::string& user_header() const { return user_header_; }
  Http::AuthorizationCacheTLS& cache_tls() const {
    return tls_slot_->getTyped<Http::AuthorizationCacheTLS>();
  }

private:
  const std::string user_header_;
  Envoy::ThreadLocal::SlotPtr tls_slot_;
};

using AuthorizationFilterConfigSharedPtr = std::shared_ptr<AuthorizationFilterConfig>;

} // namespace Envoy::Http
