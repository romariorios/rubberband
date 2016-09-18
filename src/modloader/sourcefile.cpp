// Rubberband modloader: Load external Rubberband modules
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

#include "sourcefile.hpp"

#include <fstream>

using namespace rbb;
using namespace rbb::modloader;
using namespace std;

sourcefile::sourcefile(base_master *master, const string &cfgfile_name) :
    base{cfgfile_name},
    _master{*master}
{}

sourcefile::sourcefile(base_master *master, vector<string> &parent_module_paths) :
    base{parent_module_paths},
    _master{*master}
{}

object sourcefile::load_module(const string &modname) const
{
    auto file_with_path = modname + ".rbb";

    for (auto path : module_paths) {
        if (ifstream{file_with_path}.good())
            break;

        file_with_path = path + "/" + modname + ".rbb";
    }

    return program_from_file(file_with_path);
}

object sourcefile::program_from_file(const string &filename) const
{
    ifstream file{filename};
    if (!file) {
        throw could_not_open_file{filename};
    }

    string program = "{";

    while (!file.eof()) {
        string tmp;
        getline(file, tmp);
        program += tmp;
        program += "\n";
    }

    program += "}";

    try {
        return _master.parse(program);
    } catch (const syntax_error &e) {
        throw sourcefile_syntax_error{e, filename};
    }
}
