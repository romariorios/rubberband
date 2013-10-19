#include "tests_common.hpp"

TESTS_INIT()
    TEST_CONDITION(
        rbb::number(15).__value.type & rbb::value_t::number_t,
        puts("rbb::number doesn't generate a number"))
    TEST_CONDITION(
        rbb::number(15).__value.type == rbb::value_t::integer_t,
        puts("Type isn't integer but argument was"))
    TEST_CONDITION(
        rbb::number(12.4).__value.type == rbb::value_t::floating_t,
        puts("Type isn't floating but argument was"))
    TEST_CONDITION(
        rbb::number(15).__value.integer == 15,
        puts("rbb::number doesn't initialize integer numbers correctly"))
TESTS_END()
