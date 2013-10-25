#include "tests_common.hpp"

bool list_contains(rbb::object l, rbb::object obj)
{
    int size = l.send_msg(rbb::symbol("?|")).__value.integer;
    
    for (int i = 0; i < size; ++i) {
        if (l.send_msg(rbb::number(i)) == obj)
            return true;
    }
    
    return false;
}

bool lists_have_same_elements(rbb::object l1, rbb::object l2)
{
    int size = l1.send_msg(rbb::symbol("?|")).__value.integer;
    
    for (int i = 0; i < size; ++i) {
        if (list_contains(l2, l1.send_msg(rbb::number(i))))
            return true;
    }
    
    return false;
}

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
    
    rbb::object fields = rbb::list(symbols, 3);
    
    TEST_CONDITION(
        lists_have_same_elements(g_object.send_msg(rbb::symbol("?:")), fields),
        puts("The object doesn't inform its fields"))
TESTS_END()