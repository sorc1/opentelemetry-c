#include <opentelemetry-c/exporter_otlp_http.h>
#include <stddef.h>
#include "utils.h"

int main(void) {
	opentelemetry_exporter_otlp_http_options exporter_options = {
		// Use OTEL_EXPORTER_OTLP_ENDPOINT environment variable to really send
		// traces. For example:
		// OTEL_EXPORTER_OTLP_ENDPOINT='http://172.19.0.1:4318/v1/traces'
		//.url = "http://172.19.0.1:4318/v1/traces",
		//.content_type = OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE_BINARY,
		//.json_bytes_mapping = OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KBASE64,
		//.console_debug = true,
		//.max_concurrent_requests = 1,
		//.max_requests_per_connection = 1,
	};
	opentelemetry_exporter *exporter = opentelemetry_exporter_otlp_http_create(&exporter_options);
	if (exporter == NULL)
		return 1;
	opentelemetry_processor *processor = opentelemetry_processor_simple(exporter);
	if (processor == NULL)
		return 1;
	return test_span(processor);
}
