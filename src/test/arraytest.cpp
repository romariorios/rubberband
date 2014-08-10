#include "tests_common.hpp"

bool cmp_array(rbb::object &l1, rbb::object &l2)
{
    if (l1.__value.type != rbb::value_t::data_t ||
        l2.__value.type != rbb::value_t::data_t)
        return false;

    int l1_len = (l1 << rbb::symbol("?|")).__value.integer;
    int l2_len = (l2 << rbb::symbol("?|")).__value.integer;

    if (l1_len != l2_len)
        return false;

    bool l1_eq_l2 = true;
    for (int i = 0; i < l1_len; ++i) {
        if (l1 << rbb::number(i) == l2 << rbb::number(i)) {
        } else {
            l1_eq_l2 = false;
            break;
        }
    }

    return l1_eq_l2;
}

TESTS_INIT()
    auto l = rbb::array({rbb::number(1), rbb::number(4), rbb::number(9)});

    TEST_CONDITION(
        l << rbb::symbol("==") << l == rbb::boolean(true),
        puts("An aray doesn't equal itself"))
    TEST_CONDITION(
        l << rbb::symbol("!=") << l == rbb::boolean(false),
        puts("An array differs from itself"))

    auto l_ = l;

    TEST_CONDITION(
        l << rbb::symbol("==") << l_ == rbb::boolean(true),
        puts("operator= is not working well")
    )

    TEST_CONDITION(
        l << rbb::symbol("*") == rbb::number(3),
        puts("The array doesn't correctly report its size"))
    TEST_CONDITION(
        l << rbb::number(0) == rbb::number(1),
        puts("The array doesn't return its elements"))
    TEST_CONDITION(
        l << rbb::number(1) == rbb::number(4),
        puts("The array doesn't return its elements"))
    TEST_CONDITION(
        l << rbb::number(3) == rbb::empty(),
        puts("The array isn't handling out-of-bounds well"))

    auto ch_0 = rbb::array({ rbb::number(0), rbb::symbol("number_array") });

    l << ch_0;

    TEST_CONDITION(
        l << rbb::number(0) == rbb::symbol("number_array"),
        puts("Assigning an object to a position has no effect"))

    auto l2 = rbb::array({rbb::number(2), rbb::number(3), rbb::number(5)});
    auto l3 = l << rbb::symbol("+") << l2;

    auto l4 = rbb::array({
        rbb::symbol("number_array"), rbb::number(4), rbb::number(9),
        rbb::number(2), rbb::number(3), rbb::number(5)
    });

    TEST_CONDITION(cmp_array(l3, l4), puts("Concatenation doesn't work"));

    auto sb = rbb::array({rbb::number(1), rbb::number(3)});
    auto l5 = l3 << rbb::symbol("/") << sb;
    auto el = rbb::array({rbb::number(4), rbb::number(9), rbb::number(2)});

    TEST_CONDITION(cmp_array(l5, el), puts("Slicing doesn't work"))

    TEST_OPERATOR_EQ(l, l5)
    TEST_OPERATOR_EQ(l5, rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), l5)
TESTS_END()
