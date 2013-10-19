#include "tests_common.hpp"

TESTS_INIT()
    TEST_CONDITION(rbb::symbol("abcd") == rbb::symbol("abcd"), puts("Error when comparing equal symbols"))
    TEST_CONDITION(
        rbb::symbol("abcd").send_msg(rbb::symbol("==")).send_msg(rbb::symbol("abcd")) == rbb::boolean(true),
        puts("Error when comparing equal symbols sending messages"))
TESTS_END()
