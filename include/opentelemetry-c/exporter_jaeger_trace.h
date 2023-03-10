#ifndef OPENTELEMETRY_C_EXPORTER_JAEGER_TRACE_H_
#define OPENTELEMETRY_C_EXPORTER_JAEGER_TRACE_H_

#include <opentelemetry-c/common.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

enum OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT {
	OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP = 0,
	OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
	OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_HTTP,
	OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_PROTOBUF_GRPC,
};

typedef struct opentelemetry_exporter_jaeger_options {
	enum OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT format;
	const char *endpoint;
	uint16_t server_port;
	// Only applicable when using kThriftHttp transport.
	//ext::http::client::Headers headers;
} opentelemetry_exporter_jaeger_options;

opentelemetry_exporter *opentelemetry_exporter_jaeger_create(
	const opentelemetry_exporter_jaeger_options *options);

# ifdef  __cplusplus
}
# endif

#endif /* OPENTELEMETRY_C_EXPORTER_JAEGER_TRACE_H_*/
