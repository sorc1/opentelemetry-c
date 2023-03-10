#include <opentelemetry-c/exporter_jaeger_trace.h>
#include <stddef.h>
#include "utils.h"

int main(void) {
	// comment out to see errors on stdout
	opentelemetry_set_log_level(-1);

	opentelemetry_exporter_jaeger_options exporter_options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = test_jaeger_endpoint(),
		.server_port = test_jaeger_port(),
	};
	opentelemetry_exporter *exporter = opentelemetry_exporter_jaeger_create(&exporter_options);
	if (exporter == NULL)
		return 1;
	opentelemetry_processor *processor = opentelemetry_processor_batch(exporter, NULL);
	if (processor == NULL)
		return 1;
	return test_span(processor);
}
