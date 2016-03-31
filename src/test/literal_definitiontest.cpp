#include "tests_common.hpp"

TESTS_INIT()
    {
        auto define_literal = dummy_master.parse(R"(
        {
            %*:
                > -> 39,     # ASCII for '
                = -> { !12 }
        }
        )");
        define_literal << empty() << empty();

        auto literal_value = dummy_master.parse("'");
        TEST_CONDITION(
            literal_value == number(12),
            printf(
                "Error trying to parse user-defined literal ' (expected value: %s, got: %s)\n",
                number(12).to_string().c_str(),
                literal_value.to_string().c_str()))

        auto table_with_literals = dummy_master.parse(":a -> ', b -> ' + 10");
        TEST_CONDITION(
            table_with_literals << symbol("a") == number(12) &&
            table_with_literals << symbol("b") == number(22),
            printf(
                "Error: unexpected parsed value %s",
                table_with_literals.to_string().c_str()))
    }
TESTS_END()
