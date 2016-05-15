// boost includes
#include <boost/format.hpp>

// rubberband includes
#include <parse.hpp>

// std includes
#include <iostream>
#include <string>

using namespace boost;
using namespace rbb;
using namespace std;

class rbbi_master
{
public:
    static object load(const object &, const string &) {}
    static object custom_operation(const string &, const object &) {}
};

int main(int, char **)
{
    auto context = table();

    for (;;) {
        cout << "! ";
        string code;
        getline(cin, code);

        if (code.empty())
            continue;

        try {
            auto block = parse<rbbi_master>((format("!%s") % code).str());
            auto result = block << context << empty();

            cout << format("==> %s") % result.to_string();
        } catch (const rbb::runtime_error &e) {
            cout
                << format("Runtime raised the following error object: %s")
                   % e.error_obj.to_string();
        } catch (...) {
            cout << "Error";
        }

        cout << "\n\n";
    }
}

