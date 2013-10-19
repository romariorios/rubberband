// Rubberband language basic objects
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

struct value_t
{
    enum
    {
        empty_t        = 0x00,
        number_t       = 0x10,
        integer_t      = 0x11,
        floating_t     = 0x12,
        boolean_t      = 0x01,
        symbol_t       = 0x02,
        block_t        = 0x08,
        list_t         = 0x04,
        generic_obj_t  = 0x20,
        external_obj_t = 0x40
    } type;
    
    union
    {
        long integer;
        double floating;
        bool boolean;
        symbol_node *symbol;
        void *data;
    };
};

struct object_methods;

// Object
class object
{
public:
    object();
    object(const object &other);
    ~object();
    
    bool operator==(const object &other) const;
    inline bool operator!=(const object &other) const { return !(other == *this); }
    
    object send_msg(const object &msg);
    
    value_t __value;
    object_methods *__m;
    
private:
    void destroy();
    void ref();
    int deref();
};

object empty();

object number(double val);

object symbol(char *val);
object symbol(const char *val);

object boolean(bool val);

// NEVER assign pointers directly to other objects

object list(const object obj_array[], int size);

struct list_data
{
    const object *arr;
    int size;
    int refc;
};

// TODO block

object generic_object(const object symbol_array[], const object obj_array[], int size);

}

#endif
