#include "tests_common.hpp"

#include <rbb/error.hpp>

#define TEST_RESPONDS_TO(message, expected)\
    try {\
        TEST_CONDITION(\
            obj << symbol("<<") << message == boolean(expected),\
            printf(\
                "Object %s reports it %s to %s\n",\
                obj.to_string().c_str(),\
                expected? "doesn't respond" : "responds",\
                message.to_string().c_str()))\
    } catch (message_not_recognized_error e) {\
        ++errors;\
        printf(\
            "The object %s doesn't recognize the \"responds to\" message\n",\
            e.receiver.to_string().c_str());\
    }

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
        TEST_RESPONDS_TO(array({number(10), number(20), number(30)}), true)
        TEST_RESPONDS_TO(
            table({symbol("valhalla"), symbol("lol")}, {number(10), number(20)}),
            false)
    }
    
    {
        object obj; // empty
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("+"), false)
        TEST_RESPONDS_TO(object{}, false)
    }
    
    {
        auto &&obj = symbol("valhalla");
        TEST_RESPONDS_TO(symbol("lol"), false)
        TEST_RESPONDS_TO(array({object{}, symbol("a"), number(15)}), false)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(symbol("<"), false)
        TEST_RESPONDS_TO(symbol("<<"), true)
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
        TEST_RESPONDS_TO(number(12), false)
    }
    
    {
        auto &&obj = array({number(10), number(20), symbol("a")});
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("!="), true)
        TEST_RESPONDS_TO(number(0), true)
        TEST_RESPONDS_TO(symbol("*"), true)
        TEST_RESPONDS_TO(symbol("+"), true)
        TEST_RESPONDS_TO(number(3), false)
        TEST_RESPONDS_TO(symbol("-"), false)
        TEST_RESPONDS_TO(symbol("/"), true)
        TEST_RESPONDS_TO(array({number(0), number(30)}), true)
        TEST_RESPONDS_TO(array({symbol("a"), number(12)}), false)
        TEST_RESPONDS_TO(array({number(2)}), false)
        TEST_RESPONDS_TO(array({number(12), number(12), number(12)}), false)
        TEST_RESPONDS_TO(array({number(3), number(100)}), false)
    }
TESTS_END()
