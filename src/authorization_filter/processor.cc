#include "processor.h"
#include "headers.h"
#include "absl/types/optional.h"
#include "nlohmann/json.hpp"

namespace Envoy::Http {

AuthorizationFilter::AuthorizationFilter(AuthorizationFilterConfigSharedPtr config,
                                         Upstream::ClusterManager& cluster_manager,
                                         Stats::Counter& cache_hits, Stats::Counter& cache_misses)
    : config_(config),
      cluster_manager_(cluster_manager),
      cache_tls_(config_->cache_tls()),
      cache_hits_(cache_hits),
      cache_misses_(cache_misses) {
  callbacks_ = std::make_unique<AuthorizationCallbacks>([this]() { acceptAuthorization(false); },
                                                        [this]() { denyAuthorization(false); });
}

AuthorizationFilter::~AuthorizationFilter() {}

FilterHeadersStatus AuthorizationFilter::decodeHeaders(RequestHeaderMap& headers, bool) {
  ENVOY_LOG(trace, "AuthorizationFilter decodeHeaders");

  auto user = getHeader(headers, config_->user_header());
  if (user) {
    ENVOY_LOG(trace, "Found user name in request headers: {}", *user);
    auto entry = getAuthorizationEntryFromCache(*user);
    if (!entry) {
      callAuthorizationService(*user);
      return FilterHeadersStatus::StopIteration;
    }
    if (*entry) {
      ENVOY_LOG(trace, "(Cached) user '{}' authenticated", *user);
      return FilterHeadersStatus::Continue;
    }
    denyAuthorization(true);
    return FilterHeadersStatus::StopIteration;
  }

  if (hasJsonPayload(headers)) {
    ENVOY_LOG(debug, "User header not present - checking in payload");
    // TODO: Handle missing payload
    authorization_in_payload_ = true;
    return FilterHeadersStatus::StopIteration;
  }

  ENVOY_LOG(debug, "Missing authentication data - rejecting request");
  denyAuthorization(false);
  return FilterHeadersStatus::StopIteration;
}

bool missingAuthorizationHeader(nlohmann::json& payload, const std::string& userHeader) {
  return (!payload.is_object() || !payload.contains(userHeader)
          || !payload[userHeader].is_string());
}

FilterDataStatus AuthorizationFilter::decodeData(Buffer::Instance& data, bool eos) {
  ENVOY_LOG(trace, "AuthorizationFilter decodeData eos: {}", eos);

  if (!authorization_in_payload_) {
    return FilterDataStatus::Continue;
  }
  if (!eos) {
    return FilterDataStatus::StopIterationAndBuffer;
  }

  nlohmann::json j;
  if (decoder_callbacks_->decodingBuffer() != nullptr) {
    j = nlohmann::json::parse(decoder_callbacks_->decodingBuffer()->toString() + data.toString(),
                              nullptr, false);
  } else {
    j = nlohmann::json::parse(data.toString(), nullptr, false);
  }

  if (j.is_discarded()) {
    ENVOY_LOG(debug, "The payload could not be parsed as JSON payload was invalid");
  }

  if (missingAuthorizationHeader(j, config_->user_header())) {
    denyAuthorization(false);
    return FilterDataStatus::StopIterationNoBuffer;
  }

  user_ = j[std::string(config_->user_header())];

  auto entry = getAuthorizationEntryFromCache(*user_);
  if (!entry) {
    callAuthorizationService(*user_);
    return FilterDataStatus::StopIterationAndBuffer;
  }

  if (*entry) {
    ENVOY_LOG(trace, "(Cached) user '{}' authenticated", *user_);
    return FilterDataStatus::Continue;
  }
  ENVOY_LOG(trace, "(Cached) user '{}' unauthenticated", *user_);
  denyAuthorization(true);
  return FilterDataStatus::StopIterationNoBuffer;
}

FilterTrailersStatus AuthorizationFilter::decodeTrailers(RequestTrailerMap&) {
  // TODO: Handle missing payload
  if (!authorization_in_payload_) {
    return FilterTrailersStatus::Continue;
  }

  nlohmann::json j =
      nlohmann::json::parse(decoder_callbacks_->decodingBuffer()->toString(), nullptr, false);

  if (missingAuthorizationHeader(j, config_->user_header())) {
    denyAuthorization(false);
    return FilterTrailersStatus::StopIteration;
  }

  user_ = j[std::string(config_->user_header())];

  auto entry = getAuthorizationEntryFromCache(*user_);
  if (!entry) {
    callAuthorizationService(*user_);
    return FilterTrailersStatus::StopIteration;
  }

  if (*entry) {
    ENVOY_LOG(trace, "(Cached) user '{}' authenticated", *user_);
    return FilterTrailersStatus::Continue;
  }
  ENVOY_LOG(trace, "(Cached) user '{}' unauthenticated", *user_);
  denyAuthorization(true);
  return FilterTrailersStatus::StopIteration;
}

void AuthorizationFilter::onDestroy() {
  ENVOY_LOG(trace, "AuthorizationFilter onDestroy");
}

void AuthorizationFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& cb) {
  decoder_callbacks_ = &cb;
}

absl::optional<bool> AuthorizationFilter::getAuthorizationEntryFromCache(const std::string& user) {
  if (auto cacheEntry = cache_tls_.isAuthorized(user)) {
    cache_hits_.inc();
    return *cacheEntry;
  }
  cache_misses_.inc();
  return absl::nullopt;
}

void AuthorizationFilter::callAuthorizationService(const std::string& user) {
  ENVOY_LOG(trace, "Starting authorization as: {}", user);

  user_ = user;

  if (auto cacheEntry = cache_tls_.isAuthorized(user)) {
    cache_hits_.inc();
    if (*cacheEntry) {
      acceptAuthorization(true);
    } else {
      denyAuthorization(true);
    }
    return;
  }
  cache_misses_.inc();

  const auto thread_local_cluster = cluster_manager_.getThreadLocalCluster("authorization");
  if (thread_local_cluster == nullptr) {
    ENVOY_LOG(error, "Cannot get authorization cluster");
    denyAuthorization(false);
    return;
  }
  Http::AsyncClient& async_client = thread_local_cluster->httpAsyncClient();

  Http::RequestMessagePtr request = std::make_unique<Http::RequestMessageImpl>();
  request->headers().setPath("/authorize");
  request->headers().setMethod(Http::Headers::get().MethodValues.Get);
  request->headers().addCopy(Http::LowerCaseString("user"), user);

  async_client.send(std::move(request), *callbacks_, Http::AsyncClient::RequestOptions());
}

void AuthorizationFilter::acceptAuthorization(bool cached) {
  if (user_ && !cached) {
    cache_tls_.addUserEntry(*user_, true);
  }
  decoder_callbacks_->continueDecoding();
}

void AuthorizationFilter::denyAuthorization(bool cached) {
  ENVOY_LOG(debug, "Denying unauthorized request");
  if (user_ && !cached) {
    cache_tls_.addUserEntry(*user_, false);
  }
  decoder_callbacks_->sendLocalReply(Http::Code::Unauthorized, "Unauthorized", {}, absl::nullopt,
                                     "failed-to-authorize");
}
} // namespace Envoy::Http
