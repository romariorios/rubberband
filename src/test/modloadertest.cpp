#include "tests_common.hpp"

#include <rbb/modloader/sourcefile.hpp>

#include <fstream>

class modloader_master_t : public dummy_master_t
{
public:
    modloader::sourcefile loader{this, {}};

    object load(const string &mod) override
    {
        return loader.load_module(mod);
    }
};

TESTS_INIT()
    {
        modloader_master_t master;

        ofstream file{"double-semicolon.rbb", ios_base::trunc};
        file <<
            "%lit:\n"
            "  trigger = 39,\n"
            "  eval = {\n"
            "    $skip\n"
            "    !$char_val\n"
            "  }();;\n\n"

            "!'a\n";
        file.close();

        auto double_semicolon_test = master.loader.program_from_file("double-semicolon.rbb");
        auto result = double_semicolon_test << empty() << empty();

        TEST_CONDITION(
            result == number(97),
            puts("Error when running program"));
    }

    {
        modloader_master_t master;

        ofstream mod_file{"module_test.rbb", ios_base::trunc};
        mod_file <<
R"(
~:a = 10
~:b = 20
~:c = |10, 20, 30
~:d = {!$0 + ($1) + ($2)}()
)";
        mod_file.close();

        ofstream prog_file{"program-test.rbb", ios_base::trunc};
        prog_file <<
R"(
%load module_test~()

!~d|~a, ~b, ~c 2
)";
        prog_file.close();

        auto module_test = master.loader.program_from_file("program-test.rbb");
        auto result = module_test << table() << empty();

        TEST_CONDITION(
            result == number(60),
            puts("Error when loading module"))
    }
TESTS_END()
