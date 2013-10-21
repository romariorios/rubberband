#include "tests_common.hpp"

TESTS_INIT()
    rbb::object numbers[] = {rbb::number(1), rbb::number(4), rbb::number(9)};
    rbb::object l = rbb::list(numbers, 3);

    TEST_CONDITION(
        l.send_msg(rbb::symbol("==")).send_msg(l) == rbb::boolean(true),
        puts("A list doesn't equal itself"))
    TEST_CONDITION(
        l.send_msg(rbb::symbol("!=")).send_msg(l) == rbb::boolean(false),
        puts("A list differs from itself"))
TESTS_END()
