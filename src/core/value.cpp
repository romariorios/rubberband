// Rubberband language
// Copyright (C) 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "value.hpp"

using namespace rbb;

value_t::value_t(type_t t) :
    type{t}
{}

value_t::value_t(long long val, __integer) :
    type{integer_t}
{
    ::new (st()) long long{val};
}

value_t::value_t(double val, __floating) :
    type{floating_t}
{
    ::new (st()) double{val};
}

value_t::value_t(bool val, __boolean) :
    type{boolean_t}
{
    ::new (st()) bool{val};
}

value_t::value_t(symbol_node sym) :
    type{symbol_t}
{
    ::new (st()) symbol_node{sym};
}

value_t::value_t(shared_data_t *data) :
    type{data_t}
{
    ::new (st()) data_ptr{data};
}

value_t::~value_t()
{
    if (type == data_t)
        data().~shared_ptr<shared_data_t>();
}

void value_t::copy_from(const value_t &other)
{
    type = other.type;
    if (type == data_t)
        ::new (st()) data_ptr{other.data()};
    else
        _s = other._s;
}

value_t::value_t(const value_t &other)
{
    copy_from(other);
}

value_t &value_t::operator=(const value_t &other)
{
    this->~value_t();
    copy_from(other);

    return *this;
}

void value_t::move_from(value_t &other)
{
    type = other.type;
    other.type = no_data_t;
    _s = other._s;
}

value_t::value_t(value_t &&other)
{
    move_from(other);
}

value_t &value_t::operator=(value_t &&other)
{
    this->~value_t();
    move_from(other);

    return *this;
}
