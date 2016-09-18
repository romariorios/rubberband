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

#ifndef SOURCEFILE_HPP
#define SOURCEFILE_HPP

#include "base.hpp"

#include <error.hpp>
#include <object.hpp>
#include <parse.hpp>

namespace rbb
{

namespace modloader
{

class sourcefile final : public base
{
public:
    sourcefile(base_master *master, const std::string &cfgfile_name);
    sourcefile(base_master *master, std::vector<std::string> &parent_module_paths);

    object load_module(const std::string &modname) const override;
    object program_from_file(const std::string &filename) const;

private:
    base_master &_master;
};

class could_not_open_file : public std::exception
{
public:
    could_not_open_file(const std::string &f)
        : _text{"Could not open file \"" + f + "\""}
    {}

    inline const char *what() const noexcept
    {
        return _text.c_str();
    }

private:
    std::string _text;
};

class sourcefile_syntax_error final : public load_error
{
public:
    sourcefile_syntax_error(const rbb::syntax_error &other, const std::string &filename) :
        _syntax_error{other}
    {
        _syntax_error.append_what("of file " + filename);
    }

    const char *what() const noexcept
    {
        return _syntax_error.what();
    }

private:
    rbb::syntax_error _syntax_error;
};

}

}

#endif
