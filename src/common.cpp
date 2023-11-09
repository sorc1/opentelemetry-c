#include <opentelemetry-c/common.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/processor.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/batch_span_processor.h>
#include <opentelemetry/sdk/trace/multi_span_processor.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/sdk/trace/samplers/always_on.h>
#include <opentelemetry/sdk/trace/samplers/always_off.h>
#include <opentelemetry/sdk/trace/samplers/trace_id_ratio.h>
#include <opentelemetry/sdk/trace/samplers/parent.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/tracer_provider.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include "attributes_iterable.h"
#include "headers_carrier.h"
#include "tracer.h"
#include "span.h"
#include "sampler_parent_root.h"

namespace common = OPENTELEMETRY_NAMESPACE::common;
namespace context = OPENTELEMETRY_NAMESPACE::context;
namespace trace = OPENTELEMETRY_NAMESPACE::trace;
namespace nostd = OPENTELEMETRY_NAMESPACE::nostd;
namespace sdkcommon = OPENTELEMETRY_NAMESPACE::sdk::common;
namespace sdktrace = OPENTELEMETRY_NAMESPACE::sdk::trace;
namespace sdkresource = OPENTELEMETRY_NAMESPACE::sdk::resource;

#define RAW_BOOL_LEN 4
#define RAW_INT32_LEN 4
#define RAW_INT64_LEN 8
#define RAW_UINT32_LEN 4
#define RAW_DOUBLE_LEN 8
#define RAW_STRING_LEN(str) ((str)->len + 4)
#define RAW_ARRAY_ELMPAD 1

extern "C" void opentelemetry_exporter_destroy(opentelemetry_exporter *exporter_) {
	try {
		auto exporter = reinterpret_cast<sdktrace::SpanExporter *>(exporter_);

		if (exporter == nullptr)
			return;

		delete exporter;
	} catch (...) {
	}
}

