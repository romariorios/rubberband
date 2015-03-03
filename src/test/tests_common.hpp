#ifndef TESTS_COMMON
#define TESTS_COMMON

#include <cstdio>

#include <rbb/object.hpp>

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

#endif // TESTS_COMMON
