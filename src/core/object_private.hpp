// Rubberband language
// Copyright (C) 2014--2015, 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef OBJECT_PRIVATE_HPP
#define OBJECT_PRIVATE_HPP

#include <map>
#include <memory>
#include <unordered_set>

#include "shared_data_t.hpp"

namespace rbb
{

class object;

namespace literal
{
    class block;
}

object empty();

class array_data : public shared_data_t
{
public:
    array_data(const int size) :
        size(size)
    {
        arr = new object[size];
    }

    ~array_data()
    {
        delete[] arr;
    }
    
    std::string to_string(
        std::shared_ptr<std::unordered_set<const object *>> visited = nullptr) const;

    object *arr;
    int size;
};

class table_data : public shared_data_t
{
public:
    std::string to_string(
        std::shared_ptr<std::unordered_set<const object *>> visited = nullptr) const;
    
    std::map<symbol_node, object> objtree;
};

class block_data : public shared_data_t
{
public:
    block_data(literal::block *block_l) : block_l(block_l) {}
    ~block_data()
    {
        delete block_l;
    }
    
    std::string to_string(
        std::shared_ptr<std::unordered_set<const object *>> = nullptr) const;

    literal::block *block_l;
};

static bool is_numeric(const object &val)
{
    return val.__value.type & value_t::integer_t || val.__value.type & value_t::floating_t;
}

static bool in_bounds(object obj, object index)
{
    auto size = obj << "*";
    if (!is_numeric(size))
        return false;
    
    return index << ">=" << 0 << "/\\" << (index << "<" << size) == boolean(true);
}

static int get_index_from_obj(const object &obj)
{
    auto obj_copy = obj;
    if (!is_numeric(obj_copy))
        return -1;

    if (obj.__value.type & value_t::floating_t)
        return obj.__value.floating;

    return obj.__value.integer;
}

static bool table_contains_symbol(table_data *d, const object &msg)
{
    return
        msg.__value.type & value_t::symbol_t &&
        d->objtree.find(msg.__value.symbol) != d->objtree.end();
}

}

#endif
