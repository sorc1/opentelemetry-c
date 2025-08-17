cmake_minimum_required(VERSION 3.17.5)
project(opentelemetry-c VERSION 1.1.2 LANGUAGES C CXX)

include(CTest)
include(GNUInstallDirs)
include(CMakeFindDependencyMacro)

find_package(opentelemetry-cpp REQUIRED)

add_library(opentelemetry-c SHARED
	src/common.cpp
	src/exporter_jaeger_trace.cpp
	src/exporter_otlp_http.cpp
)
target_include_directories(opentelemetry-c PUBLIC include)
target_compile_definitions(opentelemetry-c PRIVATE -DENABLE_ASYNC_EXPORT)
set_target_properties(opentelemetry-c PROPERTIES POSITION_INDEPENDET_CODE ON VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
target_link_libraries(opentelemetry-c PUBLIC
	opentelemetry-cpp::opentelemetry-cpp
	opentelemetry-cpp::opentelemetry_common
	opentelemetry-cpp::opentelemetry_trace
	opentelemetry-cpp::opentelemetry_exporter_jaeger_trace
	opentelemetry-cpp::opentelemetry_exporter_otlp_http
)

install(
	TARGETS opentelemetry-c
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY
	include/opentelemetry-c
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_subdirectory(tests)
