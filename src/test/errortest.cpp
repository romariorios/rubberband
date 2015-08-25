#include "tests_common.hpp"
#include <rbb/error.hpp>
#include <rbb/parse.hpp>

using namespace rbb;

TESTS_INIT()
    {
        long line, column;
        try {
            dummy_master.parse(R"(
~:a -> 10, b -> 20
~:a -> 10
~:c -> ~a + 20
~: -> ~a
~:valhalla -> 300
            )");
        } catch (syntax_error e) {
            line = e.line;
            column = e.column;
        }
        
        // l 5 c 4
        TEST_CONDITION(
            line == 5 && column == 4,
            printf(
                "Syntax error in wrong location "
                "(expected line 5, column 4; got line %ld, column %ld)\n",
                line, column))
    }
TESTS_END()
