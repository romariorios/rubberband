#include "tests_common.hpp"

#include <block.hpp>
#include <parse.hpp>

#define TEST_STRINGFICATION(object, expected)\
    TEST_CONDITION(\
        object.to_string() == expected,\
        printf(\
            "The object wasn't printed correctly (expected %s, got %s)\n",\
            std::string{expected}.c_str(), object.to_string().c_str()))

TESTS_INIT()
    TEST_STRINGFICATION(rbb::empty(), "[]")
    TEST_STRINGFICATION(rbb::number(10), "10")
    TEST_STRINGFICATION(rbb::number(1000.12), std::to_string(1000.12))
    TEST_STRINGFICATION(rbb::symbol("valhalla"), "valhalla")
    TEST_STRINGFICATION(rbb::boolean(true), "[T]")
    TEST_STRINGFICATION(rbb::boolean(false), "[F]")
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
    {
        rbb::literal::block bll;
        bll.return_statement().add_expr<rbb::literal::number>(10);
        
        TEST_STRINGFICATION(bll.eval(), "{ !10 }")
    }
TESTS_END()
