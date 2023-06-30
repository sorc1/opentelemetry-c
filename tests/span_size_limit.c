#include <opentelemetry-c/exporter_jaeger_trace.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main(void) {
	opentelemetry_exporter_jaeger_options exporter_options = {
		.format = OPENTELEMETRY_C_EXPORTER_JAEGER_FORMAT_THRIFT_UDP_COMPACT,
		.endpoint = test_jaeger_endpoint(),
		.server_port = test_jaeger_port(),
	};
	opentelemetry_exporter *exporter = opentelemetry_exporter_jaeger_create(&exporter_options);
	if (exporter == NULL)
		return 1;
	opentelemetry_processor *processor = opentelemetry_processor_batch(exporter, NULL);
	if (processor == NULL)
		return 1;
	opentelemetry_attribute provider_attrs[] = {
		OPENTELEMETRY_ATTRIBUTE_CSTR("service.name", "myservice"),
	};
	opentelemetry_provider *provider = opentelemetry_provider_create(
		processor, NULL, provider_attrs, sizeof(provider_attrs) / sizeof(provider_attrs[0]));
	if (provider == NULL)
		return 1;
	opentelemetry_tracer *tracer = opentelemetry_provider_get_tracer(provider, "mylib", "myversion", "myschema");
	if (tracer == NULL)
		return 1;
	opentelemetry_tracer_limit_span_size(tracer, true);
	opentelemetry_span *span = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("root"), NULL);
	if (span == NULL)
		return 1;

	opentelemetry_span *subspan = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("long_strings"), span);

	struct {
		opentelemetry_string name;
		size_t size;
	} string_infos[] = {
		{.name = OPENTELEMETRY_CSTR("str300"), .size = 300},
		{.name = OPENTELEMETRY_CSTR("str40000_1"), .size = 40000},
		{.name = OPENTELEMETRY_CSTR("str40000_2"), .size = 40000},
		{.name = OPENTELEMETRY_CSTR("str180000"), .size = 180000},
		{.name = OPENTELEMETRY_CSTR("str50"), .size = 50},
		{.name = OPENTELEMETRY_CSTR("str60"), .size = 60},
		{.name = OPENTELEMETRY_CSTR("str70"), .size = 70},
		{.name = OPENTELEMETRY_CSTR("str70000"), .size = 70000},
		{.name = OPENTELEMETRY_CSTR("str71"), .size = 71},
		{.name = OPENTELEMETRY_CSTR("str72"), .size = 72},
	};
	opentelemetry_attribute string_attrs[sizeof(string_infos) / sizeof(string_infos[0])];

	for (size_t i = 0; i != sizeof(string_infos) / sizeof(string_infos[0]); i++) {
		string_attrs[i].name = string_infos[i].name;
		string_attrs[i].value.type = OPENTELEMETRY_TYPE_STRING;
		string_attrs[i].value.s.ptr = create_test_string(string_infos[i].size);
		string_attrs[i].value.s.len = string_infos[i].size;
	}

	for (size_t i = 0; i != sizeof(string_attrs) / sizeof(string_attrs[0]); i++)
		opentelemetry_span_add_event(subspan, &(opentelemetry_string)OPENTELEMETRY_CSTR("single_str_event"), NULL,
			&string_attrs[i], 1);

	opentelemetry_span_add_event(subspan, &(opentelemetry_string)OPENTELEMETRY_CSTR("multi_str_event"), NULL,
		string_attrs, sizeof(string_attrs) / sizeof(string_attrs[0]));

	opentelemetry_string string_array[sizeof(string_infos) / sizeof(string_infos[0])];
	opentelemetry_attribute string_array_attrs[3] = {
		{
			.name = OPENTELEMETRY_CSTR("array_prefix"),
			.value = {.type = OPENTELEMETRY_TYPE_STRING, .s = OPENTELEMETRY_CSTR("array_prefix_value")}
		}, {
			.name = OPENTELEMETRY_CSTR("array"),
			.value = {.type = OPENTELEMETRY_TYPE_ARRAY_STRING, .as = {
				.len = sizeof(string_array) / sizeof(string_array[0]),
				.values = string_array,
			}},
		}, {
			.name = OPENTELEMETRY_CSTR("array_suffix"),
			.value = {.type = OPENTELEMETRY_TYPE_STRING, .s = OPENTELEMETRY_CSTR("array_suffix_value")}
		}
	};

	for (size_t i = 0; i != sizeof(string_infos) / sizeof(string_infos[0]); i++)
		string_array[i] = string_attrs[i].value.s;

	opentelemetry_span_add_event(subspan, &(opentelemetry_string)OPENTELEMETRY_CSTR("array_str_event"), NULL,
		string_array_attrs, sizeof(string_array_attrs) / sizeof(string_array_attrs[0]));

	for (size_t i = 0; i != sizeof(string_attrs) / sizeof(string_attrs[0]); i++)
		free((void *)string_attrs[i].value.s.ptr);
	opentelemetry_span_finish(subspan);

	subspan = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("long_arrays"), span);

	struct {
		enum opentelemetry_value_type type;
		opentelemetry_string name;
		size_t len;
	} array_infos[] = {
		{.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .name = OPENTELEMETRY_CSTR("bool50"), .len = 50},
		{.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .name = OPENTELEMETRY_CSTR("bool10000"), .len = 10000},
		{.type = OPENTELEMETRY_TYPE_ARRAY_INT32, .name = OPENTELEMETRY_CSTR("int32_10000"), .len = 10000},
		{.type = OPENTELEMETRY_TYPE_ARRAY_UINT32, .name = OPENTELEMETRY_CSTR("uint32_10000"), .len = 10000},
		{.type = OPENTELEMETRY_TYPE_ARRAY_INT64, .name = OPENTELEMETRY_CSTR ("int64_10000"), .len = 10000},
		{.type = OPENTELEMETRY_TYPE_ARRAY_DOUBLE, .name = OPENTELEMETRY_CSTR("double_10000"), .len = 10000},
		{.type = OPENTELEMETRY_TYPE_ARRAY_DOUBLE, .name = OPENTELEMETRY_CSTR("double_5"), .len = 5},
		{.type = OPENTELEMETRY_TYPE_ARRAY_DOUBLE, .name = OPENTELEMETRY_CSTR("double_6"), .len = 6},
		{.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .name = OPENTELEMETRY_CSTR("bool20"), .len = 20},
		{.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .name = OPENTELEMETRY_CSTR("bool21"), .len = 21},
	};

	opentelemetry_attribute array_attrs[sizeof(array_infos) / sizeof(array_infos[0])];

	for (size_t i = 0; i != sizeof(array_infos) / sizeof(array_infos[0]); i++) {
		array_attrs[i].name = array_infos[i].name;
		array_attrs[i].value.type = array_infos[i].type;
		// a hack: len is at the same place for all the array types
		array_attrs[i].value.ab.len = array_infos[i].len;
		switch (array_infos[i].type) {
		case OPENTELEMETRY_TYPE_ARRAY_BOOL:
			array_attrs[i].value.ab.values = create_test_ab(array_infos[i].len); break;
		case OPENTELEMETRY_TYPE_ARRAY_INT32:
			array_attrs[i].value.ai32.values = create_test_ai32(array_infos[i].len); break;
		case OPENTELEMETRY_TYPE_ARRAY_UINT32:
			array_attrs[i].value.au32.values = (uint32_t *)create_test_ai32(array_infos[i].len); break;
		case OPENTELEMETRY_TYPE_ARRAY_INT64:
			array_attrs[i].value.ai64.values = create_test_ai64(array_infos[i].len); break;
		case OPENTELEMETRY_TYPE_ARRAY_DOUBLE:
			array_attrs[i].value.ad.values = create_test_ad(array_infos[i].len); break;
		default: return 1;
		}
	}

	for (size_t i = 0; i != sizeof(array_attrs) / sizeof(array_attrs[0]); i++)
		opentelemetry_span_add_event(subspan, &(opentelemetry_string)OPENTELEMETRY_CSTR("single_array_event"), NULL,
			&array_attrs[i], 1);

	opentelemetry_span_add_event(subspan, &(opentelemetry_string)OPENTELEMETRY_CSTR("multi_array_event"), NULL,
		array_attrs, sizeof(array_attrs) / sizeof(array_attrs[0]));

	for (size_t i = 0; i != sizeof(array_attrs) / sizeof(array_attrs[0]); i++)
		free((void *)array_attrs[i].value.ab.values); // a hack, see above

	opentelemetry_span_finish(subspan);

	opentelemetry_span_finish(span);

	opentelemetry_tracer_destroy(tracer);
	opentelemetry_provider_destroy(provider);

	return 0;
}
