#include "tests_common.hpp"
#include "../core/block.hpp"

TESTS_INIT()

    rbb::object true_obj = rbb::boolean(true);
    rbb::object false_obj = rbb::boolean(false);
    rbb::object equals = rbb::symbol("==");

    TEST_CONDITION(true_obj.__value.boolean == true, puts("True object is false"))
    TEST_CONDITION(false_obj.__value.boolean == false, puts("False object is true"))
    TEST_CONDITION((true_obj << equals << true_obj).__value.type == rbb::value_t::boolean_t,
                   puts("The comparison block isn't returning a boolean"))
    TEST_CONDITION(true_obj << equals << true_obj == true_obj, puts("(true == true) != true"))
    TEST_CONDITION(true_obj << true_obj == rbb::empty(), puts("Boolean responds to a boolean message (it shouldn't)"))

    TEST_OPERATOR_EQ(true_obj, false_obj)
    TEST_OPERATOR_EQ(true_obj, rbb::empty())
    TEST_OPERATOR_EQ(false_obj, rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), true_obj)
    TEST_OPERATOR_EQ(rbb::empty(), false_obj)

    TEST_CONDITION(
        true_obj << rbb::symbol("/\\") << true_obj == true_obj,
        puts("AND  | T| T|  error"))
    TEST_CONDITION(
        true_obj << rbb::symbol("/\\") << false_obj == false_obj,
        puts("AND  | T| F|  error"))
    TEST_CONDITION(
        false_obj << rbb::symbol("/\\") << true_obj == false_obj,
        puts("AND  | F| T|  error"))
    TEST_CONDITION(
        false_obj << rbb::symbol("/\\") << false_obj == false_obj,
        puts("AND  | F| F|  error"))

    TEST_CONDITION(
        true_obj << rbb::symbol("\\/") << true_obj == true_obj,
        puts("OR   | T| T|  error"))
    TEST_CONDITION(
        true_obj << rbb::symbol("\\/") << false_obj == true_obj,
        puts("OR   | T| F|  error"))
    TEST_CONDITION(
        false_obj << rbb::symbol("\\/") << true_obj == true_obj,
        puts("OR   | F| T|  error"))
    TEST_CONDITION(
        false_obj << rbb::symbol("\\/") << false_obj == false_obj,
        puts("OR   | F| F|  error"))

    TEST_CONDITION(
        true_obj << rbb::symbol("><") == false_obj,
        puts("NOT     | T| error"))
    TEST_CONDITION(
        false_obj << rbb::symbol("><") == true_obj,
        puts("NOT     | F| error"))

    // [true]? :[] {! 3 } {! 5 }
    rbb::literal::block *bl_true = new rbb::literal::block;
    bl_true->set_return_expression(new rbb::literal::number(3));
    rbb::object block_true = bl_true->eval();

    rbb::literal::block *bl_false = new rbb::literal::block;
    bl_false->set_return_expression(new rbb::literal::number(5));
    rbb::object block_false = bl_false->eval();

    TEST_CONDITION(
        rbb::boolean(true)
             << rbb::symbol("?")
             << rbb::table()
             << block_true
             << block_false == rbb::number(3),
        puts("Flow control isn't working."))
    TEST_CONDITION(
        rbb::boolean(false)
             << rbb::symbol("?")
             << rbb::table()
             << block_true
             << block_false == rbb::number(5),
        puts("Flow control isn't working."))
TESTS_END()
