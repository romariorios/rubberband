#ifndef TESTS_COMMON
#define TESTS_COMMON

#include <cstdio>
#include <string>

#include <rbb/error.hpp>
#include <rbb/object.hpp>
#include <rbb/parse.hpp>

using namespace rbb;
using namespace std;

#define TESTS_INIT()\
int main() {\
    int errors = 0;\
    int tests = 0;\
    puts("Running... ");

#define TEST_CONDITION(condition, message)\
    ++tests;\
    if (!(condition)) {\
        ++errors;\
        printf("Test %d: ", tests);\
        message;\
    }

#define TEST_CONDITION_WITH_EXCEPTION(__condition, __message)\
    try {\
        TEST_CONDITION(__condition, __message)\
    } catch (message_not_recognized_error e) {\
        ++errors;\
        printf(\
            "The object %s doesn't recognize the %s message\n",\
            e.receiver.to_string().c_str(),\
            e.message.to_string().c_str());\
    }

#define TEST_MSG_NOT_RECOGNIZED(__obj, __msg) \
{\
    bool not_recognized = false;\
    object res;\
    try {\
        res = (__obj) << (__msg);\
    } catch (message_not_recognized_error) {\
        not_recognized = true;\
    }\
\
    TEST_CONDITION(\
        not_recognized,\
        printf(\
            "Expected rbb::message_not_recognized_error exception; got %s",\
            res.to_string()))\
}

// this assumes obj1 and obj2 are different
bool test_operator_eq(const rbb::object &obj1, const rbb::object &obj2)
{
    if (obj1 == obj2 ||
        obj1 != obj1 ||
        obj2 != obj2)
        return false;

    if (obj1 != obj2 &&
        obj1 == obj1 &&
        obj2 == obj2)
        return true;

    return false;
}

#define TEST_OPERATOR_EQ(obj1, obj2)\
    TEST_CONDITION(test_operator_eq(obj1, obj2), puts("operator== is acting weird"))

#define TESTS_END()\
    float error_rate = ((float) errors / (float) tests);\
    error_rate  *= 100;\
    printf("Result: %d tests, %d failures (%.0f%%).\n", tests, errors, error_rate);\
    return errors;\
}

class dummy_master
{
public:
    static object load(const object &obj, const string &str) { return {}; }
    static object custom_operation(const string &name, const object &obj) { return {}; }
};

#define TEST_PARSER(__master, program, context, message, expected)\
{\
    try {\
        auto prog = parse<__master>(program);\
        auto res = prog << (context) << (message);\
\
        TEST_CONDITION(\
            res == (expected),\
            printf(\
                "The program:\n"\
                "{ %s \n}\n"\
                "interpreted as:\n"\
                "  %s\n"\
                "running over the context:\n"\
                "  %s\n"\
                "when receiving the message:\n"\
                "  %s\n"\
                "returns:\n"\
                "  %s\n"\
                "but the following was expected:\n"\
                "  %s\n",\
                program,\
                prog.to_string().c_str(),\
                context.to_string().c_str(),\
                message.to_string().c_str(),\
                res.to_string().c_str(),\
                expected.to_string().c_str()))\
    } catch (syntax_error e) {\
        TEST_CONDITION(\
            false,\
            printf(\
                "The program:\n"\
                "{ %s \n}\n"\
                "has a syntax error at line %s, column %s (token: %s)\n",\
                program,\
                to_string(e.line).c_str(),\
                to_string(e.column).c_str(),\
                e.t().to_string().c_str()))\
    } catch (std::exception &e) {\
        TEST_CONDITION(\
            false,\
            printf(\
                "The program:\n"\
                "{ %s \n}\n"\
                "threw the following exception:\n"\
                "  %s\n",\
                program,\
                e.what()))\
    }\
}

#define TEST_PROGRAM(program, context, message, expected)\
{\
    TEST_PARSER(dummy_master, program, context, message, expected)\
}

#endif // TESTS_COMMON
