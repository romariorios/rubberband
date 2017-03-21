#include "tests_common.hpp"
#include <rbb/block.hpp>
#include <rbb/error.hpp>

TESTS_INIT()

    rbb::object true_obj = rbb::boolean(true);
    rbb::object false_obj = rbb::boolean(false);
    rbb::object equals = rbb::symbol("==");

    TEST_CONDITION(true_obj.__value.boolean() == true, puts("True object is false"))
    TEST_CONDITION(false_obj.__value.boolean() == false, puts("False object is true"))
    TEST_CONDITION((true_obj << equals << true_obj).__value.type == rbb::value_t::boolean_t,
                   puts("The comparison block isn't returning a boolean"))
    TEST_CONDITION(true_obj << equals << true_obj == true_obj, puts("(true == true) /= true"))
    bool thrown = false;
    try {
        true_obj << true_obj;
    } catch (rbb::message_not_recognized_error) {
        thrown = true;
    }
    TEST_CONDITION(thrown, puts("Boolean responds to a boolean message (it shouldn't)"))

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

    rbb::literal::block bl_true;
    bl_true.return_statement().add_expr<rbb::literal::number>(3);
    auto block_true = bl_true.eval();

    rbb::literal::block bl_false;
    bl_false.return_statement().add_expr<rbb::literal::number>(5);
    auto block_false = bl_false.eval();

    // 1? (:) {!3} {!5}
    TEST_CONDITION(
        rbb::boolean(true)
             << rbb::symbol("?")
             << rbb::table()
             << block_true
             << block_false == rbb::number(3),
        puts("Flow control isn't working."))

    // 0? (:) {!3} {!5}
    TEST_CONDITION(
        rbb::boolean(false)
             << rbb::symbol("?")
             << rbb::table()
             << block_true
             << block_false == rbb::number(5),
        puts("Flow control isn't working."))

    // 1? () {!3} {!5}
    TEST_CONDITION_WITH_EXCEPTION(
        boolean(true)
            << symbol("?")
            << empty()
            << block_true
            << block_false == number(3),
        puts("Flow control with empty context isn't working"))

    // 0? () {!3} {!5}
    TEST_CONDITION_WITH_EXCEPTION(
        boolean(false)
            << symbol("?")
            << empty()
            << block_true
            << block_false == number(5),
            puts("Flow control with empty context isn't working"))

    {
        bool error_raised = false;
        bool right_error_object = false;
        object error_obj;

        try {
            boolean(true) << symbol("^") << symbol("error");
        } catch (rbb::runtime_error e) {
            error_raised = true;
            error_obj = e.error_obj;
            if (e.error_obj == symbol("error"))
                right_error_object = true;
        }

        TEST_CONDITION(error_raised, puts("Couldn't raise error"))
        TEST_CONDITION(
            error_raised && right_error_object,
            printf(
                "Error object is \"%s\" (expected \"error\")\n",
                error_obj.to_string().c_str()))
    }
TESTS_END()
