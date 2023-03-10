#include <opentelemetry-c/exporter_jaeger_trace.h>

#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter {
namespace jaeger {

extern "C" opentelemetry_exporter *opentelemetry_exporter_jaeger_create(
	const opentelemetry_exporter_jaeger_options *options) {

	try {
		TransportFormat format;

		switch (options->format) {
		case OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP:
			format = TransportFormat::kThriftUdp; break;
		case OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT:
			format = TransportFormat::kThriftUdpCompact; break;
		case OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_HTTP:
			format = TransportFormat::kThriftHttp; break;
		case OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_PROTOBUF_GRPC:
			format = TransportFormat::kProtobufGrpc; break;
		default:
			return NULL;
		}
		JaegerExporterOptions opts = {
			.transport_format = format,
			.endpoint = std::string(options->endpoint),
			.server_port = options->server_port,
		};
		auto exporter = new JaegerExporter(opts);
		return reinterpret_cast<opentelemetry_exporter *>(exporter);
	} catch (...) {
		return NULL;
	}
}

}}
OPENTELEMETRY_END_NAMESPACE
