#include "config.h"

#include "envoy/common/exception.h"
#include "envoy/http/header_map.h"

namespace Envoy::Http {

Envoy::ThreadLocal::SlotPtr createTLSObject(Envoy::Server::Configuration::FactoryContext& context) {
  auto tls = context.threadLocal().allocateSlot();
  tls->set([](Event::Dispatcher&) -> ThreadLocal::ThreadLocalObjectSharedPtr {
    return std::make_shared<Http::AuthorizationCacheTLS>();
  });
  return tls;
}

AuthorizationFilterConfig::AuthorizationFilterConfig(
    const authorization::Config& proto_config,
    Envoy::Server::Configuration::FactoryContext& context)
    : user_header_(proto_config.user_header()), tls_slot_(createTLSObject(context)) {
  if (LowerCaseString(user_header_) == LowerCaseString("host")) {
    throw EnvoyException("cannot use host header for authentication");
  }
}
} // namespace Envoy::Http
