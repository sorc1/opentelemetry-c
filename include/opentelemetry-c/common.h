#ifndef OPENTELEMETRY_C_COMMON_H_
#define OPENTELEMETRY_C_COMMON_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct opentelemetry_exporter opentelemetry_exporter;
typedef struct opentelemetry_processor opentelemetry_processor;
typedef struct opentelemetry_sampler opentelemetry_sampler;
typedef struct opentelemetry_provider opentelemetry_provider;
typedef struct opentelemetry_tracer opentelemetry_tracer;
typedef struct opentelemetry_trace_state opentelemetry_trace_state;
typedef struct opentelemetry_span opentelemetry_span;

enum opentelemetry_value_type {
	OPENTELEMETRY_TYPE_BOOL = 0,
	OPENTELEMETRY_TYPE_INT32,
	OPENTELEMETRY_TYPE_INT64,
	OPENTELEMETRY_TYPE_UINT32,
	OPENTELEMETRY_TYPE_DOUBLE,
	OPENTELEMETRY_TYPE_STRING,
	OPENTELEMETRY_TYPE_ARRAY_BOOL,
	OPENTELEMETRY_TYPE_ARRAY_INT32,
	OPENTELEMETRY_TYPE_ARRAY_INT64,
	OPENTELEMETRY_TYPE_ARRAY_UINT32,
	OPENTELEMETRY_TYPE_ARRAY_DOUBLE,
	OPENTELEMETRY_TYPE_ARRAY_STRING,
};

#define OPENTELEMETRY_STR(value, value_len) {.len = value_len, .ptr = value}
#define OPENTELEMETRY_CSTR(value) OPENTELEMETRY_STR(value, sizeof(value) - 1)

/* order of fields should be the same as for opentelemetry::nostd::string_view */
typedef struct opentelemetry_string {
	size_t len;
	const char *ptr;
} opentelemetry_string;

typedef struct opentelemetry_http_header {
	opentelemetry_string name;
	opentelemetry_string value;
} opentelemetry_http_header;

#define OPENTELEMETRY_ARRAY(values_, len_) {.len = len_, .values = values_}
#define OPENTELEMETRY_CARRAY(values_) OPENTELEMETRY_ARRAY(values_, sizeof(values_) / sizeof(values_[0]))

typedef struct opentelemetry_array_bool {
	size_t len;
	const bool *values;
} opentelemetry_array_bool;

typedef struct opentelemetry_array_int32 {
	size_t len;
	const int32_t *values;
} opentelemetry_array_int32;

typedef struct opentelemetry_array_int64 {
	size_t len;
	const int64_t *values;
} opentelemetry_array_int64;

typedef struct opentelemetry_array_uint32 {
	size_t len;
	const uint32_t *values;
} opentelemetry_array_uint32;

typedef struct opentelemetry_array_double {
	size_t len;
	const double *values;
} opentelemetry_array_double;

typedef struct opentelemetry_array_string {
	size_t len;
	const opentelemetry_string *values;
} opentelemetry_array_string;

