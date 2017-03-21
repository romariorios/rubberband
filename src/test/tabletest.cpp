#include "tests_common.hpp"

#include <rbb/error.hpp>
#include <vector>

using namespace rbb;

bool array_contains(rbb::object l, rbb::object obj)
{
    int size = (l << rbb::symbol("*")).__value.integer();

    for (int i = 0; i < size; ++i) {
        if (l << rbb::number(i) == obj)
            return true;
    }

    return false;
}

bool arrays_have_same_elements(rbb::object l1, rbb::object l2)
{
    int size1 = (l1 << rbb::symbol("*")).__value.integer();
    int size2 = (l2 << rbb::symbol("*")).__value.integer();

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
   auto tab = rbb::table(
        {rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")},
        {rbb::number(100), rbb::number(200), rbb::number(300)});

    TEST_CONDITION(
        tab << rbb::symbol("==") << tab == rbb::boolean(true),
        puts("Table doesn't equal itself"))
    TEST_CONDITION(
        tab << rbb::symbol("/=") << tab == rbb::boolean(false),
        puts("table /= table doesn't equal false"))
    TEST_CONDITION(
        tab << rbb::symbol("a") == rbb::number(100),
        puts("Can't find field a"))
    TEST_CONDITION(
        tab << rbb::symbol("b") == rbb::number(200),
        puts("Can't find field b"))
    TEST_CONDITION(
        tab << rbb::symbol("c") == rbb::number(300),
        puts("Can't find field c"))
    TEST_MSG_NOT_RECOGNIZED(tab, symbol("d"))
    bool thrown = false;
    try {
        tab << rbb::number(100);
    } catch (rbb::message_not_recognized_error) {
        thrown = true;
    }
    TEST_CONDITION(
        thrown,
        puts("Table should raise exception for invalid field names (number 100)"))

    auto fields = rbb::array({rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c")});

    TEST_CONDITION(
        arrays_have_same_elements(tab << rbb::symbol("*"), fields),
        puts("The object doesn't inform its fields"))

    auto table2 = rbb::table(
        {rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")},
        {rbb::number(400), rbb::number(500), rbb::number(600)});

    std::vector<rbb::object> fields_result_a {
        rbb::symbol("a"), rbb::symbol("b"), rbb::symbol("c"),
        rbb::symbol("d"), rbb::symbol("e"), rbb::symbol("f")
    };

    TEST_CONDITION(
        merging_works(tab, table2, fields_result_a),
        puts("Merging doesn't work"))

    auto array1 = rbb::array({rbb::number(400), rbb::number(500), rbb::number(600)});
    auto table3 = rbb::table(
        {rbb::symbol("c"), rbb::symbol("d")},
        {rbb::number(150), array1});

    TEST_CONDITION(
        merging_works(tab, table3, fields_result_a),
        puts("Replacing fields is not working properly"))
    TEST_CONDITION(
        tab << rbb::symbol("c") == rbb::number(150),
        puts("Fields are not being properly replaced"))
    TEST_CONDITION(
        arrays_have_same_elements(tab << rbb::symbol("d"), array1),
        puts("Fields are not being properly replaced"))

    auto table4 = rbb::table(
        {rbb::symbol("g"), rbb::symbol("h")},
        {rbb::boolean(true), rbb::boolean(false)});
    auto table5 = tab << rbb::symbol("+") << table4;

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

    auto tabl4 = table(
        {symbol("a"), symbol("b"), symbol("c")},
        {number(10), number(20), number(30)});
    tabl4 << symbol("-") << symbol("c");

    TEST_CONDITION(
        tabl4 << symbol("*") << symbol("*") == number(2),
        printf(
            "Object at index c wasn't deleted (table is now %s)\n",
            tabl4.to_string().c_str()))

    {
        auto atable = table(
            {symbol("a"), symbol("b"), symbol("c")},
            {number(10), number(20), number(30)}
        );

        auto result = atable << rbb::array({symbol("a"), symbol("b")});
        TEST_CONDITION(
            result << symbol("*") << symbol("*") == number(2) &&
            result << symbol("a") == number(10) &&
            result << symbol("b") == number(20),
            puts("Can't get table slice"))
    }
TESTS_END()
