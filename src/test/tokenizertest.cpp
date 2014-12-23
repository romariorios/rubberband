#include "tests_common.hpp"

#include <iostream>
#include <rbb/tokenizer.hpp>

using namespace rbb;
using namespace std;

#define CASE_TOKEN(tok__)\
    case token::t::tok__:\
        cout << #tok__;\
        break;

void print_token(const token &tok)
{
    switch (tok.type) {
    CASE_TOKEN(invalid)
    CASE_TOKEN(bracket_open)
    CASE_TOKEN(curly_open)
    CASE_TOKEN(parenthesis_open)
    CASE_TOKEN(end_of_input)
    CASE_TOKEN(bracket_close)
    CASE_TOKEN(curly_close)
    CASE_TOKEN(parenthesis_close)
    CASE_TOKEN(arrow)
    CASE_TOKEN(comma)
    CASE_TOKEN(exclamation)
    CASE_TOKEN(stm_sep)
    CASE_TOKEN(dollar)
    CASE_TOKEN(tilde)
    CASE_TOKEN(at)
    CASE_TOKEN(percent)
    CASE_TOKEN(bar)
    CASE_TOKEN(colon)
    case token::t::number:
        cout << "number (" << tok.lexem.integer << ")";
        break;
    case token::t::number_f:
        cout << "number (" << tok.lexem.floating << ")";
        break;
    case token::t::symbol:
        cout << "symbol (" << *tok.lexem.str << ")";
        break;
    }

    cout << "; ";
}

void print_tokenization(
    const std::vector<token> &expected,
    const std::vector<token> &actual)
{
    cout
    << "Unexpected tokenization (expected length: " << expected.size()
    << ", actual length: " << actual.size() << ")" << endl;

    cout << "Expected tokens:" << endl << "  ";
    for (auto &tok : expected)
        print_token(tok);
    cout << endl << endl;

    cout << "Actual tokens:" << endl << "  ";
    for (auto &tok : actual)
        print_token(tok);
    cout << endl << endl;
}

#define TEST_TOKENIZATION_OF(actual, expected)\
TEST_CONDITION(\
    actual == expected,\
    print_tokenization(expected, actual))

#define TEST_TOKENIZATION TEST_TOKENIZATION_OF(tok_all, expected)

#include <vector>

