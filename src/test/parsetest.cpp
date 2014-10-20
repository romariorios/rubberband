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
    TEST_PROGRAM("!|[1, 2, a] 0", empty(), empty(), number(1))
    TEST_PROGRAM("!|[1, 2, a] 1", empty(), empty(), number(2))
    TEST_PROGRAM("!|[1, 2, a] 2", empty(), empty(), symbol("a"))
    TEST_PROGRAM("!|[1, 2, a] 3", empty(), empty(), empty())
    TEST_PROGRAM("!:[a -> 10, b -> 20] a", empty(), empty(), number(10))
    TEST_PROGRAM("!:[a -> 10, b -> 20] b", empty(), empty(), number(20))
    TEST_PROGRAM("!:[a -> 10, b -> 20] lol", empty(), empty(), empty())
    TEST_PROGRAM("!$", empty(), number(10), number(10));
    TEST_PROGRAM("!~", number(10), empty(), number(10));
    TEST_PROGRAM("~:x -> $ !~x * $", table({}, {}), number(10), number(100))
    TEST_PROGRAM("~:x -> $ !~x * (~x)", table({}, {}), number(10), number(100))
    TEST_PROGRAM(R"(
        ~:fibnums -> |[0, 1, 1]
        ~:n -> $

        !~n <= 2?~ {
            !~fibnums(~i)
        } {
            ~:i -> 2

            ~:loop -> {
                ~fibnums|0, ~fibnums 1
                ~fibnums|1, ~fibnums 2
                ~fibnums|2, ~fibnums 0 + (~fibnums 1)

                ~:i -> ~i + 1
                ~i < (~n)?~ {
                    ~loop[]
                }
            }~
            ~loop[]
            
            !~fibnums 2
        }
    )", table({}, {}), number(43), number(433494437))
TESTS_END()
