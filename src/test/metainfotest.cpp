#include "tests_common.hpp"

#include <rbb/block.hpp>

#define TEST_RESPONDS_TO(message, expected)\
    TEST_CONDITION_WITH_EXCEPTION(\
        obj << symbol("<<") << message == boolean(expected),\
        printf(\
            "Object %s reports it %s to %s\n",\
            obj.to_string().c_str(),\
            expected? "doesn't respond" : "responds",\
            message.to_string().c_str()))
    
#define TEST_INTERFACE(ifacename, expected)\
    TEST_CONDITION_WITH_EXCEPTION(\
        obj << symbol("<<?") << symbol(ifacename) == boolean(expected),\
        printf(\
            "Object %s reports it %s interface %s\n",\
            obj.to_string().c_str(),\
            expected? "doesn't follow" : "follows",\
            ifacename))

TESTS_INIT()
    {
        auto &&obj = number(12);
        TEST_RESPONDS_TO(symbol("+"), true)
        TEST_RESPONDS_TO(symbol("^"), false)
        TEST_RESPONDS_TO(symbol("*"), true)
        TEST_RESPONDS_TO(symbol(">="), true)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(10), number(20), number(30)}), true)
        TEST_RESPONDS_TO(
            table({symbol("valhalla"), symbol("lol")}, {number(10), number(20)}),
            false)
        
        TEST_INTERFACE("<-0", true)
    }
    
    {
        object obj; // empty
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(symbol("+"), false)
        TEST_RESPONDS_TO(object{}, false)
        
        TEST_INTERFACE("<-()", true)
    }
    
    {
        auto &&obj = symbol("valhalla");
        TEST_RESPONDS_TO(symbol("lol"), false)
        TEST_RESPONDS_TO(rbb::array({object{}, symbol("a"), number(15)}), false)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("<"), false)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        
        TEST_INTERFACE("<-a", true)
    }
    
    {
        auto &&obj = boolean(false);
        TEST_RESPONDS_TO(symbol("?"), true)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("><"), true)
        TEST_RESPONDS_TO(symbol("\\/"), true)
        TEST_RESPONDS_TO(symbol("/\\"), true)
        TEST_RESPONDS_TO(symbol("+"), false)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(number(12), false)
        
        TEST_INTERFACE("<-?", true)
    }
    
    {
        auto &&obj = rbb::array({number(10), number(20), symbol("a")});
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(number(0), true)
        TEST_RESPONDS_TO(symbol("*"), true)
        TEST_RESPONDS_TO(symbol("+"), true)
        TEST_RESPONDS_TO(number(3), false)
        TEST_RESPONDS_TO(symbol("-"), false)
        TEST_RESPONDS_TO(symbol("/"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(0), number(30)}), true)
        TEST_RESPONDS_TO(rbb::array({symbol("a"), number(12)}), false)
        TEST_RESPONDS_TO(rbb::array({number(2)}), false)
        TEST_RESPONDS_TO(rbb::array({number(12), number(12), number(12)}), false)
        TEST_RESPONDS_TO(rbb::array({number(3), number(100)}), false)
        
        TEST_INTERFACE("<-|", true)
    }
    
    {
        auto &&obj = table(
            {symbol("a"), symbol("b"), symbol("c")},
            {number(10), number(20), number(30)});
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("*"), true)
        TEST_RESPONDS_TO(symbol("/"), false)
        TEST_RESPONDS_TO(symbol("^"), false)
        TEST_RESPONDS_TO(symbol("a"), true)
        TEST_RESPONDS_TO(symbol("D"), false)
        TEST_RESPONDS_TO(symbol("-"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(10), number(20)}), false)
        TEST_RESPONDS_TO(
            table({symbol("d")}, {number(10)}),
            true)
        
        TEST_INTERFACE("<-:", true)
    }
    
    literal::block bl;
    auto &return_stm = bl.return_statement();
    return_stm.add_expr<literal::number>(10);
    return_stm.add_expr<literal::symbol>("+");
    return_stm.add_expr<literal::number>(10);
    
    {
        auto &&obj = bl.eval();
        TEST_RESPONDS_TO(symbol("pretty_much_anything"), true)
        
        TEST_INTERFACE("<-{}", true)
    }
    
    {
        auto &&obj = bl.eval() << object{};
        auto &&ans = obj << symbol("<<") << symbol("pretty_much_anything");
        
        TEST_CONDITION(
            ans == object{},
            printf(
                "Block instance reports it knows what it responds to (responded %s)\n",
                ans.to_string().c_str()))
        
        auto &&iface = obj << symbol("<<?") << symbol("<-{}");
        
        TEST_CONDITION(
            iface == object{},
            puts("Block instance report it knows whether it follows the <-{} interface"))
    }
    
    {
        auto &&obj = symbol("<-()");
        TEST_RESPONDS_TO(symbol("<<?"), true)
        
        TEST_INTERFACE("<-a", true)
    }
TESTS_END()
