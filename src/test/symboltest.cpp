#include "tests_common.hpp"

TESTS_INIT()
    TEST_CONDITION(rbb::symbol("abcd") == rbb::symbol("abcd"), puts("Error when comparing equal symbols"))
    
    rbb::object abcd_comp = rbb::symbol("abcd").send_msg(rbb::symbol("==")).send_msg(rbb::symbol("abcd"));
    
    TEST_CONDITION(
        abcd_comp.__value.type == rbb::value_t::boolean_t,
        puts("Symbol comparison doesn't return a bool"))
    TEST_CONDITION(
        abcd_comp == rbb::boolean(true),
        puts("Error when comparing equal symbols sending messages"))
    TEST_CONDITION(
        rbb::symbol("a12345").send_msg(rbb::symbol("!=")).send_msg(rbb::symbol("a1234")) == rbb::boolean(true),
        puts("Error when cheking two symbols for inequality"))
TESTS_END()
