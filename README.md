# Envoy filter example

This project demonstrates writing a custom HTTP filter.

For the demonstration purposes, the filter imitates an authorization filter with the following features:
* calling HTTP Service
* preventing HTTP payload from sending the data upstream before HTTP Service confirms authorization
* caching authorization results
* adding statistics for cache hits/misses

The filter was built on top of [Envoy Filter Example](https://github.com/envoyproxy/envoy-filter-example).

## Building

To build the Envoy static binary:

1. `git submodule update --init`
2. `bazel build //:envoy`

## Running

To run the built filter run

```
bazel-bin/envoy -c config/authorization_filter.yaml --component-log-level filter:trace --concurrency 2
```

