#pragma once
#include "sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class OpentelemetryCParentRootSampler : public Sampler
{
public:
	OpentelemetryCParentRootSampler(opentelemetry_sampler_parent_root_cb cb, void *arg):
		_cb(cb), _cb_arg(arg) { }

	inline SamplingResult ShouldSample(
		const opentelemetry::trace::SpanContext &parent_context,
		opentelemetry::trace::TraceId /*trace_id*/,
		nostd::string_view /*name*/,
		opentelemetry::trace::SpanKind /*span_kind*/,
		const opentelemetry::common::KeyValueIterable & /*attributes*/,
		const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/) noexcept override
	{
		if (parent_context.IsValid()) {
			if (parent_context.IsSampled())
				return {Decision::RECORD_AND_SAMPLE, nullptr, parent_context.trace_state()};
			return {Decision::DROP, nullptr, parent_context.trace_state()};
		}

		opentelemetry_sampling_result c_result;
		if (!_cb(&c_result, _cb_arg))
			return {Decision::DROP, nullptr, opentelemetry::trace::TraceState::GetDefault()};

		SamplingResult result;
		opentelemetry_fill_sampling_result(&c_result, &result);

		return result;
	}

	inline nostd::string_view GetDescription() const noexcept override { return "ParentRootSampler"; }

private:
    opentelemetry_sampler_parent_root_cb _cb;
    void *_cb_arg;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
