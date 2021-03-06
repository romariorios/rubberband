#include "tests_common.hpp"

#include <cstring>
#include <rbb/symbol.hpp>

#define TEST_SYMBOL_COMPARISON(sym1, sym2, symb, eq)\
TEST_CONDITION(\
    rbb::symbol(sym1) << rbb::symbol(symb) << rbb::symbol(sym2) == rbb::boolean(eq),\
    printf("%s %s %s doesn't equal %s!\n", sym1, symb, sym2, eq? "true" : "false"))

#define COMPARE_EQ_AND_NE(sym1, sym2)\
{\
bool eq;\
eq = strcmp(sym1, sym2) == 0;\
TEST_SYMBOL_COMPARISON(sym1, sym2, "==", eq)\
TEST_SYMBOL_COMPARISON(sym1, sym2, "/=", !eq)\
}

#define COMPARE_WITH_SPECIAL_SYMBOLS(sym)\
COMPARE_EQ_AND_NE(sym, "==")\
COMPARE_EQ_AND_NE(sym, "/=")\
COMPARE_EQ_AND_NE(sym, "?")\
COMPARE_EQ_AND_NE(sym, ">")\
COMPARE_EQ_AND_NE(sym, "<")\
COMPARE_EQ_AND_NE(sym, "<=")\
COMPARE_EQ_AND_NE(sym, ">=")\
COMPARE_EQ_AND_NE(sym, "+")\
COMPARE_EQ_AND_NE(sym, "-")\
COMPARE_EQ_AND_NE(sym, "*")\
COMPARE_EQ_AND_NE(sym, "/")\
COMPARE_EQ_AND_NE(sym, "?|")\
COMPARE_EQ_AND_NE(sym, "?:")

TESTS_INIT()
    TEST_CONDITION(rbb::symbol("abcd") == rbb::symbol("abcd"), puts("Error when comparing equal symbols"))

    rbb::object abcd_comp = rbb::symbol("abcd") << rbb::symbol("==") << rbb::symbol("abcd");

    TEST_CONDITION(
        abcd_comp.__value.type == rbb::value_t::boolean_t,
        puts("Symbol comparison doesn't return a bool"))
    TEST_CONDITION(
        abcd_comp == rbb::boolean(true),
        puts("Error when comparing equal symbols sending messages"))
    TEST_CONDITION(
        rbb::symbol("a12345") << rbb::symbol("/=") << rbb::symbol("a1234") == rbb::boolean(true),
        puts("Error when cheking two symbols for inequality"))

    // Test all special symbols
    COMPARE_WITH_SPECIAL_SYMBOLS("==")
    COMPARE_WITH_SPECIAL_SYMBOLS("/=")
    COMPARE_WITH_SPECIAL_SYMBOLS("?")
    COMPARE_WITH_SPECIAL_SYMBOLS(">")
    COMPARE_WITH_SPECIAL_SYMBOLS("<")
    COMPARE_WITH_SPECIAL_SYMBOLS("<=")
    COMPARE_WITH_SPECIAL_SYMBOLS(">=")
    COMPARE_WITH_SPECIAL_SYMBOLS("+")
    COMPARE_WITH_SPECIAL_SYMBOLS("-")
    COMPARE_WITH_SPECIAL_SYMBOLS("*")
    COMPARE_WITH_SPECIAL_SYMBOLS("/")

    TEST_OPERATOR_EQ(rbb::symbol("=="), rbb::symbol("/="))
    TEST_OPERATOR_EQ(rbb::symbol("abcdef"), rbb::symbol("abcdefg"))
    TEST_OPERATOR_EQ(rbb::symbol("asa"), rbb::empty())
    TEST_OPERATOR_EQ(rbb::empty(), rbb::symbol("aaa"))
TESTS_END()
