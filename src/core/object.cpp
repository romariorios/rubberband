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

#include "object.hpp"

#include "symbol.hpp"

#include <cmath>

#define SEND_MSG(typename)\
static object typename##_send_msg(object *thisptr, const object &msg)

#define DESTROY(typename)\
static void typename##_destroy(object *thisptr)

#define REF(typename)\
static void typename##_ref(object *thisptr)

#define DEREF(typename)\
static int typename##_deref(object *thisptr)

#define OBJECT_METHODS(typename)\
static object_methods *typename##_object_methods_ptr = 0; \
static object_methods *typename##_object_methods() \
{ \
    if (!typename##_object_methods_ptr) {\
        typename##_object_methods_ptr = new object_methods;\
        typename##_object_methods_ptr->send_msg = typename##_send_msg;\
        typename##_object_methods_ptr->destroy = typename##_destroy;\
        typename##_object_methods_ptr->ref = typename##_ref;\
        typename##_object_methods_ptr->deref = typename##_deref;\
    }\
    return typename##_object_methods_ptr;\
}

#define OBJECT_METHODS_NO_DATA(typename)\
DESTROY(typename) {}\
REF(typename) {}\
DEREF(typename) { return 0; }\
OBJECT_METHODS(typename)

using namespace rbb;

struct rbb::object_methods
{
    object (*send_msg)(object *, const object &);
    void (*destroy)(object *);
    void (*ref)(object *);
    int (*deref)(object *);
};

// object: The base for everything
SEND_MSG(noop)
{
    return empty();
}
OBJECT_METHODS_NO_DATA(noop)

object::object() :
    __m(noop_object_methods())
{}

object::object(const object &other) :
    __value(other.__value),
    __m(other.__m)
{
    ref();
}

static bool is_atom(const value_t &val)
{
    return val.type & value_t::empty_t   ||
           val.type & value_t::number_t  ||
           val.type & value_t::boolean_t ||
           val.type & value_t::symbol_t  ;
}

object::~object()
{
    if (is_atom(__value) && deref() == 0)
        destroy();
}

bool object::operator==(const object& other) const
{
    if (!other.__value.type == __value.type)
        return false;
    
    switch (__value.type) {
    case value_t::empty_t:
        return true;
    case value_t::integer_t:
        return __value.integer == other.__value.integer;
    case value_t::floating_t:
        return __value.floating == other.__value.floating;
    case value_t::boolean_t:
        return __value.boolean == other.__value.boolean;
    case value_t::symbol_t:
        return __value.symbol == other.__value.symbol;
    default:
        return __value.data == other.__value.data;
    }
    
    return false;
}

object object::send_msg(const object &msg)
{
    return __m->send_msg(this, msg);
}

void object::destroy()
{
    __m->destroy(this);
}

void object::ref()
{
    __m->ref(this);
}

int object::deref()
{
    return __m->deref(this);
}

// empty: Empty object
SEND_MSG(empty_cmp_eq) { return boolean(msg.__value.type == value_t::empty_t); }
OBJECT_METHODS_NO_DATA(empty_cmp_eq)

SEND_MSG(empty_cmp_ne) { return boolean(msg.__value.type != value_t::empty_t); }
OBJECT_METHODS_NO_DATA(empty_cmp_ne)

SEND_MSG(empty)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object cmp;
    
    if (msg == symbol("=="))
        cmp.__m = empty_cmp_eq_object_methods();
    else if (msg == symbol("!="))
        cmp.__m = empty_cmp_ne_object_methods();
    else
        return empty();

    return cmp;
}
OBJECT_METHODS_NO_DATA(empty)

object rbb::empty()
{
    object emp;
    emp.__value.type = value_t::empty_t;
    emp.__m = empty_object_methods();
    
    return emp;
}

// number: Numeric object
double number_to_double(const object &num)
{
    return num.__value.type == value_t::floating_t?
        num.__value.floating : (double) num.__value.integer;
}

// TODO Join eq and ne (and gt and lt, etc.) in a single function or macro
SEND_MSG(number_comp_eq)
{
    if (!msg.__value.type & value_t::number_t)
        return empty();
    
    if (thisptr->__value.type == value_t::integer_t &&
        msg.__value.type == value_t::integer_t
    ) {
        return boolean(thisptr->__value.integer == msg.__value.integer);
    }
    
    double this_val = number_to_double(*thisptr);
    double msg_val = number_to_double(msg);
    
    return boolean(this_val == msg_val);
}
OBJECT_METHODS_NO_DATA(number_comp_eq)

SEND_MSG(number_comp_ne)
{
    if (!msg.__value.type & value_t::number_t)
        return empty();
    
    if (thisptr->__value.type == value_t::integer_t &&
        msg.__value.type == value_t::integer_t
    ) {
        return boolean(thisptr->__value.integer != msg.__value.integer);
    }
    
    double this_val = number_to_double(*thisptr);
    double msg_val = number_to_double(msg);
    
    return boolean(this_val != msg_val);
}
OBJECT_METHODS_NO_DATA(number_comp_ne)

