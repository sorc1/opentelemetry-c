#include <opentelemetry-c/exporter_otlp_grpc.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>
#include "otlp.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter {
namespace otlp {

extern "C" opentelemetry_exporter *opentelemetry_exporter_otlp_grpc_create(
	const opentelemetry_exporter_otlp_grpc_options *opts) {

	try {
		OtlpGrpcExporterOptions options = {
			opts->endpoint ? std::string(opts->endpoint) : GetOtlpDefaultGrpcEndpoint(),
			opts->use_ssl_credentials_set ? opts->use_ssl_credentials : GetOtlpDefaultIsSslEnable(),
			opts->ssl_credentials_cacert_path ? std::string(opts->ssl_credentials_cacert_path) : GetOtlpDefaultSslCertificatePath(),
			opts->ssl_credentials_cacert_as_string ? std::string(opts->ssl_credentials_cacert_as_string) : GetOtlpDefaultSslCertificateString(),
			opts->timeout_set ? std::chrono::nanoseconds((uint64_t)opts->timeout.tv_sec * 1000000000 + opts->timeout.tv_nsec) : GetOtlpDefaultTimeout(),
			opts->headers_set ? OpentelemetryCOtlpHeaders(opts->headers, opts->nheaders) : GetOtlpDefaultHeaders(),
			opts->user_agent ? std::string(opts->user_agent) : GetOtlpDefaultUserAgent(),
		};
		auto exporter = new OtlpGrpcExporter(options);
		return reinterpret_cast<opentelemetry_exporter *>(exporter);
	} catch (...) {
		return NULL;
	}
}

}}
OPENTELEMETRY_END_NAMESPACE
