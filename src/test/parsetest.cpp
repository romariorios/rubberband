#include "tests_common.hpp"

#include <parse.hpp>

TESTS_INIT()
    auto empty_program = rbb::parse("");
    
    TEST_CONDITION(
        empty_program << rbb::empty() << rbb::empty() == rbb::empty(),
        puts("An empty program should return empty"))
    
    auto return_number = rbb::parse("!10");
    auto result_number = return_number << rbb::empty() << rbb::empty();
    
    TEST_CONDITION(
        result_number == rbb::number(10),
        printf(
            "Program { !10 } returns %s\n",
            rbb::object_to_string(result_number).c_str()))
TESTS_END()