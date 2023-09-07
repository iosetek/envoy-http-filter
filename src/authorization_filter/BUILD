load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)
load("@envoy_api//bazel:api_build_system.bzl", "api_proto_package")

package(default_visibility = ["//visibility:public"])

api_proto_package()

envoy_cc_library(
    name = "authorization_filter",
    srcs = ["processor.cc"],
    hdrs = ["processor.h"],
    repository = "@envoy",
    deps = [
        ":cache",
        ":headers",
        ":authorization_callbacks",
        ":authorization_filter_config",
        "@nlohmann-json//:nlohmann-json",
        "@envoy//envoy/http:async_client_interface",
        "@envoy//source/common/network:address_lib",
        "@envoy//envoy/server:filter_config_interface",
        "@envoy//envoy/upstream:cluster_manager_interface",
        "@envoy//source/common/http:message_lib",
        "@envoy//source/common/http:headers_lib",
    ],
)

envoy_cc_library(
    name = "authorization_filter_config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    repository = "@envoy",
    deps = [
        ":cache",
        ":pkg_cc_proto",
        "@envoy//envoy/server:factory_context_interface",
        "@envoy//envoy/http:header_map_interface",
    ],
)

envoy_cc_library(
    name = "authorization_filter_factory",
    srcs = ["factory.cc"],
    repository = "@envoy",
    deps = [
        ":authorization_filter",
        ":authorization_filter_config",
        ":cache",
        "@envoy//envoy/server:filter_config_interface",
    ],
)

envoy_cc_library(
    name = "authorization_callbacks",
    hdrs = ["authorization_callbacks.h"],
    srcs = ["authorization_callbacks.cc"],
    repository = "@envoy",
    deps = [
        ":headers",
        "@envoy//envoy/http:async_client_interface",
    ],
)

envoy_cc_library(
    name = "headers",
    hdrs = ["headers.h"],
    srcs = ["headers.cc"],
    repository = "@envoy",
    deps = [
        "@envoy//envoy/http:header_map_interface",
        "@envoy//envoy/server:filter_config_interface",
    ],
)

envoy_cc_library(
    name = "cache",
    hdrs = ["cache.h"],
    srcs = ["cache.cc"],
    repository = "@envoy",
    deps = [
        "@envoy//envoy/thread_local:thread_local_interface",
    ],
)
