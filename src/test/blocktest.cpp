#include "tests_common.hpp"

#include "../core/block.hpp"

TESTS_INIT()
    rbb::literal::block *bl = new rbb::literal::block;
    
    // { ~[: x = $] ! ~x * (~x) }
    rbb::block_statement *stm1 = new rbb::block_statement;
    stm1->add_expr(new rbb::literal::symbol_table);
    rbb::expr *symbols[] = { new rbb::literal::symbol("x") };
    rbb::expr *objects[] = { new rbb::literal::block_arg };
    stm1->add_expr(new rbb::literal::generic_object(symbols, objects, 1));
    bl->add_statement(stm1);
    
    rbb::block_statement *ret_expr = new rbb::block_statement;
    ret_expr->add_expr(new rbb::literal::symbol_table);
    ret_expr->add_expr(new rbb::literal::symbol("x"));
    ret_expr->add_expr(new rbb::literal::symbol("*"));
    rbb::block_statement *expr = new rbb::block_statement;
    expr->add_expr(new rbb::literal::symbol_table);
    expr->add_expr(new rbb::literal::symbol("x"));
    ret_expr->add_expr(expr);
    bl->set_return_expression(ret_expr);
    
    rbb::object sym_table = rbb::generic_object(0, 0, 0);
    
    rbb::object block_body = bl->eval();
    rbb::object block = block_body.send_msg(sym_table);
    rbb::object result = block.send_msg(rbb::number(6));
    
    TEST_CONDITION(result == rbb::number(36), puts("Block isn't computing 6 squared"))
    
    // { ! $ * $ }
    rbb::literal::block *bl2 = new rbb::literal::block;
    
    rbb::block_statement *ret_expr2 = new rbb::block_statement;
    ret_expr2->add_expr(new rbb::literal::block_arg);
    ret_expr2->add_expr(new rbb::literal::symbol("*"));
    ret_expr2->add_expr(new rbb::literal::block_arg);
    bl2->set_return_expression(ret_expr2);
    
    rbb::object block_body2 = bl2->eval();
    rbb::object block2 = block_body2.send_msg(rbb::empty());
    rbb::object result2 = block2.send_msg(rbb::number(6));
    
    TEST_CONDITION(result2 == rbb::number(36), puts("Simple block isn't computing 6 squared"))
    
    // { ! $ } # The identity
    rbb::literal::block *bl3 = new rbb::literal::block;
    bl3->set_return_expression(new rbb::literal::block_arg);
    
    rbb::object block_body3 = bl3->eval();
    rbb::object block3 = block_body3.send_msg(rbb::empty());
    rbb::object result3 = block3.send_msg(rbb::number(6));
    
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
    true_ret->add_expr(new rbb::literal::symbol_table);
    true_ret->add_expr(new rbb::literal::symbol("*"));
    true_ret->add_expr(new rbb::literal::number(2));
    true_bl->set_return_expression(true_ret);
    true_expr->add_expr(true_bl);
    
    rbb::block_statement *false_expr = new rbb::block_statement;
    rbb::literal::block *false_bl = new rbb::literal::block;
    rbb::block_statement *false_ret = new rbb::block_statement;
    false_ret->add_expr(new rbb::literal::symbol_table);
    false_ret->add_expr(new rbb::literal::symbol("*"));
    false_ret->add_expr(new rbb::literal::symbol_table);
    false_bl->set_return_expression(false_ret);
    false_expr->add_expr(false_bl);
    
    rbb::expr *decision_list[] = { true_expr, false_expr };
    rbb::literal::list *d_list = new rbb::literal::list(decision_list, 2);
    ret_expr4->add_expr(d_list);
    
    bl4->set_return_expression(ret_expr4);
    
    rbb::object block4 = bl4->eval();
    block4 = block4.send_msg(rbb::generic_object(0, 0, 0));
    
    TEST_CONDITION(
        rbb::number(9) == block4.send_msg(rbb::number(3)),
        puts("A more complex block doesn't run."));
TESTS_END()