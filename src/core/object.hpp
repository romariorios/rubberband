// Rubberband language
// Copyright (C) 2013--2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "symbol.hpp"
#include "shared_data_t.hpp"

namespace rbb
{

class shared_data_t;

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
    
    explicit value_t(type_t t) :
        type{t}
    {}

    value_t(long long val, __integer) :
        type{integer_t}
    {
        ::new (st()) long long{val};
    }

    value_t(double val, __floating) :
        type{floating_t}
    {
        ::new (st()) double{val};
    }

    value_t(bool val, __boolean) :
        type{boolean_t}
    {
        ::new (st()) bool{val};
    }

    explicit value_t(symbol_node sym) :
        type{symbol_t}
    {
        ::new (st()) symbol_node{sym};
    }

    explicit value_t(shared_data_t *data) :
        type{data_t}
    {
        ::new (st()) data_ptr{data};
    }

    void copy_from(const value_t &other)
    {
        type = other.type;
        if (type == data_t)
            ::new (st()) data_ptr{other.data()};
        else
            _s = other._s;
    }

    value_t(const value_t &other)
    {
        copy_from(other);
    }

    ~value_t()
    {
        if (type == data_t)
            data().~shared_ptr<shared_data_t>();
    }

    value_t &operator=(const value_t &other)
    {
        this->~value_t();
        copy_from(other);

        return *this;
    }

    void move_from(value_t &other)
    {
        type = other.type;
        other.type = no_data_t;
        _s = other._s;
    }

    value_t(value_t &&other)
    {
        move_from(other);
    }

    value_t &operator=(value_t &&other)
    {
        this->~value_t();
        move_from(other);

        return *this;
    }

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

class object;
typedef object (*send_msg_function)(object *, object &);

// Object
class object
{
public:
    static object create_data_object(
        shared_data_t *data,
        send_msg_function send_msg = nullptr);
    
    explicit object(value_t &&v, send_msg_function send_msg);
    object();

    bool operator==(const object &other) const;
    inline bool operator!=(const object &other) const { return !(other == *this); }

    object operator<<(object &&msg);
    object operator<<(object &msg); // send_msg
    
    // useful overloads
    object operator<<(double num);
    object operator<<(std::string &&sym);

    std::string to_string(
        std::shared_ptr<std::unordered_set<const object *>> = nullptr) const;

    value_t __value;
    send_msg_function __send_msg;
};

object empty();

object number(double val);
double number_to_double(const object &num);

object symbol(const std::string &val);

object boolean(bool val);

inline object to_object(double val) { return number(val); }
inline object to_object(std::string &&sym) { return symbol(sym); }
inline object to_object(const object &obj) { return obj; }
inline object to_object(object &&obj) { return obj; }

// NEVER assign pointers directly to other objects

object array(const std::vector<object> &objects = std::vector<object> {});
inline object array(const std::initializer_list<object> &objects)
{
    return array(std::vector<object> {objects});
}

template <int I = 0>
inline void fill_objvec(std::vector<object> &) {}

template <int I = 0, typename T, typename... OtherTypes>
void fill_objvec(std::vector<object> &vec, T &&val, OtherTypes &&... other_vals)
{
    vec[I] = to_object(std::forward<T>(val));
    fill_objvec<I + 1>(vec, std::forward<OtherTypes>(other_vals)...);
}

template <typename... Types>
object objarr(Types &&... vals)
{
    std::vector<object> vec;
    vec.resize(sizeof...(Types));
    fill_objvec(vec, std::forward<Types>(vals)...);
    
    return array(vec);
}

object table(
    const std::vector<object> &symbols = std::vector<object> {},
    const std::vector<object> &objects = std::vector<object> {});
inline object table(
    const std::initializer_list<object> &symbols,
    const std::initializer_list<object> &objects)
{
    return table(
        std::vector<object> {symbols},
        std::vector<object> {objects});
}

}

#endif
