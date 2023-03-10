#include <opentelemetry-c/exporter_jaeger_trace.h>

#include <stddef.h>

int main(void) {
	opentelemetry_exporter_jaeger_options options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = "127.0.0.1",
		.server_port = 3456,
	};
	opentelemetry_exporter *exporter1 = opentelemetry_exporter_jaeger_create(&options);
	if (exporter1 == NULL)
		return 1;
	opentelemetry_processor *processor_simple = opentelemetry_processor_simple(exporter1);
	if (processor_simple == NULL)
		return 1;

	opentelemetry_exporter *exporter2 = opentelemetry_exporter_jaeger_create(&options);
	if (exporter2 == NULL)
		return 1;
	opentelemetry_processor_batch_options batch_options = {
		.max_queue_size = 512,
		.schedule_delay_millis = 5000,
		.max_export_batch_size = 512,
	};
	opentelemetry_processor *processor_batch = opentelemetry_processor_batch(exporter2, &batch_options);
	if (processor_batch == NULL)
		return 1;
	opentelemetry_processor_destroy(processor_simple);
	opentelemetry_processor_destroy(processor_batch);
	return 0;
}
