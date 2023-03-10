#pragma once

#include <opentelemetry-c/common.h>
#include <opentelemetry/common/key_value_iterable.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

class OpentelemetryCAttributesIterable final : public KeyValueIterable
{
public:
	OpentelemetryCAttributesIterable(const opentelemetry_attribute *attributes, size_t nattributes) noexcept:
		attributes_(attributes), nattributes_(nattributes) {}

	bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
							callback) const noexcept override {

		for (size_t i = 0; i != nattributes_; i++) {
			auto attr = &attributes_[i];
			auto key = nostd::string_view(attr->name.ptr, attr->name.len);
			bool rv = false;
			auto value = &attr->value;
			switch (value->type) {
			case OPENTELEMETRY_TYPE_BOOL: rv = callback(key, value->b); break;
			case OPENTELEMETRY_TYPE_INT32: rv = callback(key, value->i32); break;
			case OPENTELEMETRY_TYPE_INT64: rv = callback(key, value->i64); break;
			case OPENTELEMETRY_TYPE_UINT32: rv = callback(key, value->u32); break;
			case OPENTELEMETRY_TYPE_DOUBLE: rv = callback(key, value->d); break;
			case OPENTELEMETRY_TYPE_STRING: rv = callback(key, nostd::string_view(value->s.ptr, value->s.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_BOOL: rv = callback(key, nostd::span<const bool>(value->ab.values, value->ab.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_INT32: rv = callback(key, nostd::span<const int32_t>(value->ai32.values, value->ai32.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_INT64: rv = callback(key, nostd::span<const int64_t>(value->ai64.values, value->ai64.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_UINT32: rv = callback(key, nostd::span<const uint32_t>(value->au32.values, value->au32.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_DOUBLE: rv = callback(key, nostd::span<const double>(value->ad.values, value->ad.len)); break;
			case OPENTELEMETRY_TYPE_ARRAY_STRING:
				// HACK! Our opentelemetry_string is binary compatible with nostd::string_view,
				// so we just use reinterpret_cast to not create an intermediate array.
				rv = callback(key, nostd::span<const nostd::string_view>(reinterpret_cast<const nostd::string_view*>(value->as.values), value->as.len));
				break;
			}
			if (!rv)
				return false;
		}
		return true;
	}

	size_t size() const noexcept override { return nattributes_; }
private:
	const opentelemetry_attribute *attributes_;
	size_t nattributes_;
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
