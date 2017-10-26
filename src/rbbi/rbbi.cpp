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

// readline includes
#include <readline/history.h>
#include <readline/readline.h>

// rubberband includes
#include <error.hpp>
#include <parse.hpp>
#include <modloader/multi.hpp>
#include <modloader/native_linux.hpp>
#include <modloader/sourcefile.hpp>

// std includes
#include <exception>
#include <iostream>
#include <string>

using namespace rbb;
using namespace std;

class rbbi_master : public base_master
{
public:
    rbbi_master() :
        loader_{this, ".cfg.json"}
    {
        loader_.add_loader<modloader::native_linux>();
        loader_.add_loader<modloader::sourcefile>();
    }

    void autoload(object &obj)
    {
        loader_.autoload(obj);
    }

    object load(const string &mod) override
    {
        return loader_.load_module(mod);
    }

    object custom_operation(const string &op, object &obj) override
    {
        if (op == "add_mod_search_path") {
            if (obj << symbol("has_iface") << symbol("listable") == boolean(false))
                throw rbb::runtime_error{parse(":error = invalid_path")};

            string path;
            auto size_obj = obj << symbol("len");
            const auto size = size_obj.__value.integer();

            for (long long i = 0; i < size; ++i) {
                auto path_symb = obj << number(i);
                if (path_symb << symbol("has_iface") << symbol("symbol") == boolean(false))
                    throw rbb::runtime_error{parse(":error = invalid_path")};

                if (path_symb == symbol("[..]"))
                    path += "..";
                else
                    path += path_symb.to_string();

                path += "/";
            }

            loader_.add_path(path);

            return {};
        }

        if (op == "exit")
            exit(0);

        throw message_not_recognized_error{symbol("Master object"), obj};
    }

private:
    modloader::multi loader_;
} master;

void print_rbb_string(object &&str)
{
    int len = number_to_double(str << "len");
    for (int i = 0; i < len; ++i)
        putchar(static_cast<char>(number_to_double(str << i)));
}

int main(int, char **)
{
    auto context = table();
    master.autoload(context);

    master.load("string") << context << empty();
    auto obj_to_string = context << "obj_to_string";

    for (;;) {
        auto line = readline("!");
        auto code = string{line};
        add_history(line);
        free(line);

        if (code.empty())
            continue;

        code = "{ !" + code + " }";

        try {
            auto block = master.parse(code);
            auto result = block << context << empty();

            cout << "==> ";
            print_rbb_string(obj_to_string << result);
        } catch (const syntax_error &e) {
            cout << string(e.column - 1, ' ') << "^\nSyntax error\n";
        } catch (const rbb::runtime_error &e) {
            cout
                << "Runtime raised the following error object: "
                << e.error_obj.to_string() << "\n";
        } catch (const std::exception &e) {
            cout << "Error: " << e.what();
        }

        cout << "\n\n";
    }
}

