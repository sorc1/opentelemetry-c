# OpenTelemetry C

An OpenTelemetry C wrapper for [OpenTelemetry C++ library](https://github.com/open-telemetry/opentelemetry-cpp).

## Requirements

- conan2
- cmake at least 3.17.5
- opentelemetry-cpp 1.9.1

## Build

```
$ git clone https://github.com/sorc1/opentelemetry-c
$ cd opentelemetry-c
$ conan install . --build=missing -of=build
$ cd build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
$ make
```
