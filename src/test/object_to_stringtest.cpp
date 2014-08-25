#include "tests_common.hpp"

#include <parse.hpp>

#define TEST_STRINGFICATION(object, expected)\
    TEST_CONDITION(\
        rbb::object_to_string(object) == expected,\
        printf(\
            "The object wasn't printed correctly (expected %s, got %s)\n",\
            std::string{expected}.c_str(), rbb::object_to_string(object).c_str()))

TESTS_INIT()
    TEST_STRINGFICATION(rbb::empty(), "[]")
    TEST_STRINGFICATION(rbb::number(10), "10")
    TEST_STRINGFICATION(rbb::number(1000.12), std::to_string(1000.12))
    TEST_STRINGFICATION(rbb::symbol("valhalla"), "valhalla")
    TEST_STRINGFICATION(
        rbb::array({
            rbb::number(10),
            rbb::number(20),
            rbb::number(30)
        }),
        "|[10, 20, 30]")
    TEST_STRINGFICATION(
        rbb::table({
            rbb::symbol("a"),
            rbb::symbol("b"),
            rbb::symbol("c")
        },
        {
            rbb::number(10),
            rbb::number(20),
            rbb::number(30)
        }),
        ":[a -> 10, b -> 20, c -> 30]")
TESTS_END()
