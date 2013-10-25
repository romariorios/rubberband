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
    int size1 = l1.send_msg(rbb::symbol("?|")).__value.integer;
    int size2 = l2.send_msg(rbb::symbol("?|")).__value.integer;
    
    if (size1 != size2)
        return false;
    
    for (int i = 0; i < size1; ++i) {
        if (list_contains(l2, l1.send_msg(rbb::number(i))))
            return true;
    }
    
    return false;
}

bool merging_works(rbb::object go1, rbb::object go2, rbb::object *result, int resultsize)
{
    go1.send_msg(go2);
    
    rbb::object result_list = rbb::list(result, resultsize);
    
    return lists_have_same_elements(go1.send_msg(rbb::symbol("?:")), result_list);
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
    
    rbb::object fields2[] = {rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")};
    rbb::object objects2[] = {rbb::number(400), rbb::number(500), rbb::number(600)};
    
    rbb::object g_object2 = rbb::generic_object(fields2, objects2, 3);
    
    rbb::object fields_result_a[] = {
        rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c"),
        rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")
    };
    
    TEST_CONDITION(
        merging_works(g_object, g_object2, fields_result_a, 6),
        puts("Merging doesn't work"))
    
    rbb::object fields3[] = {rbb::symbol("c"), rbb::symbol("d")};
    rbb::object objects3[] = {rbb::number(150), rbb::list(objects2, 3)};
    
    rbb::object g_object3 = rbb::generic_object(fields3, objects3, 2);
    
    TEST_CONDITION(
        merging_works(g_object, g_object3, fields_result_a, 6),
        puts("Replacing fields is not working properly"))
    TEST_CONDITION(
        g_object.send_msg(rbb::symbol("c")) == rbb::number(150),
        puts("Fields are not being properly replaced"))
    TEST_CONDITION(
        lists_have_same_elements(g_object.send_msg(rbb::symbol("d")), rbb::list(objects2, 3)),
        puts("Fields are not being properly replaced"))
    
    
TESTS_END()