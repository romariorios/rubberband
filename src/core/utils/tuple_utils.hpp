// Rubberband language
// Copyright (C) 2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef TUPLE_UTILS_HPP
#define TUPLE_UTILS_HPP

#include <functional>
#include <tuple>
#include <utility>

namespace rbb
{

enum class control
{
    continue_loop = 0,
    break_loop
};

template <std::size_t I, typename Func, typename... Types>
typename std::enable_if<I == sizeof...(Types), void>::type for_each(
    const std::tuple<Types...> &,
    Func)
{}

template <std::size_t I = 0, typename Func, typename... Types>
typename std::enable_if<I < sizeof...(Types), void>::type for_each(
    const std::tuple<Types...> &tuple,
    Func f)
{
    auto ctl = f(std::get<I>(tuple));

    if (ctl == control::break_loop)
        return;

    for_each<I + 1, Func, Types...>(tuple, f);
}

}

#endif
