#include "tests_common.hpp"

TESTS_INIT()
    
    rbb::object true_obj = rbb::boolean(true);
    rbb::object false_obj = rbb::boolean(false);
    rbb::object equals = rbb::symbol("==");
    
    TEST_CONDITION(true_obj.__value.boolean == true, puts("True object is false"))
    TEST_CONDITION(false_obj.__value.boolean == false, puts("False object is true"))
    TEST_CONDITION(true_obj.send_msg(equals).send_msg(true_obj).__value.type == rbb::value_t::boolean_t,
                   puts("The comparison block isn't returning a boolean"))
    TEST_CONDITION(true_obj.send_msg(equals).send_msg(true_obj) == true_obj, puts("(true == true) != true"))
    TEST_CONDITION(true_obj.send_msg(true_obj) == rbb::empty(), puts("Boolean responds to a boolean message (it shouldn't)"))
    
    TEST_OPERATOR_EQ(true_obj, false_obj)
    TEST_OPERATOR_EQ(true_obj, rbb::empty())
    TEST_OPERATOR_EQ(false_obj, rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), true_obj)
    TEST_OPERATOR_EQ(rbb::empty(), false_obj)
TESTS_END()
