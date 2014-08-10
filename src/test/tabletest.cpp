#include "tests_common.hpp"

#include <vector>

bool array_contains(rbb::object l, rbb::object obj)
{
    int size = (l << rbb::symbol("*")).__value.integer;

    for (int i = 0; i < size; ++i) {
        if (l << rbb::number(i) == obj)
            return true;
    }

    return false;
}

bool arrays_have_same_elements(rbb::object l1, rbb::object l2)
{
    int size1 = (l1 << rbb::symbol("*")).__value.integer;
    int size2 = (l2 << rbb::symbol("*")).__value.integer;

    if (size1 != size2)
        return false;

    for (int i = 0; i < size1; ++i) {
        if (array_contains(l2, l1 << rbb::number(i)))
            return true;
    }

    return false;
}

bool merging_works(rbb::object go1, rbb::object go2, const std::vector<rbb::object> &result)
{
    go1 << go2;

    rbb::object result_array = rbb::array(result);

    return arrays_have_same_elements(go1 << rbb::symbol("*"), result_array);
}

TESTS_INIT()
   auto table = rbb::table(
        {rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")},
        {rbb::number(100), rbb::number(200), rbb::number(300)});

    TEST_CONDITION(
        table << rbb::symbol("==") << table == rbb::boolean(true),
        puts("Table doesn't equal itself"))
    TEST_CONDITION(
        table << rbb::symbol("!=") << table == rbb::boolean(false),
        puts("table != table doesn't equal false"))
    TEST_CONDITION(
        table << rbb::symbol("a") == rbb::number(100),
        puts("Can't find field a"))
    TEST_CONDITION(
        table << rbb::symbol("b") == rbb::number(200),
        puts("Can't find field b"))
    TEST_CONDITION(
        table << rbb::symbol("c") == rbb::number(300),
        puts("Can't find field c"))
    TEST_CONDITION(
        table << rbb::symbol("d") == rbb::empty(),
        puts("Generic object should answer empty for a non-existing field"))
    TEST_CONDITION(
        table << rbb::number(100) == rbb::empty(),
        puts("Generic object should answer empty for invalid field names (number 100)"))

    auto fields = rbb::array({rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")});

    TEST_CONDITION(
        arrays_have_same_elements(table << rbb::symbol("*"), fields),
        puts("The object doesn't inform its fields"))

    auto table2 = rbb::table(
        {rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")},
        {rbb::number(400), rbb::number(500), rbb::number(600)});

    std::vector<rbb::object> fields_result_a {
        rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c"),
        rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")
    };

    TEST_CONDITION(
        merging_works(table, table2, fields_result_a),
        puts("Merging doesn't work"))

    auto array1 = rbb::array({rbb::number(400), rbb::number(500), rbb::number(600)});
    auto table3 = rbb::table(
        {rbb::symbol("c"), rbb::symbol("d")},
        {rbb::number(150), array1});

    TEST_CONDITION(
        merging_works(table, table3, fields_result_a),
        puts("Replacing fields is not working properly"))
    TEST_CONDITION(
        table << rbb::symbol("c") == rbb::number(150),
        puts("Fields are not being properly replaced"))
    TEST_CONDITION(
        arrays_have_same_elements(table << rbb::symbol("d"), array1),
        puts("Fields are not being properly replaced"))

    auto table4 = rbb::table(
        {rbb::symbol("g"), rbb::symbol("h")},
        {rbb::boolean(true), rbb::boolean(false)});
    auto table5 = table << rbb::symbol("+") << table4;

    auto result2 = rbb::array({
        rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c"),
        rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f"),
        rbb::symbol("g"), rbb::symbol("h")
    });

    TEST_CONDITION(
        arrays_have_same_elements(result2, table5 << rbb::symbol("*")),
        puts("Merging doesn't work"))

    auto tabl1 = rbb::table(
        { rbb::symbol("hahaha") },
        { rbb::number(15) });

    auto tabl2 = rbb::table(
        { rbb::symbol("hehehe") },
        { rbb::number(30) });

    auto tabl3 = tabl1 << tabl2;

    TEST_CONDITION(
        tabl1 == tabl3,
        puts("Object merging doesn't return self"))
TESTS_END()
