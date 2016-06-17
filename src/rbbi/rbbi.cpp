// boost includes
#include <boost/format.hpp>

// readline includes
#include <readline/history.h>
#include <readline/readline.h>

// rubberband includes
#include <parse.hpp>

// std includes
#include <exception>
#include <iostream>
#include <string>

using namespace boost;
using namespace rbb;
using namespace std;

class rbbi_master
{
public:
    static object load(const object &, const string &) {}
    static object custom_operation(const string &op, const object &)
    {
        if (op == "exit")
            exit(0);
    }
};

int main(int, char **)
{
    auto context = table();

    for (;;) {
        auto line = readline("!");
        auto code = string{line};
        add_history(line);
        free(line);

        if (code.empty())
            continue;

        code = (format("!%s") % code).str();

        try {
            auto block = parse<rbbi_master>(code);
            auto result = block << context << empty();

            cout << format("==> %s") % result.to_string();
        } catch (const syntax_error &e) {
            cout << format("%s^\nSyntax error") % string(e.column - 1, ' ');
        } catch (const rbb::runtime_error &e) {
            cout
                << format("Runtime raised the following error object: %s")
                   % e.error_obj.to_string();
        } catch (const std::exception &e) {
            cout << "Error: " << e.what();
        }

        cout << "\n\n";
    }
}

