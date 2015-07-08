// In these tests, non-native objects that act native (ducks) are tested

#include "tests_common.hpp"

TESTS_INIT()
    TEST_PROGRAM(R"(
        ~:fake_empty -> ().{
          !$ == <<? ?(:arg -> $) {
            !().{ !$ == <-() }
          } {
            !~arg == ()
          }
        }

        !~fake_empty <<? <-()
    )", table({}, {}), empty(), boolean(true))

    TEST_PROGRAM(R"(
        # Will be equivalent to |0, 10
        ~:fake_array -> ().{
          !$ == <<? ?(:arg -> $) {
            !().{ !$ == <-| }
          } {
            !~arg == *?~ {
              !2
            } {
              !~arg <<? <-0?~ {
                !~arg * 10
              } { }
            }
          }
        }
        ~:array -> |0, 10, 20, 30
        ~array(~fake_array)  # Equivalent to ~array(|0, 10), so it should set the first element to 10

        !~array 0
    )", table({}, {}), empty(), number(10))

    TEST_PROGRAM(R"(
        ~:fake_table -> ().{
          !$ == <<? ?(:arg -> $) {
            !().{ !$ == <-: }
          } {
            !~arg == *?~ {
              !|a
            } {
              !~arg == a?~ {
                !15
              } { }
            }
          }
        }

        ~(~fake_table)  # Equivalent to ~:a -> 15

        !~a
    )", table({}, {}), empty(), number(15))
TESTS_END()
