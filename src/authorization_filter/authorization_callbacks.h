#pragma once

#include <string>
#include <functional>

#include "envoy/http/async_client.h"

namespace Envoy {
namespace Http {

class AuthorizationCallbacks : public Logger::Loggable<Logger::Id::filter>, public Http::AsyncClient::Callbacks {
public:
  AuthorizationCallbacks(
    std::function<void()> authorize,
    std::function<void()> unauthorize
  );
  ~AuthorizationCallbacks();

  void onSuccess(const Http::AsyncClient::Request& request, Http::ResponseMessagePtr&& response) override;
  void onFailure(const Http::AsyncClient::Request& request, Http::AsyncClient::FailureReason reason) override;
  void onBeforeFinalizeUpstreamSpan(Envoy::Tracing::Span& span,
                                          const Http::ResponseHeaderMap* response_headers) override;

private:
  std::function<void()> authorize_;
  std::function<void()> unauthorize_;
};

} // namespace Http
} // namespace Envoy
