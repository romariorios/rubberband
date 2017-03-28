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

#ifndef VALUE_HPP
#define VALUE_HPP

#include "shared_data_t.hpp"
#include "symbol.hpp"

#include <memory>

namespace rbb {

struct value_t
{
    enum type_t
    {
        no_type_t,
        no_data_t,
        empty_t,
        integer_t,
        floating_t,
        boolean_t,
        symbol_t,
        data_t
    } type = value_t::empty_t;

    using data_ptr = std::shared_ptr<shared_data_t>;
    std::aligned_union<0, long long, double, bool, symbol_node, data_ptr>::type _s;

    const void *st() const
    {
        return static_cast<const void *>(&_s);
    }

    void *st()
    {
        return static_cast<void *>(&_s);
    }

    static struct __integer{} integer_v;
    static struct __floating{} floating_v;
    static struct __boolean{} boolean_v;

    explicit value_t(type_t t);
    value_t(long long val, __integer);
    value_t(double val, __floating);
    value_t(bool val, __boolean);
    explicit value_t(symbol_node sym);
    explicit value_t(shared_data_t *data);
    ~value_t();

    void copy_from(const value_t &other);
    value_t(const value_t &other);
    value_t &operator=(const value_t &other);

    void move_from(value_t &other);
    value_t(value_t &&other);
    value_t &operator=(value_t &&other);

    template <typename T>
    T &val() const
    {
        return *static_cast<T *>(const_cast<void *>(st()));
    }

    inline long long &integer() const { return val<long long>(); }
    inline double &floating() const { return val<double>(); }
    inline bool &boolean() const { return val<bool>(); }
    inline symbol_node &symbol() const { return val<symbol_node>(); }
    inline data_ptr &data() const { return val<data_ptr>(); }
};

}

#endif
