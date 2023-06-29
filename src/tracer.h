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

private:
    nostd::shared_ptr<Tracer> _tracer;
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
