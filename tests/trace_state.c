#include <opentelemetry-c/exporter_jaeger_trace.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

struct oheader {
	char name[256];
	size_t name_len;
	char value[256];
	size_t value_len;
	bool hit;
};

struct otheaders {
	struct oheader headers[2];
	size_t nheaders;
	int error;
};

static const struct {
	opentelemetry_string key;
	opentelemetry_string value;
} states[] = {
	{OPENTELEMETRY_CSTR("mykey1"), OPENTELEMETRY_CSTR("myvalue1")},
	{OPENTELEMETRY_CSTR("mykey2"), OPENTELEMETRY_CSTR("myvalue2")},
};
#define NSTATES (sizeof(states) / sizeof(states[0]))

static int otheaders_each(const char *name, size_t name_len, const char *value, size_t value_len, void *arg) {
	struct otheaders *ctx = arg;

	if (ctx->nheaders >= sizeof(ctx->headers) / sizeof(ctx->headers[0])) {
		ctx->error = 1;
		return -1;
	}
	struct oheader *header = &ctx->headers[ctx->nheaders++];
	if (name_len > sizeof(header->name) || value_len > sizeof(header->value)) {
		ctx->error = 1;
		return -1;
	}
	memcpy(header->name, name, name_len);
	memcpy(header->value, value, value_len);
	header->name_len = name_len;
	header->value_len = value_len;
	return 0;
}

static const char *otheaders_header_value(const char *name, size_t name_len, size_t *value_len, void *arg) {
	struct otheaders *ctx = arg;

	for (size_t i = 0; i != ctx->nheaders; i++) {
		struct oheader *header = &ctx->headers[i];

		if (header->name_len == name_len && !memcmp(header->name, name, name_len)) {
			header->hit = true;
			*value_len = header->value_len;
			return header->value;
		}
	}
	return NULL;
}

static bool otheaders_check_hit(struct otheaders *ctx) {
	for (size_t i = 0; i != ctx->nheaders; i++) {
		struct oheader *header = &ctx->headers[i];

		if (!header->hit)
			return false;
	}
	return true;
}

static bool check_states(opentelemetry_trace_state *ts, size_t count, bool present) {
	for (size_t i = 0; i != count; i++) {
		char buf[512];
		size_t buf_len = sizeof(buf) - 1;

		if (opentelemetry_trace_state_get(ts, states[i].key.ptr, states[i].key.len, buf, &buf_len) != present)
			return false;
		if (!present)
			continue;
		if (buf_len != states[i].value.len || memcmp(buf, states[i].value.ptr, buf_len))
			exit(1);
	}
	return true;
}

static bool need_sample(opentelemetry_sampling_result *result, void *arg) {
	if (arg != (void *)1)
		exit(1);
	opentelemetry_trace_state *ts = opentelemetry_trace_state_create();
	if (ts == NULL)
		exit(1);
	if (!check_states(ts, 1, false))
		exit(1);
	for (size_t i = 0; i != NSTATES; i++) {
		opentelemetry_trace_state *ts2 = opentelemetry_trace_state_set(ts, states[i].key.ptr, states[i].key.len, states[i].value.ptr, states[i].value.len);
		if (ts2 == NULL)
			exit(1);
		if (!check_states(ts2, i + 1, true))
			exit(1);
		opentelemetry_trace_state_destroy(ts);
		ts = ts2;
	}
	result->decision = OPENTELEMETRY_SAMPLING_DESISION_RECORD_AND_SAMPLE;
	result->ts = ts;

	return true;
}

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

	opentelemetry_sampler *sampler = opentelemetry_sampler_parent_root(need_sample, (void *)1);

	opentelemetry_provider *provider = opentelemetry_provider_create(
		processor, sampler, provider_attrs, sizeof(provider_attrs) / sizeof(provider_attrs[0]));
	if (provider == NULL)
		return 1;
	opentelemetry_tracer *tracer = opentelemetry_provider_get_tracer(provider, "mylib", "myversion", "myschema");
	if (tracer == NULL)
		return 1;
	opentelemetry_span *span = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("root"), NULL);
	if (span == NULL)
		return 1;
	struct otheaders otheaders = {.error = 0};
	if (opentelemetry_span_headers_get(span, otheaders_each, &otheaders))
		return 1;
	if (otheaders.error)
		return 1;
	if (otheaders.nheaders != 2)
		return 1;

	opentelemetry_trace_state *ts;

	ts = opentelemetry_span_trace_state_get(span);
	if (ts == NULL || !check_states(ts, NSTATES, true))
		return 1;
	opentelemetry_trace_state_destroy(ts);

	opentelemetry_span *subspan = opentelemetry_span_start(
		tracer,  &(opentelemetry_string)OPENTELEMETRY_CSTR("sub1"), span);
	ts = opentelemetry_span_trace_state_get(subspan);
	if (ts == NULL || !check_states(ts, NSTATES, true))
		return 1;
	opentelemetry_trace_state_destroy(ts);
	opentelemetry_span_finish(subspan);

	opentelemetry_span_finish(span);

	subspan = opentelemetry_span_start_headers(
		tracer, &(opentelemetry_string)OPENTELEMETRY_CSTR("sub2"),
		otheaders_header_value, &otheaders);
	ts = opentelemetry_span_trace_state_get(subspan);
	if (ts == NULL || !check_states(ts, NSTATES, true))
		return 1;
	opentelemetry_trace_state_destroy(ts);
	opentelemetry_span_finish(subspan);

	opentelemetry_tracer_destroy(tracer);
	opentelemetry_provider_destroy(provider);

	if (!otheaders_check_hit(&otheaders))
		return 1;
	return 0;
}
