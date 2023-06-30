#pragma once
#include <opentelemetry/trace/span.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

class OpentelemetryCSpan
{
public:
    OpentelemetryCSpan(nostd::shared_ptr<Span> &&span):
        _span(std::move(span)) {}
    Span *get() { return _span.get(); }
    nostd::shared_ptr<Span> get_shared() { return _span; }

private:
    nostd::shared_ptr<Span> _span;
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
