// Rubberband language
// Copyright (C) 2014  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "block.hpp"
#include "block_private.hpp"
#include "object.hpp"
#include "object_private.hpp"
#include "symbol.hpp"

#include <typeinfo>

#include <cstdio>

namespace rbb
{

std::string symbol_node::to_string() const
{
    if (!up)
        return std::string{};

    return up->to_string() + std::string{ch};
}

std::string array_data::to_string() const
{
    std::string result{"|["};

    for (int i = 0; i < size; ++i) {
        result += arr[i].to_string();

        if (i + 1 < size)
            result += ", ";
    }

    result += "]";

    return result;
}

std::string table_data::to_string() const
{
    std::string result{":["};

    for (auto &entry : objtree) {
        result += entry.first->to_string() + " -> " + entry.second.to_string();
        result += ", ";
    }

    if (!objtree.empty())
        result.erase(result.size() - 2);

    result += "]";

    return result;
}

std::string block_statement::to_string() const
{
    std::string result;

    for (auto &expression : expressions) {
        auto sub_expression = typeid(*expression) == typeid(block_statement);

        if (sub_expression)
            result += "(";

        result += expression->to_string();

        if (sub_expression)
            result += ")";

        result += " ";
    }

    if (!expressions.empty())
        result.erase(result.size() - 1);

    return result;
}

namespace literal
{

std::string array::to_string() const
{
    std::string result{"|["};

    for (auto &element : _objects) {
        result += element->to_string() + ", ";
    }

    if (!_objects.empty())
        result.erase(result.size() - 2);

    result += "]";

    return result;
}

std::string table::to_string() const
{
    std::string result{":["};

    for (
        auto sym = _symbols.cbegin(), obj = _objects.cbegin();
        sym != _symbols.cend() && obj != _objects.cend();
        ++sym, ++obj
    ) {
        result += (*sym)->to_string() + " -> " + (*obj)->to_string();

        auto next_sym = sym;
        ++next_sym;

        auto next_obj = obj;
        ++next_obj;

        if (next_sym != _symbols.end() && next_obj != _objects.end())
            result += ", ";
    }

    result += "]";

    return result;
}

std::string block::to_string() const
{
    std::string result{"{ "};

    for (auto &stm : _p->statements) {
        result += stm.to_string() + "; ";
    }

    if (!_p->statements.empty())
        result.erase(result.size() - 2);

    auto ret_to_string = _p->return_statement->to_string();

    if (!ret_to_string.empty())
        result += "!" + ret_to_string;

    result += " }";

    return result;
}

}

std::string block_data::to_string() const
{
    return block_l->to_string();
}


std::string object::to_string() const
{
    const auto type = __value.type;

    switch (type) {
    case value_t::empty_t:
        return "()";
    case value_t::integer_t:
        return std::to_string(__value.integer);
    case value_t::floating_t:
        return std::to_string(__value.floating);
    case value_t::symbol_t:
        return __value.symbol->to_string();
    case value_t::boolean_t:
        return __value.boolean? "[T]" : "[F]";
    case value_t::data_t:
        return __value.data->to_string();
    default:
        return "[unknown]";
    }
}

}
