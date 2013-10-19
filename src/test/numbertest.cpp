#include "tests_common.hpp"

#define TEST_NUMBER_COMPARISON(num1, num2, symb, eq)\
TEST_CONDITION(\
    rbb::number(num1).send_msg(rbb::symbol(symb)).send_msg(rbb::number(num2)) == rbb::boolean(eq),\
    printf("%lf %s %lf doesn't equal %s!\n",\
           (double) num1, symb, (double) num2, eq? "true" : "false"))

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
    
    TEST_NUMBER_COMPARISON(15, 15, "!=", false)
    TEST_NUMBER_COMPARISON(11, 13, "!=", true)
    TEST_NUMBER_COMPARISON(12.1, 12.1, "!=", false)
    TEST_NUMBER_COMPARISON(11, 13.1, "!=", true)
    TEST_NUMBER_COMPARISON(12.22, 12.222, "!=", true)
    TEST_NUMBER_COMPARISON(14.0000001, 14, "!=", true)
    TEST_NUMBER_COMPARISON(13, 12, "==", false)
    TEST_NUMBER_COMPARISON(14.2, 14.2, "!=", false)
    
    TEST_NUMBER_COMPARISON(11.000001, 11, ">", true)
    TEST_NUMBER_COMPARISON(11, 12, "<", true)
    TEST_NUMBER_COMPARISON(15.3, 13, "<", false)
    TEST_NUMBER_COMPARISON(200, 200.01, ">", false)
    TEST_NUMBER_COMPARISON(25, 25, ">=", true)
    TEST_NUMBER_COMPARISON(30, 30, "<=", true)
    TEST_NUMBER_COMPARISON(12, 13, ">=", false)
    TEST_NUMBER_COMPARISON(14, 12, "<=", false)
TESTS_END()
