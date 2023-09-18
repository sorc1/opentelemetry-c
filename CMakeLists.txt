cmake_minimum_required(VERSION 3.0.0)
project(opentelemetry-c VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

find_package(opentelemetry-cpp REQUIRED)

include(GNUInstallDirs)
#include(CMakeDependentOption)

add_library(opentelemetry-c SHARED
	src/common.cpp
	src/exporter_jaeger_trace.cpp
	#src/exporter_otlp_grpc.cpp
	src/exporter_otlp_http.cpp
)
target_include_directories(opentelemetry-c PUBLIC include)
target_compile_definitions(opentelemetry-c PRIVATE -DENABLE_ASYNC_EXPORT)
set_target_properties(opentelemetry-c PROPERTIES POSITION_INDEPENDET_CODE ON VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
#set_target_properties(opentelemetry-c PROPERTIES PUBLIC_HEADER "include/opentelemetry-c/*.h")
target_link_libraries(opentelemetry-c PUBLIC
	opentelemetry_common
	opentelemetry_trace
	opentelemetry_exporter_jaeger_trace
	#opentelemetry_exporter_otlp_grpc
	opentelemetry_exporter_otlp_http
)

include(GNUInstallDirs)

install(
	TARGETS opentelemetry-c
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY
	include
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_subdirectory(tests)
