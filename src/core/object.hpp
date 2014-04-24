// Rubberband language
// Copyright (C) 2013  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

namespace rbb
{

class symbol_node;

class shared_data_t;

struct value_t
{
    enum
    {
        no_data_t  = 0x10,
        empty_t    = 0x11,
        integer_t  = 0x12,
        floating_t = 0x13,
        boolean_t  = 0x14,
        symbol_t   = 0x15,
        data_t     = 0x20
    } type;

    union
    {
        long integer;
        double floating;
        bool boolean;
        symbol_node *symbol;
        shared_data_t *data;
    };
};

// Object
class object
{
public:
    object();
    object(const object &other);
    object &operator=(const object &other);
    ~object();

    bool operator==(const object &other) const;
    inline bool operator!=(const object &other) const { return !(other == *this); }

    object operator<<(const object &msg); // send_msg

    value_t __value;
    object (*__send_msg)(object *, const object &);

private:
    void destroy();
    void ref();
    int deref();
};

object empty();

object number(double val);
double number_to_double(const object &num);

object symbol(char *val);
object symbol(const char *val);

object boolean(bool val);

// NEVER assign pointers directly to other objects

object array(const object obj_array[], int size);

object table(const object symbol_array[], const object obj_array[], int size);

}

#endif
