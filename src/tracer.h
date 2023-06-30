#pragma once
#include <opentelemetry/trace/tracer.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

class OpentelemetryCTracer
{
public:
    OpentelemetryCTracer(nostd::shared_ptr<Tracer> &&tracer):
        _tracer(std::move(tracer)) {}
    Tracer *get() { return _tracer.get(); }

    void LimitSpanSize(bool enable) { has_span_size_limit = enable; }
    bool HasSpanSizeLimit() { return has_span_size_limit; }

private:
    nostd::shared_ptr<Tracer> _tracer;
    bool has_span_size_limit = false;
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
