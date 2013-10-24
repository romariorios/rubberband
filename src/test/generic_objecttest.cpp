#include "tests_common.hpp"

TESTS_INIT()
    rbb::object symbols[] = {rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")};
    rbb::object objects[] = {rbb::number(100), rbb::number(200), rbb::number(300)};
    
    rbb::object g_object = rbb::generic_object(symbols, objects, 3);
    
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("==")).send_msg(g_object) == rbb::boolean(true),
        puts("Generic object doesn't equal itself"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("!=")).send_msg(g_object) == rbb::boolean(false),
        puts("g_object != g_object doesn't equal false"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("a")) == rbb::number(100),
        puts("Can't find field a"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("b")) == rbb::number(200),
        puts("Can't find field b"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("c")) == rbb::number(300),
        puts("Can't find field c"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("d")) == rbb::empty(),
        puts("Generic object should answer empty for a non-existing field"))
    TEST_CONDITION(
        g_object.send_msg(rbb::number(100)) == rbb::empty(),
        puts("Generic object should answer empty for invalid field names (number 100)"))
TESTS_END()