extern "C" opentelemetry_processor *opentelemetry_processor_simple(opentelemetry_exporter *exporter_) {
	try {
		auto exporter = reinterpret_cast<sdktrace::SpanExporter *>(exporter_);
		auto exporter_uptr = std::unique_ptr<sdktrace::SpanExporter>(exporter);
		auto processor = new sdktrace::SimpleSpanProcessor(std::move(exporter_uptr));
		return reinterpret_cast<opentelemetry_processor *>(processor);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_processor *opentelemetry_processor_batch(
	opentelemetry_exporter *exporter_,
	const opentelemetry_processor_batch_options *options) {

	try {
		auto exporter = reinterpret_cast<sdktrace::SpanExporter *>(exporter_);
		auto exporter_uptr = std::unique_ptr<sdktrace::SpanExporter>(exporter);
		sdktrace::BatchSpanProcessor *processor;
		if (options == NULL) {
			sdktrace::BatchSpanProcessorOptions opts;
			processor = new sdktrace::BatchSpanProcessor(std::move(exporter_uptr), opts);
		} else {
			sdktrace::BatchSpanProcessorOptions opts = {
				.max_queue_size = options->max_queue_size,
				.schedule_delay_millis = std::chrono::milliseconds(options->schedule_delay_millis),
				.max_export_batch_size = options->max_export_batch_size,
			};
			processor = new sdktrace::BatchSpanProcessor(std::move(exporter_uptr), opts);
		}
		return reinterpret_cast<opentelemetry_processor *>(processor);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_processor *opentelemetry_processor_multi(
	opentelemetry_processor *const*processors, size_t nprocessors) {
	try {
		auto multi = new sdktrace::MultiSpanProcessor(std::vector<std::unique_ptr<sdktrace::SpanProcessor>>());
		for (size_t i = 0; i != nprocessors; i++) {
			auto processor = reinterpret_cast<sdktrace::SpanProcessor *>(processors[i]);
			auto processor_uptr = std::unique_ptr<sdktrace::SpanProcessor>(processor);
			multi->AddProcessor(std::move(processor_uptr));
		}
		return reinterpret_cast<opentelemetry_processor *>(multi);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_processor_destroy(opentelemetry_processor *processor_) {
	try {
		auto processor = reinterpret_cast<sdktrace::SpanProcessor *>(processor_);

		if (processor == nullptr)
			return;

		delete processor;
	} catch (...) {
	}
}

extern "C" opentelemetry_trace_state *opentelemetry_trace_state_create(void) {
	try {
		auto ts = new nostd::shared_ptr<trace::TraceState>(trace::TraceState::GetDefault());
		return reinterpret_cast<opentelemetry_trace_state *>(ts);
	} catch (...) {
		return NULL;
	}
}

extern "C" bool opentelemetry_trace_state_get(opentelemetry_trace_state *ts_, const char *key, size_t key_len, char *value, size_t *value_len) {
	try {
		auto ts = reinterpret_cast<nostd::shared_ptr<trace::TraceState> *>(ts_);
		std::string cpp_value;
		auto rv = (*ts)->Get(nostd::string_view(key, key_len), cpp_value);
		if (!rv)
			return false;
		size_t cpp_value_len = cpp_value.length();
		if (cpp_value_len > *value_len)
			cpp_value_len = *value_len;
		std::memcpy(value, cpp_value.data(), cpp_value_len);
		value[cpp_value_len] = '\0';
		*value_len = cpp_value_len;
		return true;
	} catch (...) {
		return false;
	}
}

extern "C" opentelemetry_trace_state *opentelemetry_trace_state_set(opentelemetry_trace_state *ts_, const char *key, size_t key_len, const char *value, size_t value_len) {
	try {
		auto ts_old = reinterpret_cast<nostd::shared_ptr<trace::TraceState> *>(ts_);
		auto ts_new = (*ts_old)->Set(nostd::string_view(key, key_len), nostd::string_view(value, value_len));
		auto ts = new nostd::shared_ptr<trace::TraceState>(ts_new);
		return reinterpret_cast<opentelemetry_trace_state *>(ts);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_trace_state_destroy(opentelemetry_trace_state *ts_) {
	try {
		auto ts = reinterpret_cast<nostd::shared_ptr<trace::TraceState> *>(ts_);

		if (ts == nullptr)
			return;
		delete ts;
	} catch (...) {
	}
}

extern "C" opentelemetry_sampler *opentelemetry_sampler_always_on(void) {
	try {
		auto sampler = new sdktrace::AlwaysOnSampler();
		return reinterpret_cast<opentelemetry_sampler *>(sampler);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_sampler *opentelemetry_sampler_always_off(void) {
	try {
		auto sampler = new sdktrace::AlwaysOffSampler();
		return reinterpret_cast<opentelemetry_sampler *>(sampler);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_sampler *opentelemetry_sampler_trace_id_ratio(double ratio) {
	try {
		auto sampler = new sdktrace::TraceIdRatioBasedSampler(ratio);
		return reinterpret_cast<opentelemetry_sampler *>(sampler);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_sampler *opentelemetry_sampler_parent(opentelemetry_sampler *delegate_sampler_) {
	try {
		auto delegate_sampler = reinterpret_cast<sdktrace::Sampler *>(delegate_sampler_);
		auto sampler = new sdktrace::ParentBasedSampler(std::shared_ptr<sdktrace::Sampler>(delegate_sampler));
		/*
		 * from now, depegate_sampler is under ParentBasedSampler control, we shouldn't
		 * destroy it explicitly
		 */
		return reinterpret_cast<opentelemetry_sampler *>(sampler);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_sampler *opentelemetry_sampler_parent_root(opentelemetry_sampler_parent_root_cb cb, void *arg) {
	try {
		auto sampler = new sdktrace::OpentelemetryCParentRootSampler(cb, arg);
		return reinterpret_cast<opentelemetry_sampler *>(sampler);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_sampler_destroy(opentelemetry_sampler *sampler_) {
	try {
		auto sampler = reinterpret_cast<sdktrace::Sampler *>(sampler_);

		if (sampler == nullptr)
			return;

		delete sampler;
	} catch (...) {
	}
}

extern "C" opentelemetry_provider *opentelemetry_provider_create(
	opentelemetry_processor *processor_, opentelemetry_sampler *sampler_,
	opentelemetry_attribute *attributes, size_t nattributes) {

	try {
		if (sampler_ == NULL)
			sampler_ = opentelemetry_sampler_always_on();
		auto processor = reinterpret_cast<sdktrace::SpanProcessor *>(processor_);
		auto sampler = reinterpret_cast<sdktrace::Sampler *>(sampler_);
		auto processor_uptr = std::unique_ptr<sdktrace::SpanProcessor>(processor);
		auto sampler_uptr = std::unique_ptr<sdktrace::Sampler>(sampler);
		common::OpentelemetryCAttributesIterable attrs(attributes, nattributes);
		auto provider = new sdktrace::TracerProvider(
			std::move(processor_uptr),
			sdkresource::Resource::Create(attrs),
			std::move(sampler_uptr));
		return reinterpret_cast<opentelemetry_provider *>(provider);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_provider_destroy(opentelemetry_provider *provider_) {
	try {
		auto provider = reinterpret_cast<trace::TracerProvider *>(provider_);

		if (provider == nullptr)
			return;

		delete provider;
	} catch (...) {
	}
}

extern "C" opentelemetry_tracer *opentelemetry_provider_get_tracer(opentelemetry_provider *provider_, const char *library_name, const char *library_version, const char *schema_url) {
	try {
		if (library_version == NULL)
			library_version = "";
		if (schema_url == NULL)
			schema_url = "";
		auto provider = reinterpret_cast<trace::TracerProvider *>(provider_);
		auto tracer = new trace::OpentelemetryCTracer(nostd::shared_ptr<trace::Tracer>(
			provider->GetTracer(library_name, library_version, schema_url)));
		return reinterpret_cast<opentelemetry_tracer *>(tracer);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_tracer_limit_span_size(opentelemetry_tracer *tracer_, bool enable) {
	try {
		auto tracer = reinterpret_cast<trace::OpentelemetryCTracer*>(tracer_);
		tracer->LimitSpanSize(enable);
	} catch (...) {
	}
}

extern "C" void opentelemetry_tracer_destroy(opentelemetry_tracer *tracer_) {
	try {
		auto tracer = reinterpret_cast<trace::OpentelemetryCTracer*>(tracer_);
		if (tracer == NULL)
			return;
		delete tracer;
	} catch (...) {
	}
}

extern "C" opentelemetry_span *opentelemetry_span_start(opentelemetry_tracer *tracer_, const opentelemetry_string *name, opentelemetry_span *parent_span) {
	try {
		auto tracer = reinterpret_cast<trace::OpentelemetryCTracer*>(tracer_);

		trace::OpentelemetryCSpan *span;
		if (parent_span == NULL) {
			span = new trace::OpentelemetryCSpan(tracer, nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len))));
		} else {
			auto parent = reinterpret_cast<trace::OpentelemetryCSpan*>(parent_span);
			trace::StartSpanOptions options = {
				common::SystemTimestamp(),
				common::SteadyTimestamp(),
				parent->get_context(),
			};
			span = new trace::OpentelemetryCSpan(tracer, nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len), options)));
		}
		return reinterpret_cast<opentelemetry_span *>(span);
	} catch (...) {
		return NULL;
	}
}

extern "C" opentelemetry_trace_state *opentelemetry_span_trace_state_get(opentelemetry_span *span_) {
	try {
		auto span = reinterpret_cast<trace::OpentelemetryCSpan*>(span_);
		auto context = span->get_context();
		auto context_ts = context.trace_state();
		if (context_ts == nullptr)
			return NULL;
		auto ts = new nostd::shared_ptr<trace::TraceState>(context_ts);
		return reinterpret_cast<opentelemetry_trace_state *>(ts);
	} catch (...) {
		return NULL;
	}
}

static size_t opentelemetry_attribute_size(const opentelemetry_attribute *attr) {
	size_t size = RAW_STRING_LEN(&attr->name);

	switch (attr->value.type) {
	case OPENTELEMETRY_TYPE_BOOL: size += RAW_BOOL_LEN; break;
	case OPENTELEMETRY_TYPE_INT32: size += RAW_INT32_LEN; break;
	case OPENTELEMETRY_TYPE_UINT32: size += RAW_UINT32_LEN; break;
	case OPENTELEMETRY_TYPE_INT64: size += RAW_INT64_LEN; break;
	case OPENTELEMETRY_TYPE_DOUBLE:  size += RAW_DOUBLE_LEN; break;
	case OPENTELEMETRY_TYPE_STRING:
		size += RAW_STRING_LEN(&attr->value.s); break;
	case OPENTELEMETRY_TYPE_ARRAY_BOOL:
		size = (size + RAW_BOOL_LEN + RAW_ARRAY_ELMPAD) * attr->value.ab.len; break;
	case OPENTELEMETRY_TYPE_ARRAY_INT32:
		size = (size + RAW_INT32_LEN + RAW_ARRAY_ELMPAD) * attr->value.ai32.len; break;
	case OPENTELEMETRY_TYPE_ARRAY_UINT32:
		size = (size + RAW_UINT32_LEN + RAW_ARRAY_ELMPAD) * attr->value.au32.len; break;
	case OPENTELEMETRY_TYPE_ARRAY_INT64:
		size = (size + RAW_INT64_LEN + RAW_ARRAY_ELMPAD) * attr->value.ai64.len; break;
	case OPENTELEMETRY_TYPE_ARRAY_DOUBLE:
		size = (size + RAW_DOUBLE_LEN + RAW_ARRAY_ELMPAD) * attr->value.ad.len; break;
	case OPENTELEMETRY_TYPE_ARRAY_STRING:
		size = 0;
		for (size_t i = 0; i != attr->value.as.len; i++)
			size += RAW_STRING_LEN(&attr->name) + RAW_STRING_LEN(&attr->value.as.values[i]);
		break;
	}
	return size;
}

extern "C" void opentelemetry_span_set_attribute(opentelemetry_span *span_, const opentelemetry_attribute *attribute) {
	try {
		auto span = reinterpret_cast<trace::OpentelemetryCSpan*>(span_);
		common::OpentelemetryCAttributesIterable attrs(attribute, 1);
		if (span->HasSpanSizeLimit()) {
			size_t attr_size = opentelemetry_attribute_size(attribute);
			if (attr_size > SPAN_LOG_SPLIT) {
				// We don't support span attributes chunking. Just skip the attribute.
				return;
			}
			if (attr_size > span->SpanSizeAvailable())
				span->Reopen();
			span->StoreSize(attr_size);
		}
		attrs.ForEachKeyValue(
			[&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
				span->get()->SetAttribute(key, value);
				return false;
		});
	} catch (...) {
	}
}

static void opentelemetry_span_add_event_attributes(
	trace::OpentelemetryCSpan *span, const opentelemetry_string *name, const struct timespec *tp,
	const opentelemetry_attribute *attributes, size_t nattributes) {

	common::OpentelemetryCAttributesIterable attrs(attributes, nattributes);
	if (tp == NULL) {
		span->get()->AddEvent(nostd::string_view(name->ptr, name->len), attrs);
	} else {
		uint64_t nanos = tp == NULL ? 0 : ((uint64_t)tp->tv_sec * 1000000000 + tp->tv_nsec);
		span->get()->AddEvent(
			nostd::string_view(name->ptr, name->len),
			common::SystemTimestamp(std::chrono::nanoseconds{nanos}), attrs);
	}
}

static void opentelemetry_span_add_event_attribute_chunked_as_partial(
	trace::OpentelemetryCSpan *span, const opentelemetry_string *name, const struct timespec *tp,
	const opentelemetry_attribute *attr) {

	opentelemetry_attribute vattr = *attr;
	opentelemetry_string str = vattr.value.as.values[0];
	vattr.value.as.len = 1;
	vattr.value.as.values = &str;

	while (1) {
		size_t size = opentelemetry_attribute_size(&vattr);
		if (size > SPAN_LOG_SPLIT) {
			size_t orig_len = str.len;

			str.len = SPAN_LOG_SPLIT - RAW_STRING_LEN(&vattr.name);
			opentelemetry_span_add_event_attributes(span, name, tp, &vattr, 1);
			span->Reopen();
			str.ptr += str.len;
			str.len = orig_len - str.len;
		} else {
			opentelemetry_span_add_event_attributes(span, name, tp, &vattr, 1);
			span->StoreSize(size);
			return;
		}
	}
}

static bool opentelemetry_span_add_event_attribute_chunked_as(
	trace::OpentelemetryCSpan *span, const opentelemetry_string *name, const struct timespec *tp,
	opentelemetry_attribute *vattr) {

	size_t nstrings = vattr->value.as.len;
	size_t total = 0;
	size_t i;

	for (i = 0; i != nstrings; i++) {
		size_t len = RAW_STRING_LEN(&vattr->name) + RAW_STRING_LEN(&vattr->value.as.values[i]);
		size_t total_new = total + len;

		if (total_new <= SPAN_LOG_SPLIT) {
			total = total_new;
			continue;
		}
		if (i) {
			vattr->value.as.len = i;
			opentelemetry_span_add_event_attributes(span, name, tp, vattr, 1);
			span->Reopen();
			vattr->value.as.values += i;
			vattr->value.as.len = nstrings - i;
			return false;
		}
		opentelemetry_span_add_event_attribute_chunked_as_partial(span, name, tp, vattr);
		vattr->value.as.values++;
		if (--vattr->value.as.len) {
			span->Reopen();
			return false;
		}
		return true;
	}
	opentelemetry_span_add_event_attributes(span, name, tp, vattr, 1);
	span->StoreSize(opentelemetry_attribute_size(vattr));

	return true;
}

static void opentelemetry_span_add_event_attribute_chunked(
	trace::OpentelemetryCSpan *span, const opentelemetry_string *name, const struct timespec *tp,
	const opentelemetry_attribute *attr) {

	if (attr->name.len > SPAN_LOG_NAME_LEN_MAX)
		return;

	// The span is empty (has SPAN_LOG_SPLIT bytes available).
	// The attribute size is greater than SPAN_LOG_SPLIT.
	// That is possible for:
	// - OPENTELEMETRY_TYPE_STRING
	// - long arrays of non-string types
	// - array of strings
	// We'll split all the attributes above.

	bool done;
	opentelemetry_attribute lattr = *attr;

#define SET_CHUNKED_VALUE(done, lattr, tvar, value_size) \
	do { \
		size_t elm_size = RAW_STRING_LEN(&lattr.name) + value_size + RAW_ARRAY_ELMPAD; \
		size_t max_len = SPAN_LOG_SPLIT / elm_size; \
		size_t len = lattr.value.tvar.len; \
		if (!(done = (len <= max_len))) { \
			lattr.value.tvar.len = max_len; \
			opentelemetry_span_add_event_attributes(span, name, tp, &lattr, 1); \
			span->Reopen(); \
			lattr.value.tvar.values += max_len; \
			lattr.value.tvar.len = len - max_len; \
		} else { \
			opentelemetry_span_add_event_attributes(span, name, tp, &lattr, 1); \
			span->StoreSize(elm_size * lattr.value.tvar.len); \
		} \
	} while(0)

	do {
		switch (lattr.value.type) {
		case OPENTELEMETRY_TYPE_STRING: {
			size_t size = opentelemetry_attribute_size(&lattr);
			size_t len = lattr.value.s.len;
			if (!(done = (size <= SPAN_LOG_SPLIT))) {
				size_t orig_len = lattr.value.s.len;

				lattr.value.s.len = SPAN_LOG_SPLIT - RAW_STRING_LEN(&lattr.name);
				opentelemetry_span_add_event_attributes(span, name, tp, &lattr, 1);
				span->Reopen();
				lattr.value.s.ptr += lattr.value.s.len;
				lattr.value.s.len = orig_len - lattr.value.s.len;
			} else {
				opentelemetry_span_add_event_attributes(span, name, tp, &lattr, 1);
				span->StoreSize(opentelemetry_attribute_size(&lattr));
			}
			break;
		}
		case OPENTELEMETRY_TYPE_ARRAY_BOOL: SET_CHUNKED_VALUE(done, lattr, ab, RAW_BOOL_LEN); break;
		case OPENTELEMETRY_TYPE_ARRAY_INT32: SET_CHUNKED_VALUE(done, lattr, ai32, RAW_INT32_LEN); break;
		case OPENTELEMETRY_TYPE_ARRAY_UINT32: SET_CHUNKED_VALUE(done, lattr, au32, RAW_UINT32_LEN); break;
		case OPENTELEMETRY_TYPE_ARRAY_INT64: SET_CHUNKED_VALUE(done, lattr, ai64, RAW_INT64_LEN); break;
		case OPENTELEMETRY_TYPE_ARRAY_DOUBLE: SET_CHUNKED_VALUE(done, lattr, ad, RAW_DOUBLE_LEN); break;
		case OPENTELEMETRY_TYPE_ARRAY_STRING:
			done = opentelemetry_span_add_event_attribute_chunked_as(span, name, tp, &lattr);
			break;
		default:
			return;
		}
	} while (!done);
#undef SET_CHUNKED_VALUE
}

static size_t opentelemetry_span_add_event_attributes_partial(
	trace::OpentelemetryCSpan *span, const opentelemetry_string *name, const struct timespec *tp,
	const opentelemetry_attribute *attributes, size_t nattributes) {

	for (size_t i = 0; i != nattributes; i++) {
		const opentelemetry_attribute *attr = &attributes[i];
		uint64_t available = span->SpanSizeAvailable();
		size_t attr_size = opentelemetry_attribute_size(attr);
		if (attr_size > available) {
			if (i) {
				opentelemetry_span_add_event_attributes(span, name, tp, attributes, i);
				span->Reopen();
				return i;
			}
			if (available < SPAN_LOG_SPLIT)
				span->Reopen();
			if (attr_size < SPAN_LOG_SPLIT)
				return 0;
			opentelemetry_span_add_event_attribute_chunked(span, name, tp, attr);
			return 1;
		}
		span->StoreSize(attr_size);
	}
	opentelemetry_span_add_event_attributes(span, name, tp, attributes, nattributes);
	return nattributes;
}

extern "C" void opentelemetry_span_add_event(
	opentelemetry_span *span_, const opentelemetry_string *name, const struct timespec *tp,
	const opentelemetry_attribute *attributes, size_t nattributes) {
	try {
		auto span = reinterpret_cast<trace::OpentelemetryCSpan*>(span_);
		if (!span->HasSpanSizeLimit()) {
			opentelemetry_span_add_event_attributes(span, name, tp, attributes, nattributes);
			return;
		}
		do {
			size_t added_attributes = opentelemetry_span_add_event_attributes_partial(span, name, tp, attributes, nattributes);
			nattributes -= added_attributes;
			attributes += added_attributes;
		} while (nattributes);
	} catch (...) {
	}
}

extern "C" int opentelemetry_span_headers_get(opentelemetry_span *span_, opentelemetry_header_each header_each, void *header_each_arg) {
	try {
		auto span = reinterpret_cast<trace::OpentelemetryCSpan*>(span_);
		context::propagation::OpentelemetryCHeadersInjector injector(header_each, header_each_arg);
		context::Context c;
		context::Context context = trace::SetSpan(c, span->get_shared());
		trace::propagation::HttpTraceContext().Inject(injector,	context);
		return 0;
	} catch (...) {
		return -1;
	}
}

extern "C" opentelemetry_span *opentelemetry_span_start_headers(opentelemetry_tracer *tracer_, const opentelemetry_string *name, opentelemetry_header_value header_value, void *header_value_arg) {
	try {
		auto tracer = reinterpret_cast<trace::OpentelemetryCTracer*>(tracer_);
		context::propagation::OpentelemetryCHeadersExtractor extractor(header_value, header_value_arg);
		context::Context c;
		context::Context context = trace::propagation::HttpTraceContext().Extract(extractor, c);
		auto sp = trace::GetSpan(context);
		trace::StartSpanOptions options = {
				common::SystemTimestamp(),
				common::SteadyTimestamp(),
				sp->GetContext(),
		};
		if (nostd::holds_alternative<trace::SpanContext>(options.parent)) {
			auto spanContext = nostd::get<trace::SpanContext>(options.parent);
			if (!spanContext.IsValid())
				return NULL;
		}
		auto span = new trace::OpentelemetryCSpan(tracer, nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len), options)));
		return reinterpret_cast<opentelemetry_span *>(span);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_span_finish(opentelemetry_span *span_) {
	try {
		auto span = reinterpret_cast<trace::OpentelemetryCSpan*>(span_);
		if (span == NULL)
			return;
		delete span;
	} catch (...) {
	}
}

extern "C" void opentelemetry_set_log_level(int log_level) {
	try {
		sdkcommon::internal_log::GlobalLogHandler::SetLogLevel(
			static_cast<sdkcommon::internal_log::LogLevel>(log_level));
	} catch (...) {
	}
}
