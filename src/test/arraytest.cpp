#include "tests_common.hpp"

bool cmp_array(rbb::object &l1, rbb::object &l2)
{
    if (l1.__value.type != rbb::value_t::data_t ||
        l2.__value.type != rbb::value_t::data_t)
        return false;

    int l1_len = (l1 << rbb::symbol("*")).__value.integer();
    int l2_len = (l2 << rbb::symbol("*")).__value.integer();

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

#define TEST_SIZE(__size)\
auto size = arr << rbb::symbol("*");\
TEST_CONDITION(\
    size == rbb::number(__size),\
    printf(\
        "The array %s wasn't properly allocated\n  Expected size: %d; actual size: %s\n",\
        arr.to_string().c_str(),\
        __size,\
        size.to_string().c_str()))

#define TEST_INDEX_ELEMENT(__index, __expected)\
auto result = arr << rbb::number(__index);\
TEST_CONDITION(\
    result == (__expected),\
    printf(\
        "The array %s wasn't properly allocated\n  The element %d was expected to be %s, but it was %s\n",\
        arr.to_string().c_str(),\
        __index,\
        (__expected).to_string().c_str(),\
        result.to_string().c_str()))

#define TEST_OUT_OF_BOUNDS_INDEX(__index) \
TEST_MSG_NOT_RECOGNIZED(arr, rbb::number(__index))

TESTS_INIT()
    {
        auto arr = number(3) << objarr();
        arr << objarr(0, 10);

        TEST_SIZE(3)
        TEST_INDEX_ELEMENT(0, rbb::number(10))
    }

    {
        auto arr = number(2) << objarr(1, 2, 3);

        TEST_SIZE(2)
        TEST_OUT_OF_BOUNDS_INDEX(2)
    }

    auto l = objarr(1, 4, 9);

    TEST_CONDITION(
        l << rbb::symbol("==") << l == rbb::boolean(true),
        puts("An aray doesn't equal itself"))
    TEST_CONDITION(
        l << rbb::symbol("/=") << l == rbb::boolean(false),
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
    {
        auto &arr = l;

        TEST_OUT_OF_BOUNDS_INDEX(3)
    }

    l << objarr(0, "number_array");

    TEST_CONDITION(
        l << rbb::number(0) == rbb::symbol("number_array"),
        puts("Assigning an object to a position has no effect"))

    auto l3 = l << "+" << objarr(2, 3, 5);
    auto l4 = objarr("number_array", 4, 9, 2, 3, 5);

    TEST_CONDITION(cmp_array(l3, l4), puts("Concatenation doesn't work"));

    auto l5 = l3 << "/" << objarr(1, 3);
    auto el = objarr(4, 9, 2);

    TEST_CONDITION(cmp_array(l5, el), puts("Slicing doesn't work"))

    TEST_OPERATOR_EQ(l, l5)
    TEST_OPERATOR_EQ(l5, rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), l5)
TESTS_END()
