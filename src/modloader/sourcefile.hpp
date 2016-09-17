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

#include <error.hpp>
#include <object.hpp>
#include <parse.hpp>

#include <fstream>
#include <json.hpp>

namespace rbb
{

namespace modloader
{

    namespace __internal
    {
        using load_module_fun =
            bool(
                const std::vector<std::string> & /* modpaths */,
                const std::string & /* modname */,
                object & /* ret */);

        // Base case
        template <load_module_fun F>
        bool load_module(const std::vector<std::string> &modpaths, const std::string &modname, object &obj)
        {
            const auto res = F(modpaths, modname, obj);
            if (!res)
                obj = empty();

            return res;
        }

        // Recursive definition
        template <load_module_fun F1, load_module_fun F2, load_module_fun... Funs>
        bool load_module(const std::vector<std::string> &modpaths, const std::string &modname, object &obj)
        {
            if (load_module<F1>(modpaths, modname, obj))
                return true;

            return load_module<F2, Funs...>(modpaths, modname, obj);
        }
    }

template <__internal::load_module_fun... Funs>
class loader final
{
public:
    loader(base_master *master, const std::string &cfgfile_name) :
        _master{*master}
    {
        std::ifstream cfgfile{cfgfile_name};
        if (cfgfile.good())
            cfgfile >> _cfg;

        auto &&modpaths = _cfg["modpaths"];
        if (modpaths.type() == nlohmann::json::value_t::array)
            add_path_list(modpaths);
    }

    void autoload(object &context) const;

    object load_module(const std::string &modname) const
    {
        object obj;
        __internal::load_module<Funs>(_module_paths, modname, obj);

        return obj;
    }

    object program_from_file(const std::string &filename) const;

    void add_path_list(const std::vector<std::string> &paths);
    inline void add_path(const std::string &path) { add_path_list({path}); }

private:
    base_master &_master;
    std::vector<std::string> _module_paths;
    nlohmann::json _cfg;
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

class sourcefile_syntax_error : public rbb::syntax_error
{
public:
    sourcefile_syntax_error(const rbb::syntax_error &other, const std::string &filename) :
        syntax_error{other}
    {
        append_what("of file " + filename);
    }
};

}

}

#endif
