#include <opentelemetry-c/exporter_jaeger_trace.h>

#include <stddef.h>

int main(void) {
	opentelemetry_exporter_jaeger_options options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = "127.0.0.1",
		.server_port = 3456,
	};
	opentelemetry_exporter *exporter =
		opentelemetry_exporter_jaeger_create(&options);
	if (exporter == NULL)
		return 1;
	opentelemetry_exporter_destroy(exporter);
	return 0;
}
