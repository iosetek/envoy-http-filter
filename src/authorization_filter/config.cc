#include "config.h"

namespace Envoy::Http {

Envoy::ThreadLocal::SlotPtr createTLSObject(Envoy::Server::Configuration::FactoryContext& context) {
  auto tls = context.threadLocal().allocateSlot();
  tls->set([](Event::Dispatcher&) -> ThreadLocal::ThreadLocalObjectSharedPtr {
    return std::make_shared<Http::AuthorizationCacheTLS>();
  });
  return tls;
}

AuthorizationFilterConfig::AuthorizationFilterConfig(const authorization::Config& proto_config, Envoy::Server::Configuration::FactoryContext& context)
  : user_header_(proto_config.user_header()),
    tls_slot_(createTLSObject(context)) {
    if (Http::LowerCaseString(user_header_) == Http::LowerCaseString("host")) {
      throw EnvoyException("cannot use host header for authentication");
    }
  }
}
