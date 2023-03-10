#ifndef OPENTELEMETRY_TESTS_UTILS_H_
#define OPENTELEMETRY_TESTS_UTILS_H_

#include <opentelemetry-c/common.h>

const char *test_jaeger_endpoint(void);
uint16_t test_jaeger_port(void);
int test_span(opentelemetry_processor *processor);

#endif /* OPENTELEMETRY_TESTS_UTILS_H_ */
