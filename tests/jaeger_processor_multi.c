#include <opentelemetry-c/exporter_jaeger_trace.h>

#include <stddef.h>

int main(void) {
	opentelemetry_exporter_jaeger_options options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = "127.0.0.1",
		.server_port = 3456,
	};

	opentelemetry_processor *processors[10];
	size_t nprocessors = sizeof(processors) / sizeof(processors[0]);

	size_t i = 0;
	for (; i != nprocessors / 2; i++) {
		opentelemetry_exporter *exporter = opentelemetry_exporter_jaeger_create(&options);
		if (exporter == NULL)
			return 1;
		opentelemetry_processor *processor = opentelemetry_processor_simple(exporter);
		if (processor == NULL)
			return 1;
		processors[i] = processor;
	}
	for (; i != nprocessors; i++) {
		opentelemetry_exporter *exporter = opentelemetry_exporter_jaeger_create(&options);
		if (exporter == NULL)
			return 1;
		opentelemetry_processor *processor = opentelemetry_processor_batch(exporter, NULL);
		if (processor == NULL)
			return 1;
		processors[i] = processor;
	}
	opentelemetry_processor *processor = opentelemetry_processor_multi(processors, nprocessors);
	opentelemetry_processor_destroy(processor);
	return 0;
}
