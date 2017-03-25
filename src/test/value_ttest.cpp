#include "tests_common.hpp"

#include "../core/object.hpp"
#include "../core/shared_data_t.hpp"
#include "../core/symbol.hpp"

using namespace rbb;

TESTS_INIT()
    value_t v1{10, value_t::integer_v};
    TEST_CONDITION(
        v1.integer() == 10,
        printf("Wrong value: expected 10, got %lld\n", v1.integer()))
    
    value_t v2{12.1, value_t::floating_v};
    TEST_CONDITION(
        v2.floating() == 12.1,
        printf("Wrong value: expected 12.1, got %lf\n", v2.floating()))
    
    value_t v3{false, value_t::boolean_v};
    TEST_CONDITION(
        !v3.boolean(),
        printf("Wrong value: expected false, got %d\n", v3.boolean()))
    
    auto v1_ass = v1;
    TEST_CONDITION(
        v1_ass.integer() == 10,
        printf("Could not assign value: expected 10, got %lld\n", v1_ass.integer()))
    
    auto v1_move_ass = move(v1);
    TEST_CONDITION(
        v1_move_ass.integer() == 10,
        printf("Could not move-assign value: expected 10, got %lld\n", v1_move_ass.integer()))
    
    value_t v2_copy{v2};
    TEST_CONDITION(
        v2_copy.floating() == 12.1,
        printf("Could not copy value: expected 12.1, got %lf\n", v2_copy.floating()))
    
    value_t v2_move{move(v2)};
    TEST_CONDITION(
        v2_move.floating() == 12.1,
        printf("Could not move value: expected 12.1, got %lf\n", v2_move.floating()))
    
    value_t vsym{retrieve_symbol("symbol")};
    TEST_CONDITION(
        *vsym.symbol() == "symbol",
        printf("Wrong value: expected symbol, got %s\n", vsym.symbol()->c_str()))
    
    class test_data : public shared_data_t
    {
    public:
        long long a;
        long long b;
        long long c;
    };
    
    auto d1 = new test_data;
    d1->a = 1000;
    d1->b = 1000000;
    d1->c = 1000000000;
    
    value_t vdata{d1};
    
    auto d1_out = static_pointer_cast<test_data>(vdata.data());
    auto d1_ok =
        d1_out->a == 1000 &&
        d1_out->b == 1000000 &&
        d1_out->c == 1000000000;
    TEST_CONDITION(d1_ok, puts("Not able to read data value"))
    
    auto vdata_copy = vdata;
    auto d1_copy = static_pointer_cast<test_data>(vdata_copy.data());
    auto d1_copy_ok =
        d1_copy->a == 1000 &&
        d1_copy->b == 1000000 &&
        d1_copy->c == 1000000000;
    TEST_CONDITION(d1_copy_ok, puts("Not able to read copied data value"))
    
    value_t vdata2_copy_to{value_t::no_data_t};
    {
        value_t vdata2_copy_l1{value_t::no_data_t};
        {
            value_t vdata2_copy_l2{value_t::no_data_t};
            {
                auto d2 = new test_data;
                d2->a = 10;
                d2->b = -10;
                d2->c = 100000;
                
                value_t vdata2{d2};
                value_t vdata_disposable{vdata2};
                vdata2_copy_l2 = vdata2;
            }
            vdata2_copy_l1 = vdata2_copy_l2;
        }
        vdata2_copy_to = vdata2_copy_l1;
    }
    
    auto d2 = static_pointer_cast<test_data>(vdata2_copy_to.data());
    auto d2_ok =
        d2->a == 10 &&
        d2->b == -10 &&
        d2->c == 100000;
    TEST_CONDITION(d2_ok, puts("Couldn't access data from copy of delete data value"))
    
    class destructor_called : public shared_data_t
    {
    public:
        destructor_called(bool &called) :
            called{called}
        {}
        
        ~destructor_called()
        {
            called = true;
        }
        
        bool &called;
    };
    
    bool d_called = false;
    
    {
        auto d3 = new destructor_called{d_called};
        value_t vdata3{d3};
        
        auto vdata3_copy = vdata3;
        {
            value_t vdata3_ctor_copy{vdata3};
            value_t vdata3_ctor_copy2{vdata3_copy};
        }
    }
    
    TEST_CONDITION(d_called, puts("Shared data was not destructed"))
    
    d1->a = 500;
    TEST_CONDITION(
        d1_copy->a == 500,
        puts("Changes in copied data values are not being reflected in the original ones"))
    
    auto create_data_val = []()
    {
        auto d = new test_data;
        d->a = 100;
        d->b = 100;
        d->c = 100;
        
        return value_t{d};
    };
    
    value_t from_func{value_t::empty_t};
    {
        value_t assigned_later{value_t::empty_t};
        
        assigned_later = create_data_val();
        
        from_func = assigned_later;
    }

    auto from_func_d = static_pointer_cast<test_data>(from_func.data());
    auto from_func_ok =
        from_func_d->a == 100 &&
        from_func_d->b == 100 &&
        from_func_d->c == 100;
    TEST_CONDITION(from_func_ok, puts("Values aren't being passed around from functions properly"))
TESTS_END()

