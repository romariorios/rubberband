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
    
    rbb::object l_ = l;
    
    TEST_CONDITION(
        l.send_msg(rbb::symbol("==")).send_msg(l_) == rbb::boolean(true),
        puts("operator= is not working well")
    )
    
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
    
    rbb::object other_numbers[] = {rbb::number(2), rbb::number(3), rbb::number(5)};
    rbb::object l2 = rbb::list(other_numbers, 3);
    rbb::object l3 = l.send_msg(rbb::symbol("+")).send_msg(l2);
    
    rbb::object all_numbers[] = {
        rbb::symbol("number_list"), rbb::number(4), rbb::number(9),
        rbb::number(2), rbb::number(3), rbb::number(5)
    };
    rbb::object l4 = rbb::list(all_numbers, 6);
    
    bool l3_eq_l4 = true;
    
    for (int i = 0; i < 6; ++i) {
        if (l3.send_msg(rbb::number(i)) == l4.send_msg(rbb::number(i))) {
        } else {
            l3_eq_l4 = false;
            break;
        }
    }
    
    TEST_CONDITION(l3_eq_l4, puts("Concatenation doesn't work"));
TESTS_END()
