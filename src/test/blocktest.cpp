#include "tests_common.hpp"

#include <rbb/block.hpp>

using namespace rbb;

TESTS_INIT()
    rbb::literal::block bl;

    // { ~:x -> $ ! ~x * (~x) }
    auto &stm1 = bl.add_statement();
    stm1.add_expr<rbb::literal::context>();
    auto &table1 = stm1.add_expr<rbb::literal::table>();
    table1.add_symbol<rbb::literal::symbol>("x");
    table1.add_object<rbb::literal::message>();

    auto &ret_stm = bl.return_statement();
    ret_stm.add_expr<rbb::literal::context>();
    ret_stm.add_expr<rbb::literal::symbol>("x");
    ret_stm.add_expr<rbb::literal::symbol>("*");
    auto &expr = ret_stm.add_expr<rbb::block_statement>();
    expr.add_expr<rbb::literal::context>();
    expr.add_expr<rbb::literal::symbol>("x");

    auto context = rbb::table();

    rbb::object block = bl.eval();
    rbb::object block_instance = block << context;
    rbb::object result = block_instance << rbb::number(6);

    TEST_CONDITION(result == rbb::number(36), puts("Block isn't computing 6 squared"))

    // { ! $ * $ }
    rbb::literal::block bl2;

    auto &ret_stm2 = bl2.return_statement();
    ret_stm2.add_expr<rbb::literal::message>();
    ret_stm2.add_expr<rbb::literal::symbol>("*");
    ret_stm2.add_expr<rbb::literal::message>();

    rbb::object block2 = bl2.eval();
    rbb::object block_instance2 = block2 << rbb::empty();
    rbb::object result2 = block_instance2 << rbb::number(6);

    TEST_CONDITION(result2 == rbb::number(36), puts("Simple block isn't computing 6 squared"))

    // { ! $ } # The identity
    rbb::literal::block bl3;
    bl3.return_statement().add_expr<rbb::literal::message>();

    rbb::object block3 = bl3.eval();
    rbb::object block_instance3 = block3 << rbb::empty();
    rbb::object result3 = block_instance3 << rbb::number(6);

    TEST_CONDITION(result3 == rbb::number(6), puts("Identity function doesn't work"))

    // { ! $ > 5 if_true $.{ ! ~ * 2 } $.{ ! ~ * ~ } }
    rbb::literal::block bl4;

    auto &ret_stm4 = bl4.return_statement();
    ret_stm4.add_expr<rbb::literal::message>();
    ret_stm4.add_expr<rbb::literal::symbol>(">");
    ret_stm4.add_expr<rbb::literal::number>(5);
    ret_stm4.add_expr<rbb::literal::symbol>("if_true");

    auto &true_stm = ret_stm4.add_expr<rbb::block_statement>();
    auto &true_bl = true_stm.add_expr<rbb::literal::block>();
    auto &true_ret = true_bl.return_statement();
    true_ret.add_expr<rbb::literal::context>();
    true_ret.add_expr<rbb::literal::symbol>("*");
    true_ret.add_expr<rbb::literal::number>(2);
    true_stm.add_expr<rbb::literal::message>();

    auto &false_stm = ret_stm4.add_expr<rbb::block_statement>();
    auto &false_bl = false_stm.add_expr<rbb::literal::block>();
    auto &false_ret = false_bl.return_statement();
    false_ret.add_expr<rbb::literal::context>();
    false_ret.add_expr<rbb::literal::symbol>("*");
    false_ret.add_expr<rbb::literal::context>();
    false_stm.add_expr<rbb::literal::message>();

    rbb::object block4 = bl4.eval();
    block4 = block4 << rbb::table();

    TEST_CONDITION(
        rbb::number(9) == block4 << rbb::number(3),
        puts("{! $ > 5 if_true $ { ! ~ * 2 }, { ! ~ * ~ } } doesn't run."));

    rbb::literal::block bl5;
    bl5.return_statement().add_expr<rbb::literal::number>(10);
    rbb::object block5 = bl5.eval();

    TEST_CONDITION(
        block5 << rbb::table() << rbb::empty() == rbb::number(10),
        puts("{! 10 } doesn't run"))

    // Testing if a block responds the same message the same way twice in a row
    // { ~:x => $ + (~x) !~x }
    auto context6 = rbb::table({ rbb::symbol("x") }, { rbb::number(13) });

    rbb::literal::block bll6;

    auto &c_x_eq_arg_pl_x = bll6.add_statement();
    c_x_eq_arg_pl_x.add_expr<rbb::literal::context>();

    auto &t6 = c_x_eq_arg_pl_x.add_expr<rbb::literal::table>();
    t6.add_symbol<rbb::literal::symbol>("x");

    auto &arg_pl_x = t6.add_object<rbb::block_statement>();
    arg_pl_x.add_expr<rbb::literal::message>();
    arg_pl_x.add_expr<rbb::literal::symbol>("+");

    auto &_x = arg_pl_x.add_expr<rbb::block_statement>();
    _x.add_expr<rbb::literal::context>();
    _x.add_expr<rbb::literal::symbol>("x");

    auto &bll6_ret = bll6.return_statement();
    bll6_ret.add_expr<rbb::literal::context>();
    bll6_ret.add_expr<rbb::literal::symbol>("x");

    rbb::object block6 = bll6.eval();
    rbb::object block_instance6 = block6 << context6;

    rbb::object result6_1 = block_instance6 << rbb::number(7);

    TEST_CONDITION(
        result6_1 == rbb::number(20),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 13 ] 7  ==  %lld (should be 20)\n", result6_1.__value.integer()))

    rbb::object result6_2 = block_instance6 << rbb::number(7);

    TEST_CONDITION(
        result6_2 == rbb::number(27),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 20 ] 7  ==  %lld (should be 27)\n", result6_2.__value.integer()))

    auto block_instance6_1 = block6 << rbb::table({ rbb::symbol("x") }, { rbb::number(10) });
    rbb::object result6_1_1 = block_instance6_1 << rbb::number(7);

    TEST_CONDITION(
        result6_1_1 == rbb::number(17),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 10 ] 7 == %lld (should be 17)\n",
            result6_1_1.__value.integer()))

    rbb::object result6_3 = block_instance6 << rbb::number(7);

    TEST_CONDITION(
        result6_3 == rbb::number(34),
        printf("{ ~:x => $ + (~x) !~x }:[ x => 27 ] 7 == %lld (should be 34)\n",
            result6_3.__value.integer()))

    // { ~:self = @; ~i < 1000 if_true ~.{ ~:i = ~i + 1; ~self~() } !~i }:i = 10
    rbb::literal::block bll7;

    auto &stm_self_is_self_ref = bll7.add_statement();
    stm_self_is_self_ref.add_expr<rbb::literal::context>();

    auto &self_is_self_ref_table = stm_self_is_self_ref.add_expr<rbb::literal::table>();
    self_is_self_ref_table.add_symbol<rbb::literal::symbol>("self");
    self_is_self_ref_table.add_object<rbb::literal::self_ref>();

    auto &stm_if_i_lt_1000_then_increment = bll7.add_statement();
    stm_if_i_lt_1000_then_increment.add_expr<rbb::literal::context>();
    stm_if_i_lt_1000_then_increment.add_expr<rbb::literal::symbol>("i");
    stm_if_i_lt_1000_then_increment.add_expr<rbb::literal::symbol>("<");
    stm_if_i_lt_1000_then_increment.add_expr<rbb::literal::number>(1000);
    stm_if_i_lt_1000_then_increment.add_expr<rbb::literal::symbol>("if_true");

    auto &bll_increment_stm = stm_if_i_lt_1000_then_increment.add_expr<rbb::block_statement>();
    auto &bll_increment = bll_increment_stm.add_expr<rbb::literal::block>();
    auto &stm_increment = bll_increment.add_statement();
    stm_increment.add_expr<rbb::literal::context>();

    auto &tbl_increment = stm_increment.add_expr<rbb::literal::table>();
    tbl_increment.add_symbol<rbb::literal::symbol>("i");
    auto &increment_expr = tbl_increment.add_object<rbb::block_statement>();
    increment_expr.add_expr<rbb::literal::context>();
    increment_expr.add_expr<rbb::literal::symbol>("i");
    increment_expr.add_expr<rbb::literal::symbol>("+");
    increment_expr.add_expr<rbb::literal::number>(1);

    auto &stm_self_call = bll_increment.add_statement();
    stm_self_call.add_expr<rbb::literal::context>();
    stm_self_call.add_expr<rbb::literal::symbol>("self");
    stm_self_call.add_expr<rbb::literal::context>();
    stm_self_call.add_expr<rbb::literal::empty>();

    bll_increment_stm.add_expr<rbb::literal::context>();

    auto &expr_bll7_ans = bll7.return_statement();
    expr_bll7_ans.add_expr<rbb::literal::context>();
    expr_bll7_ans.add_expr<rbb::literal::symbol>("i");

    auto context7 = rbb::table({
        rbb::symbol("i")
    }, {
        rbb::number(10)
    });

    auto block7 = bll7.eval() << context7;
    auto result7 = block7 << rbb::empty();

    TEST_CONDITION(
        result7 == rbb::number(1000),
        printf("{ ~:self => @; ~i < 1000 if_true~ { ~:i => ~i + 1; ~self~[] } !~i }:[i => 10][] == %lld (should be 1000)\n", result7.__value.integer()))

    // { !%a } # (being % :a -> 100)
    {
        literal::block block_lit_;

        auto &block_ret_ = block_lit_.return_statement();
        block_ret_.add_expr<literal::user_defined>(table({symbol("a")}, {number(100)}));
        block_ret_.add_expr<literal::symbol>("a");

        auto block_ = block_lit_.eval();
        auto result_ = block_ << empty() << empty();

        TEST_CONDITION(
            result_ == number(100),
            printf("Number is %s (expected 100)\n", result_.to_string().c_str()))
    }

    // { !{ !%$ }()1 } # (being % |10, 12)
    {
        literal::block block_lit_;

        auto &block_ret_ = block_lit_.return_statement();
        auto &inner_block = block_ret_.add_expr<literal::block>();
        block_ret_.add_expr<literal::empty>();
        block_ret_.add_expr<literal::number>(1);

        auto &inner_ret = inner_block.return_statement();
        inner_ret.add_expr<literal::user_defined>(rbb::array({number(10), number(12)}));
        inner_ret.add_expr<literal::message>();

        auto block_ = block_lit_.eval();
        auto result_ = block_ << empty() << empty();

        TEST_CONDITION(
            result_ == number(12),
            printf("Number is %s (expected 12)\n", result_.to_string().c_str()))
    }

    // { ~:a -> $ !<custom literal containing expression ~a> }
    // the value of the literal will be :a -> 10, b -> ~a
    {
        literal::block bl;

        auto &assign_a = bl.add_statement();
        assign_a.add_expr<literal::context>();

        auto &tab = assign_a.add_expr<literal::table>();
        tab.add_symbol<literal::symbol>("a");
        tab.add_object<literal::message>();

        auto &ret = bl.return_statement();
        auto &user_literal = ret.add_expr<literal::user_defined>(
            table({symbol("a")}, {number(10)}),
            dummy_master.parse("{!$eval_res merge:b = $eval_expr 0}") << empty(),
            vector<object>{dummy_master.parse("{!~a}")});

        auto block = bl.eval();
        auto instance = block << table();

        // call instance with 10
        auto call_1 = instance << number(10);

        TEST_CONDITION(
            call_1 << "b" == number(10),
            printf(
                "User literal was not properly evaluated: expected 10, got %s\n",
                (call_1 << "b").to_string().c_str()))

        // call instance with 20
        auto call_2 = instance << 20;

        TEST_CONDITION(
            call_2 << "b" == number(20),
            printf(
                "User literal was not properly evaluated: expected 20, got %s\n",
                (call_2 << "b").to_string().c_str()))
    }
TESTS_END()
