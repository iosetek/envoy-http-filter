workspace(name = "envoy_filter_example")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

local_repository(
    name = "envoy",
    path = "envoy",
)

load("@envoy//bazel:api_binding.bzl", "envoy_api_binding")

envoy_api_binding()

load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")

envoy_api_dependencies()

load("@envoy//bazel:repositories.bzl", "envoy_dependencies")

envoy_dependencies()

load("@envoy//bazel:repositories_extra.bzl", "envoy_dependencies_extra")

envoy_dependencies_extra()

load("@envoy//bazel:python_dependencies.bzl", "envoy_python_dependencies")

envoy_python_dependencies()

load("@envoy//bazel:dependency_imports.bzl", "envoy_dependency_imports")

envoy_dependency_imports()

new_git_repository(
    name = "nlohmann-json",
    build_file = "BUILD.nlohmann-json",
    remote = "https://github.com/nlohmann/json",
    commit = "bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d",
)
