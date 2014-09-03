#include "tests_common.hpp"

#include <parse.hpp>

using namespace rbb;

TESTS_INIT()
    auto empty_program = parser{""}.parse();

    TEST_CONDITION(
        empty_program << empty() << empty() == empty(),
        puts("An empty program should return empty"))

    auto return_number = parser{"!10"}.parse();
    auto result_number = return_number << empty() << empty();

    TEST_CONDITION(
        result_number == number(10),
        printf(
            "Program { !10 } returns %s\n",
            result_number.to_string().c_str()))
TESTS_END()
