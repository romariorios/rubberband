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
    TEST_CONDITION(
        l.send_msg(rbb::symbol("?|")) == rbb::number(3),
        puts("The list doesn't correctly report its size"))
    TEST_CONDITION(
        l.send_msg(rbb::number(0)) == rbb::number(1),
        puts("The list doesn't return its elements"))
    TEST_CONDITION(
        l.send_msg(rbb::number(1)) == rbb::number(4),
        puts("The list doesn't return its elements"))
    TEST_CONDITION(
        l.send_msg(rbb::number(3)) == rbb::empty(),
        puts("The list isn't handling out-of-bounds well"))
    
    rbb::object ch_0_arr[] = { rbb::number(0), rbb::symbol("number_list") };
    rbb::object ch_0 = rbb::list(ch_0_arr, 2);
    
    l.send_msg(ch_0);
    
    TEST_CONDITION(
        l.send_msg(rbb::number(0)) == rbb::symbol("number_list"),
        puts("Assigning an object to a position has no effect"))
TESTS_END()
