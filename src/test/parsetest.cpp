#include "tests_common.hpp"

extern void LemonCParserTrace(FILE *stream, char *zPrefix);

object __print(object *, object &msg)
{
    puts(msg.to_string().c_str());
    
    return object{};
}

class test_master_t : public base_master
{
public:
    object load(const string &str) override
    {
        if (str != "__parsetestmodule")
            return {};
        
        return parse(R"({ ~:a -> 10 })");
    }

    object custom_operation(const string &, object &) override
    {
        return {};
    }
} test_master;

#define TEST_PARSING(__program, __to_string)\
{\
    auto interpreted_as = dummy_master.parse(__program).to_string();\
\
    TEST_CONDITION(\
        interpreted_as == (__to_string),\
        printf(\
            "The text %s has been interpreted as %s (expected: %s)\n",\
            __program,\
            interpreted_as.c_str(),\
            __to_string))\
}

TESTS_INIT()
    object print;
    print.__send_msg = __print;

    TEST_PROGRAM("{}", empty(), empty(), empty())
    TEST_PROGRAM("{ !10 }", empty(), empty(), number(10))
    TEST_PROGRAM("{ !10 }", empty(), empty(), number(10))
    TEST_PROGRAM("{ !(|1, 2, a) 0 }", empty(), empty(), number(1))
    TEST_PROGRAM("{ !(|1, 2, a) 1 }", empty(), empty(), number(2))
    TEST_PROGRAM("{ !(|1, 2, a) 2 }", empty(), empty(), symbol("a"))
    TEST_PROGRAM("{ !(|1, 2, a) << 3 }", empty(), empty(), boolean(false))
    TEST_PROGRAM("{ !(:a -> 10, b -> 20) a }", empty(), empty(), number(10))
    TEST_PROGRAM("{ !(:a -> 10, b -> 20) b }", empty(), empty(), number(20))
    TEST_PROGRAM("{ !(:a -> 10, b -> 20) << lol }", empty(), empty(), boolean(false))
    TEST_PROGRAM("{ !$ }", empty(), number(10), number(10));
    TEST_PROGRAM("{ !~ }", number(10), empty(), number(10));
    TEST_PROGRAM("{ ~:x -> $ !~x * $ }", table({}, {}), number(10), number(100))
    TEST_PROGRAM("{ ~:x -> $ !~x * (~x) }", table({}, {}), number(10), number(100))
    TEST_PROGRAM(R"(
    {
        ~:fibnums -> (|0, 1, 1)
        ~:n -> $

        !~n <= 2 if_true~ {
            !~fibnums(~i)
        } {
            ~:i -> 2

            ~:loop -> {
                ~fibnums|0, ~fibnums 1
                ~fibnums|1, ~fibnums 2
                ~fibnums|2, ~fibnums 0 + (~fibnums 1)

                ~:i -> ~i + 1
                ~i < (~n) if_true~ {
                    ~loop()
                }
            }~
            ~loop()

            !~fibnums 2
        }
    })", table({}, {}), number(43), number(433494437))
    TEST_PROGRAM(R"(
    {
        ~:while -> {
            ~:ctx -> $0, cond_bl -> $1, exec_bl -> $2

            ~:loop -> {
                ~cond_bl(~ctx)() if_true~ {
                    ~exec_bl(~ctx)()

                    ~loop()
                }
            }~
            ~loop()
        }(:)

        ~:fibnums -> |0, 1, 1
        ~:n -> $

        ~while|~, { !~n > 2 }, {
            ~fibnums|0, ~fibnums 1
            ~fibnums|1, ~fibnums 2
            ~fibnums|2, ~fibnums 0 + (~fibnums 1)

            ~:n -> ~n - 1
        }

        !~fibnums 2
    })", table({}, {}), number(43), number(433494437))
    TEST_PROGRAM(R"(
    {
        ~:v -> 3(|)
        ~v|0, 10
        ~v|1, 20
        ~v|2, 30

        !~v 0
    })", table({}, {}), empty(), number(10))
    TEST_PROGRAM(R"(
    {
        ~:v -> 3(|)
        ~v|0, 10
        ~v|1, 20
        ~v|2, 30

        !~v 1
    })", table({}, {}), empty(), number(20))
    TEST_PROGRAM(R"(
    {
        ~:v -> |.3
        ~v|0, 10
        ~v|1, 20
        ~v|2, 30

        !~v 2
    })", table({}, {}), empty(), number(30))
    
    TEST_PARSER(test_master, R"(
    {
        %^__parsetestmodule~()
        !~a
    })", table({}, {}), empty(), number(10))

    TEST_PARSING("()", "()")
    TEST_PARSING("12", "12")
    TEST_PARSING("|a, b, c, 1, 2, 3", "(|a, b, c, 1, 2, 3)")
    TEST_PARSING(":a -> 10, b -> 20, c -> 30", "(:a -> 10, b -> 20, c -> 30)")
    TEST_PARSING("?0", "?0")
    TEST_PARSING("{ :.{}; :a -> 10 }", "{ ({  } (:)); (:a -> 10) }")

    TEST_PROGRAM(R"(
    {
        !2.(|0, 10, this_one, 20)
    })", empty(), empty(), symbol("this_one"))

    TEST_PROGRAM(R"(
    {
        (~
        :a -> 10)
        !(~
        a)
    })", table({}, {}), empty(), number(10))

    TEST_PROGRAM(R"(
        { !?0 }
    )", empty(), empty(), boolean(false))

    TEST_PROGRAM(R"(
        { !?1 }
    )", empty(), empty(), boolean(true))
TESTS_END()