#define OPENTELEMETRY_VALUE_BOOL(value) {.type = OPENTELEMETRY_TYPE_BOOL, .b = value}
#define OPENTELEMETRY_VALUE_INT32(value) {.type = OPENTELEMETRY_TYPE_INT32, .i32 = value}
#define OPENTELEMETRY_VALUE_INT64(value) {.type = OPENTELEMETRY_TYPE_INT64, .i64 = value}
#define OPENTELEMETRY_VALUE_UINT32(value) {.type = OPENTELEMETRY_TYPE_UINT32, .u32 = value}
#define OPENTELEMETRY_VALUE_DOUBLE(value) {.type = OPENTELEMETRY_TYPE_DOUBLE, .d = value}
#define OPENTELEMETRY_VALUE_CSTR(value) {.type = OPENTELEMETRY_TYPE_STRING, .s = OPENTELEMETRY_CSTR(value)}
#define OPENTELEMETRY_VALUE_STR(value, value_len) {.type = OPENTELEMETRY_TYPE_STRING, .s = OPENTELEMETRY_STR(value, value_len)}
#define OPENTELEMETRY_VALUE_ARRAY_BOOL(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .ab = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_ARRAY_INT32(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_INT32, .ai32 = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_ARRAY_INT64(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_INT64, .ai64 = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_ARRAY_UINT32(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_UINT32, .au32 = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_ARRAY_DOUBLE(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_DOUBLE, .ad = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_ARRAY_STR(values, len) {.type = OPENTELEMETRY_TYPE_ARRAY_STRING, .as = OPENTELEMETRY_ARRAY(values, len)}
#define OPENTELEMETRY_VALUE_CARRAY_BOOL(values) {.type = OPENTELEMETRY_TYPE_ARRAY_BOOL, .ab = OPENTELEMETRY_CARRAY(values)}
#define OPENTELEMETRY_VALUE_CARRAY_INT32(values) {.type = OPENTELEMETRY_TYPE_ARRAY_INT32, .ai32 = OPENTELEMETRY_CARRAY(values)}
#define OPENTELEMETRY_VALUE_CARRAY_INT64(values) {.type = OPENTELEMETRY_TYPE_ARRAY_INT64, .ai64 = OPENTELEMETRY_CARRAY(values)}
#define OPENTELEMETRY_VALUE_CARRAY_UINT32(values) {.type = OPENTELEMETRY_TYPE_ARRAY_UINT32, .au32 = OPENTELEMETRY_CARRAY(values)}
#define OPENTELEMETRY_VALUE_CARRAY_DOUBLE(values) {.type = OPENTELEMETRY_TYPE_ARRAY_DOUBLE, .ad = OPENTELEMETRY_CARRAY(values)}
#define OPENTELEMETRY_VALUE_CARRAY_STR(values) {.type = OPENTELEMETRY_TYPE_ARRAY_STRING, .as = OPENTELEMETRY_CARRAY(values)}

typedef struct opentelemetry_value {
	unsigned type; // enum opentelemetry_value_type
	union {
		bool b;
		int32_t i32;
		int64_t i64;
		uint32_t u32;
		double d;
		opentelemetry_string s;
		opentelemetry_array_bool ab;
		opentelemetry_array_int32 ai32;
		opentelemetry_array_int64 ai64;
		opentelemetry_array_uint32 au32;
		opentelemetry_array_double ad;
		opentelemetry_array_string as;
	};
} opentelemetry_value;

#define OPENTELEMETRY_ATTRIBUTE(name_, value_) {.name = name_, .value = value_}
#define OPENTELEMETRY_ATTRIBUTE_BOOL(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_BOOL(value_))
#define OPENTELEMETRY_ATTRIBUTE_INT32(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_INT32(value_))
#define OPENTELEMETRY_ATTRIBUTE_INT64(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_INT64(value_))
#define OPENTELEMETRY_ATTRIBUTE_UINT32(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_UINT32(value_))
#define OPENTELEMETRY_ATTRIBUTE_DOUBLE(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_DOUBLE(value_))
#define OPENTELEMETRY_ATTRIBUTE_CSTR(name_, value_) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CSTR(value_))
#define OPENTELEMETRY_ATTRIBUTE_STR(name_, value_, value_len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_STR(value_, value_len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_BOOL(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_ARRAY_BOOL(values, len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_INT32(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_ARRAY_INT32(values, len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_INT64(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_ARRAY_INT64(values, len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_UINT32(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_ARRAY_UINT32(values, len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_DOUBLE(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_ARRAY_DOUBLE(values, len))
#define OPENTELEMETRY_ATTRIBUTE_ARRAY_STR(name_, values, len) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_STR(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_BOOL(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_BOOL(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_INT32(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_INT32(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_INT64(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_INT64(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_UINT32(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_UINT32(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_DOUBLE(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_DOUBLE(values))
#define OPENTELEMETRY_ATTRIBUTE_CARRAY_STR(name_, values) OPENTELEMETRY_ATTRIBUTE(OPENTELEMETRY_CSTR(name_), OPENTELEMETRY_VALUE_CARRAY_STR(values))

typedef struct opentelemetry_attribute {
	opentelemetry_string name;
	opentelemetry_value value;
} opentelemetry_attribute;

void opentelemetry_exporter_destroy(opentelemetry_exporter *exporter);

opentelemetry_processor *opentelemetry_processor_simple(opentelemetry_exporter *exporter);

typedef struct opentelemetry_processor_batch_options {
	/**
	 * The maximum buffer/queue size. After the size is reached, spans are
	 * dropped.
	 */
	size_t max_queue_size;

	/* The time interval between two consecutive exports. */
	int64_t schedule_delay_millis;

	/**
	 * The maximum batch size of every export. It must be smaller or
	 * equal to max_queue_size.
	 */
	size_t max_export_batch_size;
} opentelemetry_processor_batch_options;

