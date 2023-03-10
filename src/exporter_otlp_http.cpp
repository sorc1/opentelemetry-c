#include <opentelemetry-c/exporter_otlp_http.h>

#include <opentelemetry/exporters/otlp/otlp_http_exporter.h>
#include "otlp.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter {
namespace otlp {

extern "C" opentelemetry_exporter *opentelemetry_exporter_otlp_http_create(
	const opentelemetry_exporter_otlp_http_options *opts) {

	try {
		HttpRequestContentType content_type = HttpRequestContentType::kJson;

		switch (opts->content_type) {
		case OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE_JSON:
			content_type = HttpRequestContentType::kJson; break;
		case OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_CONTENT_TYPE_BINARY:
			content_type = HttpRequestContentType::kBinary; break;
		}

		JsonBytesMappingKind json_bytes_mapping = JsonBytesMappingKind::kHexId;

		switch (opts->json_bytes_mapping) {
		case OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KHEXID:
			json_bytes_mapping = JsonBytesMappingKind::kHexId; break;
		case OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KHEX:
			json_bytes_mapping = JsonBytesMappingKind::kHex; break;
		case OPENTELEMETRY_C_EXPORTER_OTLP_HTTP_JSON_BMAPPING_KBASE64:
			json_bytes_mapping = JsonBytesMappingKind::kBase64; break;
		}
		OtlpHttpExporterOptions options = {
			opts->url ? std::string(opts->url) : GetOtlpDefaultHttpEndpoint(),
			content_type,
			json_bytes_mapping,
			opts->use_json_name,
			opts->console_debug,
			opts->timeout_set ? std::chrono::nanoseconds((uint64_t)opts->timeout.tv_sec * 1000000000 + opts->timeout.tv_nsec) : GetOtlpDefaultTimeout(),
			opts->headers_set ? OpentelemetryCOtlpHeaders(opts->headers, opts->nheaders) : GetOtlpDefaultHeaders(),
			opts->max_concurrent_requests ? opts->max_concurrent_requests : 64,
			opts->max_requests_per_connection ? opts->max_requests_per_connection : 8,
		};
		auto exporter = new OtlpHttpExporter(options);
		return reinterpret_cast<opentelemetry_exporter *>(exporter);
	} catch (...) {
		return NULL;
	}
}

}}
OPENTELEMETRY_END_NAMESPACE
