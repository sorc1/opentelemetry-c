#pragma once

#include <opentelemetry-c/common.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
namespace propagation
{

class OpentelemetryCHeadersInjector: public TextMapCarrier {
public:
	OpentelemetryCHeadersInjector(opentelemetry_header_each header_each, void *header_each_arg):
		header_each_(header_each), header_each_arg_(header_each_arg) {}

	nostd::string_view Get(nostd::string_view key) const noexcept {
		return nostd::string_view("", 0);
	}

	void Set(nostd::string_view key, nostd::string_view value) noexcept {
		if (header_each_ == NULL)
			return;
		int rv = header_each_(key.data(), key.length(), value.data(), value.length(), header_each_arg_);
		if (rv)
			header_each_ = NULL;
	}

private:
	opentelemetry_header_each header_each_;
	void *header_each_arg_;
};

class OpentelemetryCHeadersExtractor: public TextMapCarrier {
public:
	OpentelemetryCHeadersExtractor(opentelemetry_header_value header_value, void *header_value_arg):
		header_value_(header_value), header_value_arg_(header_value_arg) {};

	nostd::string_view Get(nostd::string_view key) const noexcept {
		size_t value_len;
		const char *value = header_value_(key.data(), key.length(), &value_len, header_value_arg_);
		if (value == NULL)
			return nostd::string_view("", 0);
		return nostd::string_view(value, value_len);
	}

	void Set(nostd::string_view key, nostd::string_view value) noexcept {}

private:
	opentelemetry_header_value header_value_;
	void *header_value_arg_;
};

}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
