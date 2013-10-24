#include "tests_common.hpp"

TESTS_INIT()
    rbb::object symbols[] = {rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")};
    rbb::object objects[] = {rbb::number(100), rbb::number(200), rbb::number(300)};
    
    rbb::object g_object = rbb::generic_object(symbols, objects, 3);
    
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("==")).send_msg(g_object) == rbb::boolean(true),
        puts("Generic object doesn't equal itself"))
TESTS_END()