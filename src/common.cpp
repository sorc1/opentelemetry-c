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

namespace common = OPENTELEMETRY_NAMESPACE::common;
namespace context = OPENTELEMETRY_NAMESPACE::context;
namespace trace = OPENTELEMETRY_NAMESPACE::trace;
namespace nostd = OPENTELEMETRY_NAMESPACE::nostd;
namespace sdkcommon = OPENTELEMETRY_NAMESPACE::sdk::common;
namespace sdktrace = OPENTELEMETRY_NAMESPACE::sdk::trace;
namespace sdkresource = OPENTELEMETRY_NAMESPACE::sdk::resource;

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
		auto tracer = new nostd::shared_ptr<trace::Tracer>(provider->GetTracer(
			library_name, library_version, schema_url));
		return reinterpret_cast<opentelemetry_tracer *>(tracer);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_tracer_destroy(opentelemetry_tracer *tracer_) {
	try {
		auto tracer = reinterpret_cast<nostd::shared_ptr<trace::Tracer>*>(tracer_);
		if (tracer == NULL)
			return;
		delete tracer;
	} catch (...) {
	}
}

extern "C" opentelemetry_span *opentelemetry_span_start(opentelemetry_tracer *tracer_, const opentelemetry_string *name, opentelemetry_span *parent_span) {
	try {
		auto tracer = reinterpret_cast<nostd::shared_ptr<trace::Tracer>*>(tracer_);

		if (parent_span == NULL) {
			auto span = new nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len)));
			return reinterpret_cast<opentelemetry_span *>(span);
		} else {
			auto parent = reinterpret_cast<nostd::shared_ptr<trace::Span>*>(parent_span);
			trace::StartSpanOptions options = {
				common::SystemTimestamp(),
				common::SteadyTimestamp(),
				parent->get()->GetContext(),
			};
			auto span = new nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len), options));
			return reinterpret_cast<opentelemetry_span *>(span);
		}
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_span_set_attribute(opentelemetry_span *span_, const opentelemetry_attribute *attribute) {
	try {
		auto span = reinterpret_cast<nostd::shared_ptr<trace::Span>*>(span_);
		common::OpentelemetryCAttributesIterable attrs(attribute, 1);
		attrs.ForEachKeyValue(
			[&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
				span->get()->SetAttribute(key, value);
				return false;
		});
	} catch (...) {
	}
}

extern "C" void opentelemetry_span_add_event(
	opentelemetry_span *span_, const opentelemetry_string *name, const struct timespec *tp,
	opentelemetry_attribute *attributes, size_t nattributes) {
	try {
		auto span = reinterpret_cast<nostd::shared_ptr<trace::Span>*>(span_);
		common::OpentelemetryCAttributesIterable attrs(attributes, nattributes);
		if (tp == NULL) {
			span->get()->AddEvent(nostd::string_view(name->ptr, name->len), attrs);
		} else {
			uint64_t nanos = tp == NULL ? 0 : ((uint64_t)tp->tv_sec * 1000000000 + tp->tv_nsec);
			span->get()->AddEvent(
				nostd::string_view(name->ptr, name->len),
				common::SystemTimestamp(std::chrono::nanoseconds{nanos}), attrs);
		}
	} catch (...) {
	}
}

extern "C" int opentelemetry_span_headers_get(opentelemetry_span *span_, opentelemetry_header_each header_each, void *header_each_arg) {
	try {
		auto span = reinterpret_cast<nostd::shared_ptr<trace::Span>*>(span_);
		context::propagation::OpentelemetryCHeadersInjector injector(header_each, header_each_arg);
		context::Context c;
		context::Context context = trace::SetSpan(c, *span);
		trace::propagation::HttpTraceContext().Inject(injector,	context);
		return 0;
	} catch (...) {
		return -1;
	}
}

extern "C" opentelemetry_span *opentelemetry_span_start_headers(opentelemetry_tracer *tracer_, const opentelemetry_string *name, opentelemetry_header_value header_value, void *header_value_arg) {
	try {
		auto tracer = reinterpret_cast<nostd::shared_ptr<trace::Tracer>*>(tracer_);
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
		auto span = new nostd::shared_ptr<trace::Span>(tracer->get()->StartSpan(nostd::string_view(name->ptr, name->len), options));
		return reinterpret_cast<opentelemetry_span *>(span);
	} catch (...) {
		return NULL;
	}
}

extern "C" void opentelemetry_span_finish(opentelemetry_span *span_) {
	try {
		auto span = reinterpret_cast<nostd::shared_ptr<trace::Span>*>(span_);
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
