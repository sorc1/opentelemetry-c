set(TESTS
	jaeger_exporter_only
	jaeger_processor_only
	jaeger_processor_multi
	sampler_only
	jaeger_provider_only
	jaeger_tracer_only
	jaeger_span
	otlp_http_span
	span_headers
)

add_library(testutils STATIC utils.c)
target_include_directories(testutils PUBLIC ../include)

foreach(test ${TESTS})
	add_executable(${test} "${test}.c")
	target_include_directories(${test} PUBLIC ../include)
	target_link_libraries(${test} testutils opentelemetry-c)
	add_test(NAME ${test} COMMAND ${test})
endforeach()
