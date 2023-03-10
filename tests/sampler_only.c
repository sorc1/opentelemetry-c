#include <opentelemetry-c/common.h>

#include <stddef.h>

int main(void) {
	opentelemetry_sampler *always_on = opentelemetry_sampler_always_on();
	if (always_on == NULL)
		return 1;
	opentelemetry_sampler *always_off = opentelemetry_sampler_always_off();
	if (always_off == NULL)
		return 1;
	opentelemetry_sampler *ratio = opentelemetry_sampler_trace_id_ratio(0.5);
	if (ratio == NULL)
		return 1;
	opentelemetry_sampler *parent = opentelemetry_sampler_parent(ratio);
	if (ratio == NULL)
		return 1;
	opentelemetry_sampler_destroy(always_on);
	opentelemetry_sampler_destroy(always_off);
	opentelemetry_sampler_destroy(parent);
	/*
	 * we shouldn't destroy ratio sampler as it was controlled by parent
	 * sampler and has been already destoyed
	 */
	return 0;
}
