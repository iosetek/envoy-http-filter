#pragma once

#include <string>

#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"
#include "envoy/http/async_client.h"
#include "envoy/http/message.h"
#include "source/common/http/message_impl.h"

#include "config.h"
#include "cache.h"
#include "authorization_callbacks.h"
#include "src/authorization_filter/config.pb.h"

namespace Envoy {
namespace Http {

// TODO: Add access logs
// TODO: Add tests
// TODO: Add redirect for unauthorized call

class AuthorizationFilter : public StreamDecoderFilter, Logger::Loggable<Logger::Id::filter> {
public:
  AuthorizationFilter(
    AuthorizationFilterConfigSharedPtr config,
    Upstream::ClusterManager& cluster_manager,
    Stats::Counter& cache_hits,
    Stats::Counter& cache_misses);
  ~AuthorizationFilter();

  FilterHeadersStatus decodeHeaders(RequestHeaderMap&, bool) override;
  FilterDataStatus decodeData(Buffer::Instance&, bool) override;
  FilterTrailersStatus decodeTrailers(RequestTrailerMap&) override;
  void onDestroy() override;

  void setDecoderFilterCallbacks(StreamDecoderFilterCallbacks&) override;

private:
  void callAuthorizationService(const std::string& user);
  absl::optional<bool> getAuthorizationEntryFromCache(const std::string& user);

  void acceptAuthorization(bool cached);
  void denyAuthorization(bool cached);

  bool authorization_in_payload_ = false;
  absl::optional<std::string> user_;

  const AuthorizationFilterConfigSharedPtr config_;
  Upstream::ClusterManager& cluster_manager_;
  Http::AuthorizationCacheTLS& cache_tls_;
  Stats::Counter& cache_hits_;
  Stats::Counter& cache_misses_;
  std::unique_ptr<AuthorizationCallbacks> callbacks_;
  StreamDecoderFilterCallbacks* decoder_callbacks_;
};

} // namespace Http
} // namespace Envoy
