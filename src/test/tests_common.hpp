#ifndef TESTS_COMMON
#define TESTS_COMMON

#include <cstdio>

#include "../core/object.hpp"

#define TESTS_INIT()\
int errors = 0;\
int tests = 0;

#define TEST_CONDITION(condition, message)\
++tests;\
if (!(condition)) {\
    ++errors;\
    message;\
}

#define TESTS_END()\
float error_rate = ((float) errors / (float) tests);\
error_rate  *= 100;\
printf("Result: %d tests, %d failures (%.0f%%).\n", tests, errors, error_rate);

#endif // TESTS_COMMON
