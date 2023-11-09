#pragma once
#include <opentelemetry/trace/span.h>
#include "tracer.h"

#define SPAN_LOG_SPLIT 60000
#define SPAN_LOG_NAME_LEN_MAX 3000

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

class OpentelemetryCSpan
{
public:
    OpentelemetryCSpan(OpentelemetryCTracer *tracer, nostd::shared_ptr<Span> &&span):
        _tracer(tracer), _span(std::move(span)), context(_span->GetContext()) { }
    Span *get() { return _span.get(); }
    trace::SpanContext get_context() { return context; }
    nostd::shared_ptr<Span> get_shared() { return _span; }

    bool HasSpanSizeLimit() { return _tracer->HasSpanSizeLimit(); }
    uint64_t SpanSizeAvailable() { return SPAN_LOG_SPLIT > stored ? SPAN_LOG_SPLIT - stored : 0; }
    void StoreSize(uint64_t size) { stored += size; }
    void Reopen() {
		static const nostd::string_view span_chunk_key = "_continue_";

        trace::StartSpanOptions options = {
            common::SystemTimestamp(),
            common::SteadyTimestamp(),
            context,
        };
        _span = nostd::shared_ptr<trace::Span>(_tracer->get()->StartSpan(span_chunk_key, options));
        stored = 0;
    }

private:
    OpentelemetryCTracer *_tracer;
    nostd::shared_ptr<Span> _span;
    trace::SpanContext context;

    uint64_t stored = 0;
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
