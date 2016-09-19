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

#ifndef RUBBERBAND_MULTI_HPP
#define RUBBERBAND_MULTI_HPP

#include "base.hpp"

#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace rbb
{

namespace modloader
{

class multi final : public base
{
public:
    explicit multi(const std::string &cfgfile_path = {});

    object load_module(const std::string &modname) const override;

    // Loader with no constructor args
    template <typename LoaderT>
    void add_loader()
    {
        _loaders.emplace_back(new LoaderT{module_paths});
    }

    // Loader with one or more constructor args
    template <typename LoaderT, typename TArg, typename... TOtherArgs>
    LoaderT &add_loader(TArg &&a, TOtherArgs &&... otherA)
    {
        // Pointer will be passed to emplace_back right away, so it should be okay
        auto new_loader = new LoaderT{
            std::forward<TArg>(a),
            std::forward<TOtherArgs>(otherA)...,
            module_paths};
        _loaders.emplace_back(new_loader);

        return *new_loader;
    };

private:
    std::vector<std::unique_ptr<base>> _loaders;
};

class multi_load_error final : public load_error
{
public:
    multi_load_error();

    void add_error(const load_error &err);
    const char *what() const noexcept;

private:
    std::string _err_str;
};

}

}

#endif //RUBBERBAND_MULTI_HPP