TESTS_INIT()
    {
        tokenizer tok {"{ !10 }"};
        std::vector<token> expected {
            token::t::curly_open,
            token::t::exclamation,
            token::number(10),
            token::t::curly_close
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION

        TEST_CONDITION(
            tok.look_next() == expected[0],
            puts("First token by look_next() isn't {"))

        TEST_CONDITION(
            tok.next() == expected[0],
            puts("First token by next() isn't {"))

        TEST_CONDITION(
            tok.look_next() == expected[1],
            puts("Next token by look_next() isn't !"))

        TEST_CONDITION(
            tok.next() == expected[1],
            puts("Next token by next() isn't !"))

        TEST_CONDITION(
            tok.look_next() == expected[2],
            puts("Next token by look_next() isn't 10"))

        TEST_CONDITION(
            tok.next() == expected[2],
            puts("Next token by next() isn't 10"))

        TEST_CONDITION(
            tok.look_next() == expected[3],
            puts("Next token by next() isn't }"))
    }

    {
        tokenizer t{"!10"};
        std::vector<token> expected {
            token::t::exclamation,
            token::number(10)
        };
        auto tok_all = t.all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{
            R"(
                ~:i -> 0
                ~:v -> |20|[]
                ~while~ {! ~i < 20 } {
                    ~v|~i, ~i * (~i)
                }
            )"
        };
        std::vector<token> expected {
            token::t::tilde,
            token::t::colon,
            token::symbol("i"),
            token::t::arrow,
            token::number(0),
            token::t::stm_sep,
            token::t::tilde,
            token::t::colon,
            token::symbol("v"),
            token::t::arrow,
            token::t::bar,
            token::number(20),
            token::t::bar,
            token::t::bracket_open,
            token::t::bracket_close,
            token::t::stm_sep,
            token::t::tilde,
            token::symbol("while"),
            token::t::tilde,
            token::t::curly_open,
            token::t::exclamation,
            token::t::tilde,
            token::symbol("i"),
            token::symbol("<"),
            token::number(20),
            token::t::curly_close,
            token::t::curly_open,
            token::t::tilde,
            token::symbol("v"),
            token::t::bar,
            token::t::tilde,
            token::symbol("i"),
            token::t::comma,
            token::t::tilde,
            token::symbol("i"),
            token::symbol("*"),
            token::t::parenthesis_open,
            token::t::tilde,
            token::symbol("i"),
            token::t::parenthesis_close,
            token::t::curly_close
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{R"(
            # This is a comment
            { ~:x -> $; ~:y -> ~x +
            .$ ! ~x * (~x) } # This is also a comment
        )"};
        std::vector<token> expected {
            token::t::curly_open,
            token::t::tilde,
            token::t::colon,
            token::symbol("x"),
            token::t::arrow,
            token::t::dollar,
            token::t::stm_sep,
            token::t::tilde,
            token::t::colon,
            token::symbol("y"),
            token::t::arrow,
            token::t::tilde,
            token::symbol("x"),
            token::symbol("+"),
            token::t::dollar,
            token::t::exclamation,
            token::t::tilde,
            token::symbol("x"),
            token::symbol("*"),
            token::t::parenthesis_open,
            token::t::tilde,
            token::symbol("x"),
            token::t::parenthesis_close,
            token::t::curly_close
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION

        tokenizer tok2{R"(
            # This is a comment
            { ~:x -> $; ~:y -> ~x +.
            $ ! ~x * (~x) } # This is also a comment
        )"};

        TEST_TOKENIZATION_OF(tok2.look_all(), expected)
    }

    {
        tokenizer tok{"!|[1, 2, a] 2"};
        std::vector<token> expected {
            token::t::exclamation,
            token::t::bar,
            token::t::bracket_open,
            token::number(1),
            token::t::comma,
            token::number(2),
            token::t::comma,
            token::symbol("a"),
            token::t::bracket_close,
            token::number(2)
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{"!:[a -> 10, b -> 20] b"};
        std::vector<token> expected {
            token::t::exclamation,
            token::t::colon,
            token::t::bracket_open,
            token::symbol("a"),
            token::t::arrow,
            token::number(10),
            token::t::comma,
            token::symbol("b"),
            token::t::arrow,
            token::number(20),
            token::t::bracket_close,
            token::symbol("b")
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{"{}:[]; :a -> 10"};
        std::vector<token> expected {
            token::t::curly_open,
            token::t::curly_close,
            token::t::colon,
            token::t::bracket_open,
            token::t::bracket_close,
            token::t::stm_sep,
            token::t::colon,
            token::symbol("a"),
            token::t::arrow,
            token::number(10)
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{R"(
            ~uncurry:
                args -> (~argc)|[],
                i -> 1,
                argc -> ~argc,
                context -> ~context,
                fun -> $
        )"};
        std::vector<token> expected {
            token::t::tilde,
            token::symbol("uncurry"),
            token::t::colon,
            token::symbol("args"),
            token::t::arrow,
            token::t::parenthesis_open,
            token::t::tilde,
            token::symbol("argc"),
            token::t::parenthesis_close,
            token::t::bar,
            token::t::bracket_open,
            token::t::bracket_close,
            token::t::comma,
            token::symbol("i"),
            token::t::arrow,
            token::number(1),
            token::t::comma,
            token::symbol("argc"),
            token::t::arrow,
            token::t::tilde,
            token::symbol("argc"),
            token::t::comma,
            token::symbol("context"),
            token::t::arrow,
            token::t::tilde,
            token::symbol("context"),
            token::t::comma,
            token::symbol("fun"),
            token::t::arrow,
            token::t::dollar
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{R"(
            ~fun:[
                stuff -> 10,
                more_stuff -> 20
            ]()
        )"};
        std::vector<token> expected{
            token::t::tilde,
            token::symbol("fun"),
            token::t::colon,
            token::t::bracket_open,
            token::symbol("stuff"),
            token::t::arrow,
            token::number(10),
            token::t::comma,
            token::symbol("more_stuff"),
            token::t::arrow,
            token::number(20),
            token::t::bracket_close,
            token::t::parenthesis_open,
            token::t::parenthesis_close
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }

    {
        tokenizer tok{R"(
            %^~math
            ~:good_stuff -> :[]
            %^(~good_stuff)external
        )"};
        vector<token> expected{
            token::t::percent,
            token::symbol("^"),
            token::t::tilde,
            token::symbol("math"),
            token::t::stm_sep,
            token::t::tilde,
            token::t::colon,
            token::symbol("good_stuff"),
            token::t::arrow,
            token::t::colon,
            token::t::bracket_open,
            token::t::bracket_close,
            token::t::stm_sep,
            token::t::percent,
            token::symbol("^"),
            token::t::parenthesis_open,
            token::t::tilde,
            token::symbol("good_stuff"),
            token::t::parenthesis_close,
            token::symbol("external")
        };
        auto tok_all = tok.look_all();

        TEST_TOKENIZATION
    }
TESTS_END()
