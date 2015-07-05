// Rubberband language
// Copyright (C) 2013--2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
#include <unordered_set>
#include <vector>

namespace rbb
{

class symbol_node;

class shared_data_t;

struct value_t
{
    enum type_t
    {
        no_data_t  = 0x10,
        empty_t    = 0x11,
        integer_t  = 0x12,
        floating_t = 0x13,
        boolean_t  = 0x14,
        symbol_t   = 0x15,
        data_t     = 0x20
    } type = value_t::empty_t;

    union
    {
        long long integer;
        double floating;
        bool boolean;
        symbol_node *symbol;
        std::shared_ptr<shared_data_t> *shared_data;
    };

    shared_data_t *data() const { return shared_data->get(); }
};

class object;
typedef object (*send_msg_function)(object *, const object &);

// Object
class object
{
public:
    object();
    object(const object &other);
    object(object &&other);
    object &operator=(const object &other);
    ~object();

    bool operator==(const object &other) const;
    inline bool operator!=(const object &other) const { return !(other == *this); }

    object operator<<(const object &msg); // send_msg

    std::string to_string(
        std::shared_ptr<std::unordered_set<const object *>> visited = nullptr) const;

    value_t __value;
    send_msg_function __send_msg;
};

object empty();

object number(double val);
double number_to_double(const object &num);

object symbol(const std::string &val);

object boolean(bool val);

// NEVER assign pointers directly to other objects

object array(const std::vector<object> &objects = std::vector<object> {});
inline object array(const std::initializer_list<object> &objects)
{
    return array(std::vector<object> {objects});
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
