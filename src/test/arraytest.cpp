#include "tests_common.hpp"

bool cmp_array(rbb::object &l1, rbb::object &l2)
{
    if (l1.__value.type != rbb::value_t::data_t ||
        l2.__value.type != rbb::value_t::data_t)
        return false;
    
    int l1_len = l1.send_msg(rbb::symbol("?|")).__value.integer;
    int l2_len = l2.send_msg(rbb::symbol("?|")).__value.integer;
    
    if (l1_len != l2_len)
        return false;
    
    bool l1_eq_l2 = true;
    for (int i = 0; i < l1_len; ++i) {
        if (l1.send_msg(rbb::number(i)) == l2.send_msg(rbb::number(i))) {
        } else {
            l1_eq_l2 = false;
            break;
        }
    }
    
    return l1_eq_l2;
}

TESTS_INIT()
    rbb::object numbers[] = {rbb::number(1), rbb::number(4), rbb::number(9)};
    rbb::object l = rbb::array(numbers, 3);

    TEST_CONDITION(
        l.send_msg(rbb::symbol("==")).send_msg(l) == rbb::boolean(true),
        puts("An aray doesn't equal itself"))
    TEST_CONDITION(
        l.send_msg(rbb::symbol("!=")).send_msg(l) == rbb::boolean(false),
        puts("An array differs from itself"))
    
    rbb::object l_ = l;
    
    TEST_CONDITION(
        l.send_msg(rbb::symbol("==")).send_msg(l_) == rbb::boolean(true),
        puts("operator= is not working well")
    )
    
    TEST_CONDITION(
        l.send_msg(rbb::symbol("?|")) == rbb::number(3),
        puts("The array doesn't correctly report its size"))
    TEST_CONDITION(
        l.send_msg(rbb::number(0)) == rbb::number(1),
        puts("The array doesn't return its elements"))
    TEST_CONDITION(
        l.send_msg(rbb::number(1)) == rbb::number(4),
        puts("The array doesn't return its elements"))
    TEST_CONDITION(
        l.send_msg(rbb::number(3)) == rbb::empty(),
        puts("The array isn't handling out-of-bounds well"))
    
    rbb::object ch_0_arr[] = { rbb::number(0), rbb::symbol("number_array") };
    rbb::object ch_0 = rbb::array(ch_0_arr, 2);
    
    l.send_msg(ch_0);
    
    TEST_CONDITION(
        l.send_msg(rbb::number(0)) == rbb::symbol("number_array"),
        puts("Assigning an object to a position has no effect"))
    
    rbb::object other_numbers[] = {rbb::number(2), rbb::number(3), rbb::number(5)};
    rbb::object l2 = rbb::array(other_numbers, 3);
    rbb::object l3 = l.send_msg(rbb::symbol("+")).send_msg(l2);
    
    rbb::object all_numbers[] = {
        rbb::symbol("number_array"), rbb::number(4), rbb::number(9),
        rbb::number(2), rbb::number(3), rbb::number(5)
    };
    rbb::object l4 = rbb::array(all_numbers, 6);
    
    TEST_CONDITION(cmp_array(l3, l4), puts("Concatenation doesn't work"));
    
    rbb::object slice_by[] = {rbb::number(1), rbb::number(3)};
    rbb::object sb = rbb::array(slice_by, 2);
    
    rbb::object l5 = l3.send_msg(rbb::symbol("/")).send_msg(sb);
    
    rbb::object expected_array[] = {rbb::number(4), rbb::number(9), rbb::number(2)};
    rbb::object el = rbb::array(expected_array, 3);
    
    TEST_CONDITION(cmp_array(l5, el), puts("Slicing doesn't work"))
    
    TEST_OPERATOR_EQ(l, l5)
    TEST_OPERATOR_EQ(l5, rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), l5)
TESTS_END()
