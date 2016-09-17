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

#ifndef RUBBERBAND_BASE_HPP
#define RUBBERBAND_BASE_HPP

#include <error.hpp>
#include <parse.hpp>

namespace rbb
{

namespace modloader
{

class base
{
public:
    base(const std::string &cfgfile_name);

    virtual object load_module(const std::string &modname) const = 0;

    void autoload(object &context) const;
    void add_path_list(const std::vector<std::string> &paths);
    inline void add_path(const std::string &path) { add_path_list({path}); }

protected:
    std::vector<std::string> module_paths;
};

}

}


#endif //RUBBERBAND_BASE_HPP
