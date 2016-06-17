#include "tests_common.hpp"

#define TEST_VALUE_PARSING(__string, __value) \
{\
    auto parsed_value = dummy_master.parse(__string);\
    TEST_CONDITION(\
        parsed_value == (__value),\
        printf(\
            "Error trying to parse %s (expected %s, got %s)",\
            __string,\
            (__value).to_string().c_str(),\
            parsed_value.to_string().c_str()))\
}

TESTS_INIT()
    {
        auto define_literal = dummy_master.parse(R"(
        {
            %+:
                > -> 39,     # ASCII for '
                = -> { !12 }
        }
        )");
        define_literal << empty() << empty();

        TEST_VALUE_PARSING("'", number(12))

        auto table_with_literals = dummy_master.parse(":a -> ', b -> ' + 10");
        TEST_CONDITION(
            table_with_literals << symbol("a") == number(12) &&
            table_with_literals << symbol("b") == number(22),
            printf(
                "Error: unexpected parsed value %s",
                table_with_literals.to_string().c_str()))
    }

    {
        auto define_char_literal = dummy_master.parse(R"(
        {
            %+:
                > -> 39,           # ASCII for '
                = -> { ~> !~[@] }  # ~>: go to next char; ~[@]: current char
        }
        )");
        define_char_literal << empty() << empty();

        TEST_VALUE_PARSING("'c", number('c'))
    }
TESTS_END()
