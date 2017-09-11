// Rubberband modloader: Load external Rubberband modules
// Copyright (c) 2016--2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
#include <object_private.hpp>

using namespace rbb;
using namespace rbb::modloader;
using namespace std;

sourcefile::sourcefile(base_master *master, const string &cfgfile_name) :
    base{master, cfgfile_name}
{}

sourcefile::sourcefile(base_master *master, vector<string> &parent_module_paths) :
    base{master, parent_module_paths}
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

class sourcefile_module_data : public shared_data_t
{
public:
    sourcefile_module_data(
        base_master &master,
        const object &first_chunk,
        const vector<string> &following_chunks) :

        _master{master},
        first_chunk{first_chunk},
        following_chunks(following_chunks)
    {}

    base_master &_master;
    object context;
    object first_chunk;
    vector<string> following_chunks;
};

object sourcefile_module_exec_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<sourcefile_module_data>();

    object ret = d->first_chunk << d->context << msg;

    for (const auto &c : d->following_chunks)
        ret = d->_master.parse("{" + c + "}") << d->context << msg;

    return ret;
}

object sourcefile_module_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<sourcefile_module_data>();

    d->context = msg;
    return object{
        value_t{new sourcefile_module_data{*d}},
        sourcefile_module_exec_send_msg};
}

static vector<string> split(const string &str, const string &delim)
{
    const auto delim_pos = str.find(delim);

    if (delim_pos == string::npos)
        return {str};

    vector<string> ret{str.substr(0, delim_pos)};
    const auto tail = split(str.substr(delim_pos + delim.size(), string::npos), delim);

    ret.insert(ret.end(), tail.begin(), tail.end());

    return ret;
}

object sourcefile::program_from_file(const string &filename) const
{
    ifstream file{filename};
    if (!file) {
        throw could_not_open_file{filename};
    }

    string program;

    while (!file.eof()) {
        string tmp;
        getline(file, tmp);
        program += tmp;
        program += "\n";
    }

    try {
        const auto chunks = split(program, ";;");
        auto first_chunk = master.parse("{" + chunks[0] + "}");

        return object{
            value_t{
                new sourcefile_module_data{
                    master, first_chunk,
                    vector<string>(chunks.cbegin() + 1, chunks.cend())}},
            sourcefile_module_send_msg};
    } catch (const syntax_error &e) {
        throw sourcefile_syntax_error{e, filename};
    }
}
