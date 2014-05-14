#include "tests_common.hpp"

#include "../core/block.hpp"

TESTS_INIT()
    rbb::literal::block *bl = new rbb::literal::block;

    // { ~[: x = $] ! ~x * (~x) }
    rbb::block_statement *stm1 = new rbb::block_statement;
    stm1->add_expr(new rbb::literal::context);
    rbb::expr *symbols[] = { new rbb::literal::symbol("x") };
    rbb::expr *objects[] = { new rbb::literal::message };
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
    ret_expr2->add_expr(new rbb::literal::message);
    ret_expr2->add_expr(new rbb::literal::symbol("*"));
    ret_expr2->add_expr(new rbb::literal::message);
    bl2->set_return_expression(ret_expr2);

    rbb::object block_body2 = bl2->eval();
    rbb::object block2 = block_body2 << rbb::empty();
    rbb::object result2 = block2 << rbb::number(6);

    TEST_CONDITION(result2 == rbb::number(36), puts("Simple block isn't computing 6 squared"))

    // { ! $ } # The identity
    rbb::literal::block *bl3 = new rbb::literal::block;
    bl3->set_return_expression(new rbb::literal::message);

    rbb::object block_body3 = bl3->eval();
    rbb::object block3 = block_body3 << rbb::empty();
    rbb::object result3 = block3 << rbb::number(6);

    TEST_CONDITION(result3 == rbb::number(6), puts("Identity function doesn't work"))

    // { ! $ > 5? $ { ! ~ * 2 } { ! ~ * ~ } }
    rbb::literal::block *bl4 = new rbb::literal::block;

    rbb::block_statement *ret_expr4 = new rbb::block_statement;
    ret_expr4->add_expr(new rbb::literal::message);
    ret_expr4->add_expr(new rbb::literal::symbol(">"));
    ret_expr4->add_expr(new rbb::literal::number(5));
    ret_expr4->add_expr(new rbb::literal::symbol("?"));
    ret_expr4->add_expr(new rbb::literal::message);

    rbb::block_statement *true_expr = new rbb::block_statement;
    rbb::literal::block *true_bl = new rbb::literal::block;
    rbb::block_statement *true_ret = new rbb::block_statement;
    true_ret->add_expr(new rbb::literal::context);
    true_ret->add_expr(new rbb::literal::symbol("*"));
    true_ret->add_expr(new rbb::literal::number(2));
    true_bl->set_return_expression(true_ret);
    true_expr->add_expr(true_bl);
    ret_expr4->add_expr(true_expr);

    rbb::block_statement *false_expr = new rbb::block_statement;
    rbb::literal::block *false_bl = new rbb::literal::block;
    rbb::block_statement *false_ret = new rbb::block_statement;
    false_ret->add_expr(new rbb::literal::context);
    false_ret->add_expr(new rbb::literal::symbol("*"));
    false_ret->add_expr(new rbb::literal::context);
    false_bl->set_return_expression(false_ret);
    false_expr->add_expr(false_bl);
    ret_expr4->add_expr(false_expr);

    bl4->set_return_expression(ret_expr4);

    rbb::object block4 = bl4->eval();
    block4 = block4 << rbb::table(0, 0, 0);

    TEST_CONDITION(
        rbb::number(9) == block4 << rbb::number(3),
        puts("{! $ > 5? $ { ! ~ * 2 }, { ! ~ * ~ } } doesn't run."));

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
    arg_pl_x->add_expr(new rbb::literal::message);
    arg_pl_x->add_expr(new rbb::literal::symbol("+"));

    rbb::block_statement *_x = new rbb::block_statement;
    _x->add_expr(new rbb::literal::context);
    _x->add_expr(new rbb::literal::symbol("x"));

    arg_pl_x->add_expr(_x);

    rbb::expr *s6[] = { new rbb::literal::symbol("x") };
    rbb::expr *o6[] = { arg_pl_x };
    c_x_eq_arg_pl_x->add_expr(new rbb::literal::table(s6, o6, 1));

    bll6->add_statement(c_x_eq_arg_pl_x);
    bll6->set_return_expression(_x);

    rbb::object prototype6 = bll6->eval();
    rbb::object block6 = prototype6 << context6;

    rbb::object result6_1 = block6 << rbb::number(7);

    TEST_CONDITION(
        result6_1 == rbb::number(20),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 13 ] 7  ==  %ld\n", result6_1.__value.integer))

    rbb::object result6_2 = block6 << rbb::number(7);

    TEST_CONDITION(
        result6_2 == rbb::number(27),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 20 ] 7  ==  %ld\n", result6_2.__value.integer))

    rbb::object symbols6_1[] = { rbb::symbol("x") };
    rbb::object objects6_1[] = { rbb::number(10) };
    rbb::object block6_1 = prototype6 << rbb::table(symbols6_1, objects6_1, 1);
    rbb::object result6_1_1 = block6_1 << rbb::number(7);

    TEST_CONDITION(
        result6_1_1 == rbb::number(17),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 10 ] 7 == %ld (should be 17)\n",
            result6_1_1.__value.integer))

    rbb::object result6_3 = block6 << rbb::number(7);

    TEST_CONDITION(
        result6_3 == rbb::number(34),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 27 ] 7 == %ld (should be 34)\n",
            result6_3.__value.integer))

    // { ~:self => @; ~i < 1000?~ { ~:i => ~i + 1; ~self~[] } !~i }:i => 10
    rbb::expr *self[] = { new rbb::literal::symbol("self") };
    rbb::expr *self_ref[] = { new rbb::literal::self_ref };
    auto table_self_is_self_ref = new rbb::literal::table { self, self_ref, 1 };

    auto stm_self_is_self_ref = new rbb::block_statement;
    stm_self_is_self_ref->add_expr(new rbb::literal::context);
    stm_self_is_self_ref->add_expr(table_self_is_self_ref);

    auto increment_expr_ = new rbb::block_statement;
    increment_expr_->add_expr(new rbb::literal::context);
    increment_expr_->add_expr(new rbb::literal::symbol("i"));
    increment_expr_->add_expr(new rbb::literal::symbol("+"));
    increment_expr_->add_expr(new rbb::literal::number(1));

    rbb::expr *increment_symbol[] = { new rbb::literal::symbol("i") };
    rbb::expr *increment_expr[] = { increment_expr_ };
    auto table_increment = new rbb::literal::table(increment_symbol, increment_expr, 1);

    auto stm_increment = new rbb::block_statement;
    stm_increment->add_expr(new rbb::literal::context);
    stm_increment->add_expr(table_increment);

    auto stm_self_call = new rbb::block_statement;
    stm_self_call->add_expr(new rbb::literal::context);
    stm_self_call->add_expr(new rbb::literal::symbol("self"));
    stm_self_call->add_expr(new rbb::literal::context);
    stm_self_call->add_expr(new rbb::literal::empty);

    auto bll_increment = new rbb::literal::block;
    bll_increment->add_statement(stm_increment);
    bll_increment->add_statement(stm_self_call);

    auto stm_if_i_lt_1000_then_increment = new rbb::block_statement;
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::context);
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::symbol("i"));
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::symbol("<"));
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::number(1000));
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::symbol("?"));
    stm_if_i_lt_1000_then_increment->add_expr(new rbb::literal::context);
    stm_if_i_lt_1000_then_increment->add_expr(bll_increment);

    auto expr_bll7_ans = new rbb::block_statement;
    expr_bll7_ans->add_expr(new rbb::literal::context);
    expr_bll7_ans->add_expr(new rbb::literal::symbol("i"));

    auto bll7 = new rbb::literal::block;
    bll7->add_statement(stm_self_is_self_ref);
    bll7->add_statement(stm_if_i_lt_1000_then_increment);
    bll7->set_return_expression(expr_bll7_ans);

    rbb::object symbols7[] = { rbb::symbol("i") };
    rbb::object *objects7 = objects6_1;
    auto context7 = rbb::table(symbols7, objects7, 1);

    auto block7 = bll7->eval() << context7;
    auto result7 = block7 << rbb::empty();

    TEST_CONDITION(
        result7 == rbb::number(1000),
        printf("{ ~:self => @; ~i < 1000?~ { ~:i => ~i + 1; ~self~[] } !~i }:[i => 10][] == %ld (should be 1000)\n", result7.__value.integer))
TESTS_END()
