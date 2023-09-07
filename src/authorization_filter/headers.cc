#include "headers.h"

namespace Envoy::Http {

absl::optional<std::string> getHeader(const RequestOrResponseHeaderMap& headers, std::string key) {
  auto header = headers.get(Http::LowerCaseString(key));
  if (header.empty() || header.size() != 1) {
    return absl::nullopt;
  }
  return std::string(header[0]->value().getStringView().data(), header[0]->value().size());
}

bool hasJsonPayload(RequestHeaderMap& headers) {
  auto content_type = getHeader(headers, "content-type");
  if (!content_type) {
    return false;
  }
  return Http::LowerCaseString(*content_type) == Http::LowerCaseString("application/json");
}

} // namespace Envoy::Http