SEND_MSG(number)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object comp;
    
    if (msg == symbol("=="))
        comp.__m = number_comp_eq_object_methods();
    else if (msg == symbol("!="))
        comp.__m = number_comp_ne_object_methods();
    else
        return empty();
    
    comp.__value.type = thisptr->__value.type;
    switch (thisptr->__value.type) {
    case value_t::integer_t:
        comp.__value.integer = thisptr->__value.integer;
        break;
    case value_t::floating_t:
        comp.__value.floating = thisptr->__value.floating;
        break;
    default:
        return empty();
    }
    
    return comp;
}
OBJECT_METHODS_NO_DATA(number)

object rbb::number(double val)
{
    object num;
    num.__m = number_object_methods();
    
    if (val - trunc(val) != 0) {
        num.__value.type = value_t::floating_t;
        num.__value.floating = val;
    } else {
        num.__value.type = value_t::integer_t;
        num.__value.integer = val;
    }
    
    return num;
}

// symbol: Symbol object
SEND_MSG(symbol_comp_eq)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    return thisptr->__value.symbol == msg.__value.symbol? boolean(true) : boolean(false);
}
OBJECT_METHODS_NO_DATA(symbol_comp_eq)

SEND_MSG(symbol_comp_ne)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    return thisptr->__value.symbol != msg.__value.symbol? boolean(true) : boolean(false);
}
OBJECT_METHODS_NO_DATA(symbol_comp_ne)

SEND_MSG(symbol)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object cmp_op;
    cmp_op.__value.symbol = thisptr->__value.symbol;
    
    if (msg.__value.symbol == symbol_node::retrieve("=="))
        cmp_op.__m = symbol_comp_eq_object_methods();
    
    if (msg.__value.symbol == symbol_node::retrieve("!="))
        cmp_op.__m = symbol_comp_ne_object_methods();
    
    return cmp_op;
}
OBJECT_METHODS_NO_DATA(symbol)

object rbb::symbol(char* val)
{
    object symb;
    symb.__m = symbol_object_methods();
    symb.__value.type = value_t::symbol_t;
    symb.__value.symbol = symbol_node::retrieve(val);
    
    return symb;
}

object rbb::symbol(const char* val)
{
    object symb;
    symb.__m = symbol_object_methods();
    symb.__value.type = value_t::symbol_t;
    symb.__value.symbol = symbol_node::retrieve(val);
    
    return symb;
}

// boolean: Boolean object
SEND_MSG(boolean_comp)
{
    if (msg.__value.type != value_t::boolean_t)
        return boolean(false);
    
    if (msg.__value.boolean != thisptr->__value.boolean)
        return boolean(false);
    
    return boolean(true);
}
OBJECT_METHODS_NO_DATA(boolean_comp)

SEND_MSG(boolean)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object comp_block;
    comp_block.__m = boolean_comp_object_methods();
    comp_block.__value.type = value_t::external_obj_t;
    
    if (msg.__value.symbol == symbol("==").__value.symbol) {
        comp_block.__value.boolean = thisptr->__value.boolean;
        return comp_block;
    }
    
    if (msg.__value.symbol == symbol("!=").__value.symbol) {
        comp_block.__value.boolean = !thisptr->__value.boolean;
        return comp_block;
    }
    
    return empty();
}
OBJECT_METHODS_NO_DATA(boolean)

object rbb::boolean(bool val)
{
    object b;
    b.__m = boolean_object_methods();
    b.__value.type = value_t::boolean_t;
    b.__value.boolean = val;
    
    return b;
}

// list: Array of objects
static int get_index_from_obj(const object &obj)
{
    if (obj.__value.type != value_t::number_t)
        return -1;
    
    int ind;
    if (obj.__value.type == value_t::floating_t)
        return obj.__value.floating;
    
    return obj.__value.integer;
}

static bool in_bounds(list_data *d, int i)
{
    return i >= 0 && i < d->size;
}

SEND_MSG(list)
{
    int ind = get_index_from_obj(msg);
    if (ind < 0)
        return empty();
    
    list_data *d = reinterpret_cast<list_data *>(thisptr->__value.data);
    
    if (!in_bounds(d, ind))
        return empty();
    
    return d->arr[ind];
}

DESTROY(list) {}
REF(list) {}

DEREF(list)
{
    return 0;
}

OBJECT_METHODS(list)

object rbb::list(const object obj_array[], int size)
{
    list_data *d = new list_data;
    
    d->arr = obj_array;
    d->size = size;
    d->refc = 1;
    
    object arr;
    arr.__value.type = value_t::list_t;
    arr.__value.data = (void *) d;
    arr.__m = list_object_methods();
    
    return arr;
}

// void list::destroy()
// {
//     for (int i = 0; i < d->size; ++i) {
//         delete d->arr[i];
//     }
//     
//     delete d;
// }
// 
// int list::deref()
// {
//     return --d->refc;
// }
// 
// const value_t list::value() const
// {
//     value_t v;
//     v.type = value_t::variable_t;
//     v.variable = d;
// }

// TODO block


