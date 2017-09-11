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

#include "multi.hpp"

using namespace rbb;
using namespace rbb::modloader;
using namespace std;

multi::multi(base_master *master, const std::string &cfgfile_path) :
    base{master, cfgfile_path}
{}

object multi::load_module(const std::string &modname) const
{
    multi_load_error error;

    for (auto &&loader : _loaders) {
        try {
            return loader->load_module(modname);
        } catch (const load_error &err) {
            error.add_error(err);
        }
    }

    throw error;
}

multi_load_error::multi_load_error() :
    _err_str{"The following errors occurred while trying to load the module: "}
{}

void multi_load_error::add_error(const load_error &err)
{
    _err_str += err.what();
    _err_str += "; ";
}

const char *multi_load_error::what() const noexcept
{
    return _err_str.c_str();
}
