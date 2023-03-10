#pragma once

#include <opentelemetry-c/common.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter {
namespace otlp {

static OtlpHeaders OpentelemetryCOtlpHeaders(const opentelemetry_http_header *headers, size_t nheaders) {

	OtlpHeaders result;

	for (size_t i = 0; i != nheaders; i++) {
		auto header = &headers[i];
		result.emplace(std::make_pair(
			std::string(header->name.ptr, header->name.len),
			std::string(header->value.ptr, header->value.len)));
	}
	return result;
}

}}
OPENTELEMETRY_END_NAMESPACE
