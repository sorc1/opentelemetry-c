#pragma once
#include <opentelemetry/sdk/trace/sampler.h>
#include <opentelemetry-c/common.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

static inline void opentelemetry_fill_sampling_result(const opentelemetry_sampling_result *from, SamplingResult *to) {
	switch (from->decision) {
	case OPENTELEMETRY_SAMPLING_DESISION_RECORD_ONLY:
		to->decision = Decision::RECORD_ONLY; break;
	case OPENTELEMETRY_SAMPLING_DESISION_RECORD_AND_SAMPLE:
		to->decision = Decision::RECORD_AND_SAMPLE; break;
	default:
		to->decision = Decision::DROP; break;
	}

	to->attributes = nullptr;

	if (from->ts == NULL)
		to->trace_state = nullptr;
	else {
		to->trace_state = *reinterpret_cast<nostd::shared_ptr<opentelemetry::trace::TraceState> *>(from->ts);
		opentelemetry_trace_state_destroy(from->ts);
	}
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
