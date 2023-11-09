#include "utils.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

const char *test_jaeger_endpoint(void) {
	char *name = getenv("JAEGER_ENDPOINT");
	if (name == NULL)
		name = "127.0.0.1";
	return name;
}

uint16_t test_jaeger_port(void) {
	char *value = getenv("JAEGER_PORT");
	if (value == NULL)
		return 6831;

	errno = 0;
	long int v = strtol(value, NULL, 10);
	if (errno)
		return 6831;
	return v;
}

char *create_test_string(size_t size) {
	static const char string_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char *buf = malloc(size);

	if (buf == NULL)
		return NULL;

	for (size_t i = 0, j = 0; i != size; i++, j = (j + 1) % (sizeof(string_chars) - 1))
		buf[i] = string_chars[j];
	return buf;
}

bool *create_test_ab(size_t len) {
	bool *buf = malloc(len);

	if (buf == NULL)
		return NULL;
	bool cur_value = false;
	for (size_t i = 0; i != len; i++, cur_value = !cur_value)
		buf[i] = cur_value;
	return buf;
}

int32_t *create_test_ai32(size_t len) {
	int32_t *buf = malloc(len * sizeof(*buf));

	if (buf == NULL)
		return NULL;
	for (size_t i = 0; i != len; i++)
		buf[i] = i;
	return buf;
}

int64_t *create_test_ai64(size_t len) {
	int64_t *buf = malloc(len * sizeof(*buf));

	if (buf == NULL)
		return NULL;
	for (size_t i = 0; i != len; i++)
		buf[i] = i;
	return buf;
}

double *create_test_ad(size_t len) {
	double *buf = malloc(len * sizeof(*buf));

	if (buf == NULL)
		return NULL;
	for (size_t i = 0; i != len; i++)
		buf[i] = i;
	return buf;
}

int test_span(opentelemetry_processor *processor) {
	double dvalues[] = {1.1, 2.2, 3.3};
	opentelemetry_string svalues[] = {OPENTELEMETRY_CSTR("value1"), OPENTELEMETRY_CSTR("value2")};
	opentelemetry_attribute provider_attrs[] = {
		OPENTELEMETRY_ATTRIBUTE_CSTR("service.name", "myservice"),
		OPENTELEMETRY_ATTRIBUTE_BOOL("my.bool", true),
		OPENTELEMETRY_ATTRIBUTE_INT32("my.int32", -90),
		OPENTELEMETRY_ATTRIBUTE_INT32("my.int32", -91),
		OPENTELEMETRY_ATTRIBUTE_INT64("my.int64", 0x7fffffffffffffff),
		OPENTELEMETRY_ATTRIBUTE_INT32("my.uint32", 90),
		OPENTELEMETRY_ATTRIBUTE_DOUBLE("my.double", 90.53),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE("my.array.double", dvalues),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_STR("my.array.string", svalues),
		OPENTELEMETRY_ATTRIBUTE_CSTR("my.cstr", "myvalue"),
	};
	opentelemetry_provider *provider = opentelemetry_provider_create(
		processor, NULL, provider_attrs, sizeof(provider_attrs) / sizeof(provider_attrs[0]));
	if (provider == NULL)
		return 1;
	opentelemetry_tracer *tracer = opentelemetry_provider_get_tracer(provider, "mylib", "myversion", "myschema");
	if (tracer == NULL)
		return 1;
	opentelemetry_span *span = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("root"), NULL);
	if (span == NULL)
		return 1;
	double dvalues1[] = {4.4, 5.5, 6.6};
	double dvalues2[] = {7.7, 8.8, 9.9};
	opentelemetry_attribute span_attrs[] = {
		OPENTELEMETRY_ATTRIBUTE_CSTR("service.name", "myservice1"),
		OPENTELEMETRY_ATTRIBUTE_BOOL("my.bool", false),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE("doubles", dvalues1),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE("doubles", dvalues2),
		OPENTELEMETRY_ATTRIBUTE_BOOL("my.bool", true),
		OPENTELEMETRY_ATTRIBUTE_CSTR("my.bool", "string"),
	};
	for (size_t i = 0; i != sizeof(span_attrs) / sizeof(span_attrs[0]); i++)
		opentelemetry_span_set_attribute(span, &span_attrs[i]);
	opentelemetry_attribute span_event1_attrs[] = {
		OPENTELEMETRY_ATTRIBUTE_CSTR("service.name", "myservice2"),
		OPENTELEMETRY_ATTRIBUTE_CSTR("string.empty", ""),
		OPENTELEMETRY_ATTRIBUTE_BOOL("my.bool", false),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE("doubles", dvalues2),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE("doubles", dvalues1),
		OPENTELEMETRY_ATTRIBUTE_BOOL("my.bool", true),
		OPENTELEMETRY_ATTRIBUTE_CSTR("my.bool", "string"),
		OPENTELEMETRY_ATTRIBUTE_CARRAY_STR("my.array.string", svalues),
	};

	opentelemetry_span_add_event(
		span, &(opentelemetry_string)OPENTELEMETRY_CSTR(""), NULL,
		span_event1_attrs, sizeof(span_event1_attrs) / sizeof(span_event1_attrs[0]));
	opentelemetry_span_add_event(
		span, &(opentelemetry_string)OPENTELEMETRY_CSTR("myevent2"), NULL, NULL, 0);

	opentelemetry_span *subspan = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("sub1"), span);
	opentelemetry_span_finish(subspan);

	subspan = opentelemetry_span_start2(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("sub2"), span, OPENTELEMETRY_SPAN_KIND_SERVER);
	opentelemetry_span_finish(subspan);

	opentelemetry_span_finish(span);
	opentelemetry_tracer_destroy(tracer);

	tracer = opentelemetry_provider_get_tracer(provider, "mylib", NULL, NULL);
	if (tracer == NULL)
		return 1;
	opentelemetry_tracer_destroy(tracer);
	usleep(10000);
	opentelemetry_provider_destroy(provider);
	return 0;
}
