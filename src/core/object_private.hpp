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

#ifndef OBJECT_PRIVATE_HPP
#define OBJECT_PRIVATE_HPP

#include <map>

namespace rbb
{

class object;
class symbol_node;

namespace literal
{
    class block;
}

object empty();

class shared_data_t
{
public:
    shared_data_t() :
        refc(1)
    {}

    virtual ~shared_data_t()
    {}

    int refc;
};

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

    object *arr;
    int size;
};

class table_data : public shared_data_t
{
public:
    std::map<symbol_node *, object> objtree;
};

class block_data : public shared_data_t
{
public:
    block_data(literal::block *block_l) : block_l(block_l) {}
    ~block_data()
    {
        delete block_l;
    }

    literal::block *block_l;
};

}

#endif
