#ifndef OPENTELEMETRY_TESTS_UTILS_H_
#define OPENTELEMETRY_TESTS_UTILS_H_

#include <opentelemetry-c/common.h>

const char *test_jaeger_endpoint(void);
uint16_t test_jaeger_port(void);
int test_span(opentelemetry_processor *processor);
char *create_test_string(size_t size);
bool *create_test_ab(size_t len);
int32_t *create_test_ai32(size_t len);
int64_t *create_test_ai64(size_t len);
double *create_test_ad(size_t len);

#endif /* OPENTELEMETRY_TESTS_UTILS_H_ */
