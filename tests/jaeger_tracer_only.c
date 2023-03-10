#include <opentelemetry-c/exporter_jaeger_trace.h>

#include <stddef.h>

int main(void) {
	opentelemetry_exporter_jaeger_options exporter_options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = "127.0.0.1",
		.server_port = 3456,
	};
	opentelemetry_exporter *exporter = opentelemetry_exporter_jaeger_create(&exporter_options);
	if (exporter == NULL)
		return 1;
	opentelemetry_processor *processor = opentelemetry_processor_batch(exporter, NULL);
	if (processor == NULL)
		return 1;
	opentelemetry_provider *provider = opentelemetry_provider_create(processor, NULL, NULL, 0);
	if (provider == NULL)
		return 1;
	opentelemetry_tracer *tracer;

	tracer = opentelemetry_provider_get_tracer(provider, "mylib", "myversion", "myschema");
	if (tracer == NULL)
		return 1;
	opentelemetry_tracer_destroy(tracer);

	tracer = opentelemetry_provider_get_tracer(provider, "mylib", NULL, NULL);
	if (tracer == NULL)
		return 1;
	opentelemetry_tracer_destroy(tracer);

	opentelemetry_provider_destroy(provider);
	return 0;
}
