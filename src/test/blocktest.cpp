#include "tests_common.hpp"

#include "../core/block.hpp"

TESTS_INIT()
    rbb::literal::block *bl = new rbb::literal::block;

    // { ~[: x = $] ! ~x * (~x) }
    rbb::block_statement *stm1 = new rbb::block_statement;
    stm1->add_expr(new rbb::literal::context);
    rbb::expr *symbols[] = { new rbb::literal::symbol("x") };
    rbb::expr *objects[] = { new rbb::literal::block_arg };
    stm1->add_expr(new rbb::literal::table(symbols, objects, 1));
    bl->add_statement(stm1);

    rbb::block_statement *ret_expr = new rbb::block_statement;
    ret_expr->add_expr(new rbb::literal::context);
    ret_expr->add_expr(new rbb::literal::symbol("x"));
    ret_expr->add_expr(new rbb::literal::symbol("*"));
    rbb::block_statement *expr = new rbb::block_statement;
    expr->add_expr(new rbb::literal::context);
    expr->add_expr(new rbb::literal::symbol("x"));
    ret_expr->add_expr(expr);
    bl->set_return_expression(ret_expr);

    rbb::object context = rbb::table(0, 0, 0);

    rbb::object block_body = bl->eval();
    rbb::object block = block_body << context;
    rbb::object result = block << rbb::number(6);

    TEST_CONDITION(result == rbb::number(36), puts("Block isn't computing 6 squared"))

    // { ! $ * $ }
    rbb::literal::block *bl2 = new rbb::literal::block;

    rbb::block_statement *ret_expr2 = new rbb::block_statement;
    ret_expr2->add_expr(new rbb::literal::block_arg);
    ret_expr2->add_expr(new rbb::literal::symbol("*"));
    ret_expr2->add_expr(new rbb::literal::block_arg);
    bl2->set_return_expression(ret_expr2);

    rbb::object block_body2 = bl2->eval();
    rbb::object block2 = block_body2 << rbb::empty();
    rbb::object result2 = block2 << rbb::number(6);

    TEST_CONDITION(result2 == rbb::number(36), puts("Simple block isn't computing 6 squared"))

    // { ! $ } # The identity
    rbb::literal::block *bl3 = new rbb::literal::block;
    bl3->set_return_expression(new rbb::literal::block_arg);

    rbb::object block_body3 = bl3->eval();
    rbb::object block3 = block_body3 << rbb::empty();
    rbb::object result3 = block3 << rbb::number(6);

    TEST_CONDITION(result3 == rbb::number(6), puts("Identity function doesn't work"))

    // { ! $ > 5? $ [| { ! ~ * 2 }, { ! ~ * ~ }] }
    rbb::literal::block *bl4 = new rbb::literal::block;

    rbb::block_statement *ret_expr4 = new rbb::block_statement;
    ret_expr4->add_expr(new rbb::literal::block_arg);
    ret_expr4->add_expr(new rbb::literal::symbol(">"));
    ret_expr4->add_expr(new rbb::literal::number(5));
    ret_expr4->add_expr(new rbb::literal::symbol("?"));
    ret_expr4->add_expr(new rbb::literal::block_arg);

    rbb::block_statement *true_expr = new rbb::block_statement;
    rbb::literal::block *true_bl = new rbb::literal::block;
    rbb::block_statement *true_ret = new rbb::block_statement;
    true_ret->add_expr(new rbb::literal::context);
    true_ret->add_expr(new rbb::literal::symbol("*"));
    true_ret->add_expr(new rbb::literal::number(2));
    true_bl->set_return_expression(true_ret);
    true_expr->add_expr(true_bl);

    rbb::block_statement *false_expr = new rbb::block_statement;
    rbb::literal::block *false_bl = new rbb::literal::block;
    rbb::block_statement *false_ret = new rbb::block_statement;
    false_ret->add_expr(new rbb::literal::context);
    false_ret->add_expr(new rbb::literal::symbol("*"));
    false_ret->add_expr(new rbb::literal::context);
    false_bl->set_return_expression(false_ret);
    false_expr->add_expr(false_bl);

    rbb::expr *decision_array[] = { true_expr, false_expr };
    rbb::literal::array *d_array = new rbb::literal::array(decision_array, 2);
    ret_expr4->add_expr(d_array);

    bl4->set_return_expression(ret_expr4);

    rbb::object block4 = bl4->eval();
    block4 = block4 << rbb::table(0, 0, 0);

    TEST_CONDITION(
        rbb::number(9) == block4 << rbb::number(3),
        puts("{! $ > 5? $ [| { ! ~ * 2 }, { ! ~ * ~ }] } doesn't run."));

    rbb::literal::block *bl5 = new rbb::literal::block;
    bl5->set_return_expression(new rbb::literal::number(10));
    rbb::object block5 = bl5->eval();

    TEST_CONDITION(
        block5 << rbb::table(0, 0, 0) << rbb::empty() == rbb::number(10),
        puts("{! 10 } doesn't run"))

    // Testing if a block responds the same message the same way twice in a row
    // { ~:x => $ + (~x) !~x }
    rbb::object symbols6[] = { rbb::symbol("x") };
    rbb::object objects6[] = { rbb::number(13) };
    rbb::object context6 = rbb::table(symbols6, objects6, 1);

    rbb::literal::block *bll6 = new rbb::literal::block;

    rbb::block_statement *c_x_eq_arg_pl_x = new rbb::block_statement;
    c_x_eq_arg_pl_x->add_expr(new rbb::literal::context);

    rbb::block_statement *arg_pl_x = new rbb::block_statement;
    arg_pl_x->add_expr(new rbb::literal::block_arg);
    arg_pl_x->add_expr(new rbb::literal::symbol("+"));

    rbb::block_statement *_x = new rbb::block_statement;
    _x->add_expr(new rbb::literal::context);
    _x->add_expr(new rbb::literal::symbol("x"));

    arg_pl_x->add_expr(_x);

    rbb::expr *s6[] = { new rbb::literal::symbol("+") };
    rbb::expr *o6[] = { arg_pl_x };
    c_x_eq_arg_pl_x->add_expr(new rbb::literal::table(s6, o6, 1));

    bll6->add_statement(c_x_eq_arg_pl_x);
    bll6->set_return_expression(_x);

    rbb::object block6 = bll6->eval();
    block6 = block6 << context6;

    rbb::object result6_1 = block6 << rbb::number(7);

    TEST_CONDITION(
        result6_1 == rbb::number(20),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 13 ] 7  ==  %ld\n", result6_1.__value.integer))

    rbb::object result6_2 = block6 << rbb::number(7);

    TEST_CONDITION(
        result6_2 == rbb::number(20),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 13 ] 7  ==  %ld\n", result6_2.__value.integer))

    TEST_CONDITION(
        result6_1 == result6_2,
        puts("Block gives different results when run a second time"))
TESTS_END()
