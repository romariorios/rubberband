#include "tests_common.hpp"

#include <tokenizer.hpp>
#include <vector>

using namespace rbb;

TESTS_INIT()
    {
        auto tok = tokenizer{"{ !10 }"};
        auto expected = std::vector<token>{
            token{token::type_e::curly_open},
            token{token::type_e::exclamation},
            token::number(10),
            token{token::type_e::curly_close}
        };

        TEST_CONDITION(
            tok.look_all() == expected,
            puts("Unexpected tokenization"))

        TEST_CONDITION(
            tok.look_next() == expected[0],
            puts("First token by look_next() isn't {"))

        TEST_CONDITION(
            tok.next() == expected[0],
            puts("First token by next() isn't {"))

        TEST_CONDITION(
            tok.look_next() == expected[1],
            puts("Next token by look_next() isn't !"))

        TEST_CONDITION(
            tok.next() == expected[1],
            puts("Next token by next() isn't !"))

        TEST_CONDITION(
            tok.look_next() == expected[2],
            puts("Next token by look_next() isn't 10"))

        TEST_CONDITION(
            tok.next() == expected[2],
            puts("Next token by next() isn't 10"))
    }
TESTS_END()
