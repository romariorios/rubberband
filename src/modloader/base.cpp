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

#include "base.hpp"

#include <json.hpp>
#include <fstream>

using namespace nlohmann;
using namespace rbb;
using namespace rbb::modloader;
using namespace std;

struct base::config
{
    json cfg;
};

base::base(base_master *master, const std::string &cfgfile_name) :
    _module_paths_concrete{new vector<string>},
    module_paths{*_module_paths_concrete.get()},
    master{*master},
    _cfg{new config}
{
    ifstream cfgfile{cfgfile_name};
    if (cfgfile.good())
        cfgfile >> _cfg->cfg;

    auto &&modpaths = _cfg->cfg["modpaths"];
    if (modpaths.type() == json::value_t::array)
        add_path_list(modpaths);
}

base::base(base_master *master, std::vector<std::string> &parent_module_paths) :
    module_paths{parent_module_paths},
    master{*master},
    _cfg{new config}
{}

base::~base()
{}

void base::autoload(object &context) const
{
    for (const auto &m : _cfg->cfg["autoload"])
        load_module(m) << context << object{};
}

void base::add_path_list(const vector<string> &paths)
{
    module_paths.insert(module_paths.cbegin(), paths.begin(), paths.end());
}

load_error::load_error(const std::string &error_str) :
    _error_str{error_str}
{}

const char *load_error::what() const noexcept
{
    return _error_str.c_str();
}
