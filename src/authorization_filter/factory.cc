#include <string>

#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"
#include "envoy/stats/stats.h"

#include "src/authorization_filter/config.pb.h"
#include "src/authorization_filter/config.pb.validate.h"
#include "config.h"
#include "processor.h"
#include "cache.h"

namespace Envoy::Server::Configuration {

class AuthorizationFilterConfigFactory : public NamedHttpFilterConfigFactory {
public:
  Http::FilterFactoryCb createFilterFactoryFromProto(const Protobuf::Message& proto_config,
                                                     const std::string& stat_prefix,
                                                     FactoryContext& context) override {
    auto casted = Envoy::MessageUtil::downcastAndValidate<const authorization::Config&>(
                                proto_config, context.messageValidationVisitor());

    Http::AuthorizationFilterConfigSharedPtr config =
        std::make_shared<Http::AuthorizationFilterConfig>(
            Http::AuthorizationFilterConfig(casted, context));

    auto& cacheHitsStats = context.scope().counterFromString(stat_prefix + "total_cache_hits");
    auto& cacheMissesStats = context.scope().counterFromString(stat_prefix + "total_cache_misses");

    return [config, &context, &cacheHitsStats, &cacheMissesStats](Http::FilterChainFactoryCallbacks& callbacks) -> void {

      auto filter = new Http::AuthorizationFilter(
        config,
        context.clusterManager(),
        cacheHitsStats,
        cacheMissesStats
      );
      callbacks.addStreamDecoderFilter(Http::StreamDecoderFilterSharedPtr{filter});
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return ProtobufTypes::MessagePtr{new authorization::Config()};
  }

  std::string name() const override { return "authorization"; }
};

static Registry::RegisterFactory<AuthorizationFilterConfigFactory, NamedHttpFilterConfigFactory>
    register_;

} // namespace Envoy::Server::Configuration
