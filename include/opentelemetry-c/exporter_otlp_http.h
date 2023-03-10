#ifndef OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_
#define OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_

#include <opentelemetry-c/common.h>

#ifdef  __cplusplus
extern "C" {
#endif

enum OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE {
	OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE_JSON = 0,
	OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE_BINARY,
};

enum OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING {
	OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KHEXID = 0,
	OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KHEX,
	OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KBASE64,
};


typedef struct opentelemetry_exporter_otlp_http_options {
	const char *url;
	enum OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE content_type;
	enum OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING json_bytes_mapping;
	const opentelemetry_http_header *headers;
	size_t nheaders;
	size_t max_concurrent_requests;
	size_t max_requests_per_connection;
	struct timespec timeout;
	bool use_json_name;
	bool console_debug;

	bool timeout_set;
	bool headers_set;
} opentelemetry_exporter_otlp_http_options;

opentelemetry_exporter *opentelemetry_exporter_otlp_http_create(
	const opentelemetry_exporter_otlp_http_options *options);

# ifdef  __cplusplus
}
# endif

#endif /* OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_ */
