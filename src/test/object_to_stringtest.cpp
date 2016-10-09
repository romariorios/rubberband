#include "tests_common.hpp"

#include <rbb/block.hpp>

#define TEST_STRINGFICATION(object, expected)\
    TEST_CONDITION(\
        object.to_string() == expected,\
        printf(\
            "The object wasn't printed correctly (expected %s, got %s)\n",\
            std::string{expected}.c_str(), object.to_string().c_str()))

TESTS_INIT()
    TEST_STRINGFICATION(rbb::empty(), "()")
    TEST_STRINGFICATION(rbb::number(10), "10")
    TEST_STRINGFICATION(rbb::number(1000.12), std::to_string(1000.12))
    TEST_STRINGFICATION(rbb::symbol("valhalla"), "valhalla")
    TEST_STRINGFICATION(rbb::boolean(true), "?1")
    TEST_STRINGFICATION(rbb::boolean(false), "?0")
    TEST_STRINGFICATION(
        rbb::array({
            rbb::number(10),
            rbb::number(20),
            rbb::number(30)
        }),
        "(|10, 20, 30)")
    TEST_STRINGFICATION(
        rbb::table({
            rbb::symbol("a"),
            rbb::symbol("b"),
            rbb::symbol("c")
        },
        {
            rbb::number(10),
            rbb::number(20),
            rbb::number(30)
        }),
        "(:a -> 10, b -> 20, c -> 30)")
    {
        rbb::literal::block bll;
        bll.return_statement().add_expr<rbb::literal::number>(10);

        TEST_STRINGFICATION(bll.eval(), "{ !10 }")
    }

    {
        rbb::literal::block bll;
        auto &ret = bll.return_statement();
        ret.add_expr<rbb::literal::context>();
        ret.add_expr<rbb::literal::symbol>("x");
        ret.add_expr<rbb::literal::symbol>("+");
        auto &sub_statement = ret.add_expr<rbb::block_statement>();
        sub_statement.add_expr<rbb::literal::context>();
        sub_statement.add_expr<rbb::literal::symbol>("x");

        TEST_STRINGFICATION(bll.eval(), "{ !~ x + (~ x) }")
    }

    {
        rbb::literal::block bll;

        auto &stm1 = bll.add_statement();
        stm1.add_expr<rbb::literal::context>();
        auto &table1 = stm1.add_expr<rbb::literal::table>();
        table1.add_symbol<rbb::literal::symbol>("a");
        table1.add_object<rbb::literal::number>(10);

        auto &stm2 = bll.add_statement();
        stm2.add_expr<rbb::literal::context>();
        auto &table2 = stm2.add_expr<rbb::literal::table>();
        table2.add_symbol<rbb::literal::symbol>("b");
        table2.add_object<rbb::literal::number>(20);

        auto &stm3 = bll.add_statement();
        stm3.add_expr<rbb::literal::context>();
        auto &table3 = stm3.add_expr<rbb::literal::table>();
        table3.add_symbol<rbb::literal::symbol>("c");
        auto &array1 = table3.add_object<rbb::literal::array>();
        array1.add_element<rbb::literal::symbol>("thirty");
        array1.add_element<rbb::literal::symbol>("trinta");
        array1.add_element<rbb::literal::number>(30);

        TEST_STRINGFICATION(
            bll.eval(),
            "{ ~ (:a -> 10); ~ (:b -> 20); ~ (:c -> (|thirty, trinta, 30)) }"
        )
    }

    TEST_STRINGFICATION(token::boolean(true), "boolean (true)")
    TEST_STRINGFICATION(
        token::custom_literal(rbb::array({number(10), number(20), number(30)})),
        "custom literal ((|10, 20, 30))")
TESTS_END()
