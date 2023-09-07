#include "authorization_callbacks.h"
#include "headers.h"

namespace Envoy::Http {

AuthorizationCallbacks::AuthorizationCallbacks(std::function<void()> authorize,
                                               std::function<void()> unauthorize)
    : authorize_(authorize), unauthorize_(unauthorize) {}

AuthorizationCallbacks::~AuthorizationCallbacks() {}

void AuthorizationCallbacks::onSuccess(const Http::AsyncClient::Request&,
                                       Http::ResponseMessagePtr&& response) {
  if (response == nullptr) {
    ENVOY_LOG(error, "Could not obtain response object from authorization call");
    unauthorize_();
    return;
  }

  auto response_code = getHeader(response->headers(), ":status");
  if (!response_code) {
    ENVOY_LOG(error, "Could not obtain headers from authorization service");
    unauthorize_();
    return;
  }
  if (*response_code != "200") {
    ENVOY_LOG(debug, "Unauthorized user");
    unauthorize_();
    return;
  }
  ENVOY_LOG(trace, "Authorization succeeded");
  authorize_();
}

void AuthorizationCallbacks::onFailure(const Http::AsyncClient::Request&,
                                       Http::AsyncClient::FailureReason reason) {
  ENVOY_LOG(error, "Failed to call authorization service: {}", int(reason));
  unauthorize_();
}

void AuthorizationCallbacks::onBeforeFinalizeUpstreamSpan(Envoy::Tracing::Span&,
                                                          const Http::ResponseHeaderMap*) {}
} // namespace Envoy::Http
