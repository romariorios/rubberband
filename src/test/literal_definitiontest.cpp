#include "tests_common.hpp"

#define TEST_VALUE_PARSING(__string, __value) \
{\
    auto parsed_value = dummy_master.parse(__string);\
    TEST_CONDITION(\
        parsed_value == (__value),\
        printf(\
            "Error trying to parse %s (expected %s, got %s)",\
            __string,\
            (__value).to_string().c_str(),\
            parsed_value.to_string().c_str()))\
}

TESTS_INIT()
    {
        auto define_literal = dummy_master.parse(R"(
        {
            %lit:
                trigger -> 39,
                eval -> { !12 }()
        }
        )");
        define_literal << empty() << empty();

        TEST_VALUE_PARSING("'", number(12))

        auto table_with_literals = dummy_master.parse(":a -> ', b -> ' + 10");
        TEST_CONDITION(
            table_with_literals << "a" == number(12) &&
            table_with_literals << "b" == number(22),
            printf(
                "Error: unexpected parsed value %s",
                table_with_literals.to_string().c_str()))
    }

    {
        auto define_char_literal = dummy_master.parse(R"(
        {
            %lit:
                trigger -> 39,
                eval -> { $skip !$char_val }()  # ~skip: go to next char; ~char_val: current char
        }
        )");
        define_char_literal << empty() << empty();

        TEST_VALUE_PARSING("'c", number('c'))

        try {
            auto define_string_literal = dummy_master.parse(R"(
            {
                %lit:
                    trigger -> '",
                    eval -> {
                        ~:self -> @, input -> $
                        ~input skip
                        ~:cur -> ~input char_val
                        
                        # First, create a linked list until a second " is found
                        !~cur /= '" if_true~ {
                            ~list:
                                size -> ~list size + 1,
                                cell -> (:el -> ~cur, prev -> ~list cell)
                            !~self~(~input)
                        } {
                            # Then, create an array with the size of the list
                            # and insert all chars from the list into it
                            !{
                                ~:self -> @
                                
                                !~cell /= () if_true~ {
                                    ~str|~i, ~cell el
                                    ~:cell -> ~cell prev, i -> ~i - 1
                                    
                                    !~self~()
                                } {
                                    !~str
                                }
                            }(:str -> ~list size(|), i -> ~list size - 1, cell -> ~list cell)()
                        }
                    }(:list -> (:size -> 0, cell -> ()))
            }
            )");
            define_string_literal << empty() << empty();
        } catch (const syntax_error &e) {
            ++errors;\
            printf(\
                "Syntax error at line %ld and column %ld (token: %s)\n",\
                e.line,\
                e.column,\
                e.t.to_string().c_str());
        }

        auto hello_world = dummy_master.parse(R"("Hello, world")");
        TEST_CONDITION_WITH_EXCEPTION(
            hello_world << 0 == number('H') &&
            hello_world << 1 == number('e') &&
            hello_world << 2 == number('l') &&
            hello_world << 3 == number('l') &&
            hello_world << 4 == number('o') &&
            hello_world << 5 == number(',') &&
            hello_world << 6 == number(' ') &&
            hello_world << 7 == number('w') &&
            hello_world << 8 == number('o') &&
            hello_world << 9 == number('r') &&
            hello_world << 10 == number('l') &&
            hello_world << 11 == number('d'),
            printf(
                "Unexpected result for string literal: %s (expecting %s)\n",
                hello_world.to_string().c_str(),
                rbb::array({
                   number('H'), number('e'), number('l'), number('l'), number('o'),
                   number(','), number(' '), number('w'), number('o'), number('r'),
                   number('l'), number('d')
               }).to_string().c_str()))

        // Define user-literals containing language expressions
        // field trigger
        //   Trigger
        // field eval
        //   Evaluate static parts of the literal and determine dynamic ones
        //   current char: ~char_val
        //   skip char: ~skip
        //   go back: ~back
        //   append expression until some char: ~parse_until (some char)
        // field run_eval
        //   Evaluate dynamic parts of the literal
        //   (if empty, will just return the result of the static evaluator)
        //   result of field eval: ~eval_res
        //   eval expression n: ~eval_expr <n>
        auto define_object_wrapper_literal = dummy_master.parse(R"({
            %lit:
                trigger -> 'o,
                eval -> {
                    $skip; $parse_until 'u; $skip
                    # returns nothing because there's no static part in this literal
                }(),
                run_eval -> {
                    # returns value of first expression
                    !$eval_expr 0
                }()
        })");
        define_object_wrapper_literal << empty() << empty();

        auto o_table = dummy_master.parse(R"(o:a -> 10, b -> 20u)");
        TEST_CONDITION_WITH_EXCEPTION(
            o_table << "<<?" << "table" == boolean(true),
            printf(
                "Could not parse custom literal (expecting table, got %s)\n",
                o_table.to_string().c_str()))

        auto define_multiple_expressions = dummy_master.parse(R"({
            %lit:
                trigger -> '[,
                eval -> {
                    $skip; $parse_until ',; $skip; $parse_until ']; $skip
                }(),
                run_eval -> {
                    # Return array with two elements
                    !|$eval_expr 0, $eval_expr 1
                }()
        })");
        define_multiple_expressions << empty() << empty();

        auto obj_pair = dummy_master.parse(R"([10 + 20, 30 * 40])");
        TEST_CONDITION_WITH_EXCEPTION(
            obj_pair << "<<?" << "listable" == boolean(true),
            printf(
                "Could not parse custom literal (expecting listable, got %s)\n",
                obj_pair.to_string().c_str()))
    }
TESTS_END()
