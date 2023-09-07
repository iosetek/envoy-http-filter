#pragma once

#include <string>

#include "envoy/http/filter.h"

namespace Envoy::Http {

absl::optional<std::string> getHeader(const RequestOrResponseHeaderMap& headers, std::string key);

bool hasJsonPayload(RequestHeaderMap& headers);

} // namespace Envoy::Http
