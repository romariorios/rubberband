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
#include <string>
#include <vector>

#include "value.hpp"

namespace rbb
{

class shared_data_t;

class object;
typedef object (*send_msg_function)(object *, object &);

// Object
class object
{
public:
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
