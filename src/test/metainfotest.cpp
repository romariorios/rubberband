#include "tests_common.hpp"

#include <rbb/block.hpp>
#include <array>

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

const char *interfaces[] = {"[a]", "[+]", "[0]", "[?]", "[|]", "[:]", "[{}]", "[=]", "[<]"};
constexpr const unsigned char interfaces_len = sizeof(interfaces) / sizeof(char *);
using expected_t = std::array<char, interfaces_len>;

struct test_interfaces_result
{
    bool result;
    const char *wrong_interface;

    operator bool() const { return result; }
};

test_interfaces_result test_interfaces(object &obj, const expected_t &responds_to)
{
    for (auto i = 0; i < interfaces_len; ++i)
        if (obj << symbol("<<?") << symbol(interfaces[i]) != boolean(responds_to[i]))
            return {false, interfaces[i]};

    return {true, 0};
}

#define TEST_INTERFACES \
{\
    auto result = test_interfaces(obj, expected);\
\
    TEST_CONDITION_WITH_EXCEPTION(\
        result,\
        printf(\
            "The object %s responds incorrectly to %s\n",\
            obj.to_string().c_str(),\
            result.wrong_interface))\
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
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(10), number(20), number(30)}), true)
        TEST_RESPONDS_TO(
            table({symbol("valhalla"), symbol("lol")}, {number(10), number(20)}),
            false)
        
        expected_t expected{0, 1, 1, 0, 0, 0, 0, 1, 1};
        TEST_INTERFACES
    }
    
    {
        object obj; // empty
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(symbol("+"), false)
        TEST_RESPONDS_TO(object{}, false)
        
        expected_t expected{0, 0, 0, 0, 0, 0, 0, 1, 0};
        TEST_INTERFACES
    }
    
    {
        auto &&obj = symbol("valhalla");
        TEST_RESPONDS_TO(symbol("lol"), false)
        TEST_RESPONDS_TO(rbb::array({object{}, symbol("a"), number(15)}), false)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(symbol("<"), false)
        TEST_RESPONDS_TO(symbol("<<"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        
        expected_t expected{1, 0, 0, 0, 0, 0, 0, 1, 0};
        TEST_INTERFACES
    }
    
    {
        auto &&obj = boolean(false);
        TEST_RESPONDS_TO(symbol("?"), true)
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(symbol("><"), true)
        TEST_RESPONDS_TO(symbol("\\/"), true)
        TEST_RESPONDS_TO(symbol("/\\"), true)
        TEST_RESPONDS_TO(symbol("+"), false)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(number(12), false)
        
       expected_t expected{0, 0, 0, 1, 0, 0, 0, 1, 0};
       TEST_INTERFACES
    }
    
    {
        auto &&obj = rbb::array({number(10), number(20), symbol("a")});
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(number(0), true)
        TEST_RESPONDS_TO(symbol("len"), true)
        TEST_RESPONDS_TO(symbol("concat"), true)
        TEST_RESPONDS_TO(number(3), false)
        TEST_RESPONDS_TO(symbol("-"), false)
        TEST_RESPONDS_TO(symbol("slice"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(0), number(30)}), true)
        TEST_RESPONDS_TO(rbb::array({symbol("a"), number(12)}), false)
        TEST_RESPONDS_TO(rbb::array({number(2)}), false)
        TEST_RESPONDS_TO(rbb::array({number(12), number(12), number(12)}), false)
        TEST_RESPONDS_TO(rbb::array({number(3), number(100)}), false)
        
        expected_t expected{0, 0, 0, 0, 1, 0, 0, 1, 0};
        TEST_INTERFACES
    }
    
    {
        auto &&obj = table(
            {symbol("a"), symbol("b"), symbol("c")},
            {number(10), number(20), number(30)});
        TEST_RESPONDS_TO(symbol("=="), true)
        TEST_RESPONDS_TO(symbol("/="), true)
        TEST_RESPONDS_TO(symbol("*"), true)
        TEST_RESPONDS_TO(symbol("/"), false)
        TEST_RESPONDS_TO(symbol("^"), false)
        TEST_RESPONDS_TO(symbol("a"), true)
        TEST_RESPONDS_TO(symbol("D"), false)
        TEST_RESPONDS_TO(symbol("-"), true)
        TEST_RESPONDS_TO(symbol("<<?"), true)
        TEST_RESPONDS_TO(rbb::array({number(10), number(20)}), false)
        TEST_RESPONDS_TO(rbb::array({symbol("a"), symbol("b")}), true)
        TEST_RESPONDS_TO(rbb::array({symbol("a"), symbol("b"), symbol("d")}), true)
        TEST_RESPONDS_TO(rbb::array({symbol("a"), symbol("b"), number(10)}), false)
        TEST_RESPONDS_TO(
            table({symbol("d")}, {number(10)}),
            true)
        
        expected_t expected{0, 0, 0, 0, 0, 1, 0, 1, 0};
        TEST_INTERFACES
    }
    
    literal::block bl;
    auto &return_stm = bl.return_statement();
    return_stm.add_expr<literal::number>(10);
    return_stm.add_expr<literal::symbol>("+");
    return_stm.add_expr<literal::number>(10);
    
    {
        auto &&obj = bl.eval();
        TEST_RESPONDS_TO(symbol("pretty_much_anything"), true)
        
        expected_t expected{0, 0, 0, 0, 0, 0, 1, 1, 0};
        TEST_INTERFACES
    }
    
    {
        auto &&obj = bl.eval() << object{};
        auto &&ans = obj << symbol("<<") << symbol("pretty_much_anything");

        TEST_CONDITION(
            ans == boolean(false),
            printf(
                "Block instance should return false for a message it doesn't know how to respond "
                "(responded %s)\n",
                ans.to_string().c_str()))
        
        auto &&iface = obj << symbol("<<?") << symbol("<-{}");

        TEST_CONDITION(
            iface == boolean(false),
            printf(
                "Block instance should return false when asked if it follows the <-{} interface "
                "(responded %s)\n",
                iface.to_string().c_str()))
    }
TESTS_END()
