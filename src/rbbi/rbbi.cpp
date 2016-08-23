// Rubberband interpreter
// Copyright (c) 2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// boost includes
#include <boost/format.hpp>

// readline includes
#include <readline/history.h>
#include <readline/readline.h>

// rubberband includes
#include <error.hpp>
#include <parse.hpp>

// std includes
#include <exception>
#include <iostream>
#include <string>

using namespace boost;
using namespace rbb;
using namespace std;

class rbbi_master : public base_master
{
public:
    object load(const string &) override { return empty(); }
    object custom_operation(const string &op, const object &) override
    {
        if (op == "exit")
            exit(0);
    }
} master;

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

        code = (format("{ !%s }") % code).str();

        try {
            auto block = master.parse(code);
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