opentelemetry_processor *opentelemetry_processor_batch(
	opentelemetry_exporter *exporter,
	const opentelemetry_processor_batch_options *options);

opentelemetry_processor *opentelemetry_processor_multi(
	opentelemetry_processor *const*processors, size_t nprocessors);

void opentelemetry_processor_destroy(opentelemetry_processor *processor);

opentelemetry_trace_state *opentelemetry_trace_state_create(void);
bool opentelemetry_trace_state_get(opentelemetry_trace_state *ts, const char *key, size_t key_len, char *value, size_t *value_len);
opentelemetry_trace_state *opentelemetry_trace_state_set(opentelemetry_trace_state *ts, const char *key, size_t key_len, const char *value, size_t value_len);
void opentelemetry_trace_state_destroy(opentelemetry_trace_state *ts);

enum opentelemetry_sampling_decision {
	OPENTELEMETRY_SAMPLING_DESISION_DROP = 0,
	OPENTELEMETRY_SAMPLING_DESISION_RECORD_ONLY,
	OPENTELEMETRY_SAMPLING_DESISION_RECORD_AND_SAMPLE,
};

typedef struct opentelemetry_sampling_result {
	unsigned decision; // enum opentelemetry_sampling_decision
	opentelemetry_trace_state *ts;
} opentelemetry_sampling_result;

opentelemetry_sampler *opentelemetry_sampler_always_on(void);
opentelemetry_sampler *opentelemetry_sampler_always_off(void);
opentelemetry_sampler *opentelemetry_sampler_trace_id_ratio(double ratio);
opentelemetry_sampler *opentelemetry_sampler_parent(opentelemetry_sampler *delegate_sampler);
typedef bool (*opentelemetry_sampler_parent_root_cb)(opentelemetry_sampling_result *result, void *arg);
opentelemetry_sampler *opentelemetry_sampler_parent_root(opentelemetry_sampler_parent_root_cb cb, void *arg);
void opentelemetry_sampler_destroy(opentelemetry_sampler *sampler);

opentelemetry_provider *opentelemetry_provider_create(
	opentelemetry_processor *processor, opentelemetry_sampler *sampler,
	opentelemetry_attribute *attributes, size_t nattributes);
void opentelemetry_provider_destroy(opentelemetry_provider *provider);

opentelemetry_tracer *opentelemetry_provider_get_tracer(opentelemetry_provider *provider, const char *library_name, const char *library_version, const char *schema_url);
void opentelemetry_tracer_limit_span_size(opentelemetry_tracer *tracer, bool enable);
void opentelemetry_tracer_destroy(opentelemetry_tracer *tracer);

opentelemetry_span *opentelemetry_span_start(opentelemetry_tracer *tracer, const opentelemetry_string *name, opentelemetry_span *parent_span);
opentelemetry_trace_state *opentelemetry_span_trace_state_get(opentelemetry_span *span);
void opentelemetry_span_set_attribute(opentelemetry_span *span, const opentelemetry_attribute *attribute);
void opentelemetry_span_add_event(
	opentelemetry_span *span, const opentelemetry_string *name,	const struct timespec *tp,
	const opentelemetry_attribute *attributes, size_t nattributes);
typedef int (*opentelemetry_header_each)(const char *name, size_t name_len, const char *value, size_t value_len, void *arg);
int opentelemetry_span_headers_get(opentelemetry_span *span, opentelemetry_header_each header_each, void *header_each_arg);
typedef const char *(*opentelemetry_header_value)(const char *name, size_t name_len, size_t *value_len, void *arg);
opentelemetry_span *opentelemetry_span_start_headers(opentelemetry_tracer *tracer, const opentelemetry_string *name, opentelemetry_header_value header_value, void *header_value_arg);
void opentelemetry_span_finish(opentelemetry_span *span);

enum opentelemetry_log_level {
	OPENTELEMETRY_LOG_LEVEL_ERROR = 0,
	OPENTELEMETRY_LOG_LEVEL_WARNING,
	OPENTELEMETRY_LOG_LEVEL_INFO,
	OPENTELEMETRY_LOG_LEVEL_DEBUG,
};

void opentelemetry_set_log_level(int log_level); // enum opentelemetry_log_level

# ifdef  __cplusplus
}
# endif

#endif /* OPENTELEMETRY_C_COMMON_H_*/
