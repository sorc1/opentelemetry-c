#ifndef OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_
#define OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_

#include <opentelemetry-c/common.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct opentelemetry_exporter_otlp_grpc_options {
	const char *endpoint;
	const char *ssl_credentials_cacert_path;
	const char *ssl_credentials_cacert_as_string;
	const char *user_agent;
	struct timespec timeout;
	const opentelemetry_http_header *headers;
	size_t nheaders;
	bool use_ssl_credentials;

	bool use_ssl_credentials_set;
	bool timeout_set;
	bool headers_set;
} opentelemetry_exporter_otlp_grpc_options;

opentelemetry_exporter *opentelemetry_exporter_otlp_grpc_create(
	const opentelemetry_exporter_otlp_grpc_options *options);

# ifdef  __cplusplus
}
# endif

#endif /* OPENTELEMETRY_C_EXPORTER_OLTP_GRPC_H_ */
