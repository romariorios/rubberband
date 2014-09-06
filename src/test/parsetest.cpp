#include "tests_common.hpp"

#include <parse.hpp>
#include <string>

using namespace rbb;

#define TEST_PROGRAM(program, context, message, expected)\
{\
    auto prog = parser{program}.parse();\
    auto res = prog << (context) << (message);\
\
    TEST_CONDITION(\
        res == (expected),\
        printf(\
            "The program { %s } (interpreted as %s) returns %s (expected: %s)\n",\
            program,\
            prog.to_string().c_str(),\
            res.to_string().c_str(),\
            expected.to_string().c_str()))\
}

TESTS_INIT()
    TEST_PROGRAM("", empty(), empty(), empty())
    TEST_PROGRAM("!10", empty(), empty(), number(10))
    TEST_PROGRAM("!10 ", empty(), empty(), number(10))
TESTS_END()
