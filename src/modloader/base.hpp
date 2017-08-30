// Rubberband modloader: Load external Rubberband modules
// Copyright (c) 2016, 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include <exception>
#include <memory>

namespace rbb
{

namespace modloader
{

class base
{
public:
    base(const std::string &cfgfile_name = {});
    base(std::vector<std::string> &parent_module_paths);
    ~base();

    virtual object load_module(const std::string &modname) const = 0;

    void autoload(object &context) const;
    void add_path_list(const std::vector<std::string> &paths);
    inline void add_path(const std::string &path) { add_path_list({path}); }

private:
    // Must come before module_paths
    std::unique_ptr<std::vector<std::string>> _module_paths_concrete;

protected:
    std::vector<std::string> &module_paths;

private:
    struct config;
    std::unique_ptr<config> _cfg;
};

class load_error : public std::exception
{
public:
    explicit load_error(const std::string &error_str = {});

    const char *what() const noexcept;

private:
    std::string _error_str;
};

}

}


#endif //RUBBERBAND_BASE_HPP
