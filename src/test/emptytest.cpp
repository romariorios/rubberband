#include "tests_common.hpp"

TESTS_INIT()
    rbb::object empty_equals_empty = rbb::empty() << rbb::symbol("==") << rbb::empty();

    TEST_CONDITION(
        empty_equals_empty.__value.type == rbb::value_t::boolean_t,
        printf("\"Empty equals empty\" isn't a boolean (Actual value: %d)\n", empty_equals_empty.__value.type))
    TEST_CONDITION(
        empty_equals_empty == rbb::boolean(true), puts("Empty doesn't equal empty"))

    rbb::object empty_differs_empty = rbb::empty() << rbb::symbol("!=") << rbb::empty();

    TEST_CONDITION(
        empty_differs_empty == rbb::boolean(false),
        puts("\"Empty differs empty\" equals true"))
    TEST_CONDITION(
        rbb::empty() << rbb::symbol("==") << rbb::symbol("duvnis") == rbb::boolean(false),
        puts("Empty equals a random symbol"))
TESTS_END()
