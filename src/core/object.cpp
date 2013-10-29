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

#include "object.hpp"

#include "data_templates.hpp"
#include "symbol.hpp"

#include <cmath>

#define SEND_MSG(typename)\
static object typename##_send_msg(object *thisptr, const object &msg)

using namespace rbb;

class rbb::shared_data_t
{
public:
    shared_data_t() :
        refc(1)
    {}
    
    virtual ~shared_data_t()
    {}

    int refc;
};

// object: The base for everything
SEND_MSG(noop)
{
    return empty();
}

object::object() :
    __send_msg(noop_send_msg)
{}

object::object(const object &other) :
    __value(other.__value),
    __send_msg(other.__send_msg)
{
    ref();
}

static bool is_numeric(const value_t &val)
{
    return val.type == value_t::integer_t ||
           val.type == value_t::floating_t;
}

object::~object()
{
    if (deref() == 0)
        destroy();
}

bool object::operator==(const object& other) const
{
    if (other.__value.type != __value.type)
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
    return __send_msg(this, msg);
}

void object::destroy()
{
    if (__value.type == value_t::data_t)
        delete __value.data;
}

void object::ref()
{
    if (__value.type == value_t::data_t)
        ++__value.data->refc;
}

int object::deref()
{
    if (__value.type == value_t::data_t)
        return --__value.data->refc;
    
    return 1;
}

class object_data : public shared_data_t
{
public:
    object_data(const object &obj) :
        obj(obj)
    {}
    
    object obj;
};

// empty: Empty object
SEND_MSG(empty_cmp_eq) { return boolean(msg.__value.type == value_t::empty_t); }
SEND_MSG(empty_cmp_ne) { return boolean(msg.__value.type != value_t::empty_t); }

SEND_MSG(empty)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object cmp;
    cmp.__value.type = value_t::no_data_t;
    
    if (msg == symbol("=="))
        cmp.__send_msg = empty_cmp_eq_send_msg;
    else if (msg == symbol("!="))
        cmp.__send_msg = empty_cmp_ne_send_msg;
    else
        return empty();

    return cmp;
}

object rbb::empty()
{
    object emp;
    emp.__value.type = value_t::empty_t;
    emp.__send_msg = empty_send_msg;
    
    return emp;
}

// number: Numeric object
double rbb::number_to_double(const object &num)
{
    if (!is_numeric(num.__value))
        return NAN;
    
    return num.__value.type == value_t::floating_t?
        num.__value.floating : (double) num.__value.integer;
}

static object num_operation(const object &thisobj, const object &msg,
                            object (*int_operation)(long, long),
                            object (*float_operation)(double, double))
{
    object obj = static_cast<object_data *>(thisobj.__value.data)->obj;
    
    if (!is_numeric(obj.__value))
        return empty();

    if (obj.__value.type == value_t::integer_t &&
        msg.__value.type == value_t::integer_t
    ) {
        return int_operation(obj.__value.integer, msg.__value.integer);
    }

    double this_val = number_to_double(obj);
    double msg_val = number_to_double(msg);

    return float_operation(this_val, msg_val);
}

#define NUM_OPERATION(operation, symbol, ret_obj)\
object num_int_operation_##operation(long a, long b)\
{ return ret_obj(a symbol b); }\
object num_float_operation_##operation(double a, double b)\
{ return ret_obj(a symbol b); }\
\
SEND_MSG(num_op_##operation)\
{\
    return num_operation(*thisptr, msg,\
                         num_int_operation_##operation, num_float_operation_##operation);\
}

NUM_OPERATION(eq, ==, boolean)
NUM_OPERATION(ne, !=, boolean)
NUM_OPERATION(lt, <, boolean)
NUM_OPERATION(gt, >, boolean)
NUM_OPERATION(le, <=, boolean)
NUM_OPERATION(ge, >=, boolean)

NUM_OPERATION(add, +, number)
NUM_OPERATION(sub, -, number)
NUM_OPERATION(mul, *, number)
NUM_OPERATION(div, /, number)

SEND_MSG(number)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object comp;
    
    if (msg == symbol("=="))
        comp.__send_msg = num_op_eq_send_msg;
    else if (msg == symbol("!="))
        comp.__send_msg = num_op_ne_send_msg;
    else if (msg == symbol("<"))
        comp.__send_msg = num_op_lt_send_msg;
    else if (msg == symbol(">"))
        comp.__send_msg = num_op_gt_send_msg;
    else if (msg == symbol("<="))
        comp.__send_msg = num_op_le_send_msg;
    else if (msg == symbol(">="))
        comp.__send_msg = num_op_ge_send_msg;
    else if (msg == symbol("+"))
        comp.__send_msg = num_op_add_send_msg;
    else if (msg == symbol("-"))
        comp.__send_msg = num_op_sub_send_msg;
    else if (msg == symbol("*"))
        comp.__send_msg = num_op_mul_send_msg;
    else if (msg == symbol("/"))
        comp.__send_msg = num_op_div_send_msg;
    else
        return empty();
    
    comp.__value.type = value_t::data_t;
    comp.__value.data = new object_data(*thisptr);
    
    return comp;
}

object rbb::number(double val)
{
    object num;
    num.__send_msg = number_send_msg;
    
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
static object symbol_comp_send_msg(object *thisptr, const object &msg, bool eq)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    return static_cast<object_data *>(thisptr->__value.data)->obj == msg?
        boolean(eq) : boolean(!eq);
}

SEND_MSG(symbol_comp_eq) { return symbol_comp_send_msg(thisptr, msg, true); }

SEND_MSG(symbol_comp_ne) { return symbol_comp_send_msg(thisptr, msg, false); }

SEND_MSG(symbol)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object cmp_op;
    cmp_op.__value.data = new object_data(*thisptr);
    cmp_op.__value.type = value_t::data_t;
    
    if (msg.__value.symbol == symbol_node::retrieve("=="))
        cmp_op.__send_msg = symbol_comp_eq_send_msg;
    
    if (msg.__value.symbol == symbol_node::retrieve("!="))
        cmp_op.__send_msg = symbol_comp_ne_send_msg;
    
    return cmp_op;
}

object rbb::symbol(char* val)
{
    object symb;
    symb.__send_msg = symbol_send_msg;
    symb.__value.type = value_t::symbol_t;
    symb.__value.symbol = symbol_node::retrieve(val);
    
    return symb;
}

object rbb::symbol(const char* val)
{
    object symb;
    symb.__send_msg = symbol_send_msg;
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

SEND_MSG(boolean)
{
    if (msg.__value.type != value_t::symbol_t)
        return empty();
    
    object comp_block;
    comp_block.__send_msg = boolean_comp_send_msg;
    comp_block.__value.type = value_t::no_data_t;
    
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

object rbb::boolean(bool val)
{
    object b;
    b.__send_msg = boolean_send_msg;
    b.__value.type = value_t::boolean_t;
    b.__value.boolean = val;
    
    return b;
}

// Comparison for any other objects
static object data_comparison_send_msg(object *thisptr, const object &msg, bool eq)
{
    object_data *d = static_cast<object_data *>(thisptr->__value.data);
    
    if (!d)
        return empty();
    
    return rbb::boolean((d->obj == msg) == eq);
}

SEND_MSG(data_comparison_eq) { return data_comparison_send_msg(thisptr, msg, true); }

SEND_MSG(data_comparison_ne) { return data_comparison_send_msg(thisptr, msg, false); }

// list: Array of objects
class list_data : public shared_data_t
{
public:
    list_data(const int size) :
        size(size)
    {
        arr = new object[size];
    }
    
    ~list_data()
    {
        delete[] arr;
    }
    
    object *arr;
    int size;
};

SEND_MSG(list);

static object create_list_object(list_data *d)
{
    object l;
    l.__value.type = value_t::data_t;
    l.__value.data = d;
    l.__send_msg = list_send_msg;
    
    return l;
}

SEND_MSG(list_concatenation)
{
    if (msg.__value.type != value_t::data_t)
        return empty();
    
    object d_obj = static_cast<object_data *>(thisptr->__value.data)->obj;
    list_data *d = static_cast<list_data *>(d_obj.__value.data);
    list_data *msg_d = static_cast<list_data *>(msg.__value.data);
    
    if (!msg_d)
        return empty();
    
    list_data *new_d = new list_data(d->size + msg_d->size);
    
    for (int i = 0; i < d->size; ++i)
        new_d->arr[i] = d->arr[i];
    
    for (int i = d->size, j = 0; j < msg_d->size; ++i, ++j)
        new_d->arr[i] = msg_d->arr[j];
    
    return create_list_object(new_d);
}

static int get_index_from_obj(const object &obj);

SEND_MSG(list_slicing)
{
    if (msg.__value.type != value_t::data_t)
        return empty();
    
    object d_obj = static_cast<object_data *>(thisptr->__value.data)->obj;
    list_data *d = static_cast<list_data *>(d_obj.__value.data);
    list_data *msg_d = static_cast<list_data *>(msg.__value.data);
    
    if (!msg_d || msg_d->size < 2)
        return empty();
    
    int pos = get_index_from_obj(msg_d->arr[0]);
    int size = get_index_from_obj(msg_d->arr[1]);
    
    if (pos < 0 || size < 0)
        return empty();
    
    int this_len = d->size;
    
    if (pos + size > this_len)
        return empty();
    
    list_data *new_d = new list_data(size);
    
    for (int i = 0, j = pos; i < size; ++i, ++j)
        new_d->arr[i] = d->arr[j];
    
    return create_list_object(new_d);
}

static int get_index_from_obj(const object &obj)
{
    if (!is_numeric(obj.__value))
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
    list_data *d = static_cast<list_data *>(thisptr->__value.data);
    
    if (msg.__value.type == value_t::symbol_t) {
        if (msg == symbol("?|"))
            return rbb::number(d->size);
        
        object symb_ret;
        symb_ret.__value.type = value_t::data_t;
        symb_ret.__value.data = new object_data(*thisptr);
        
        if (msg == symbol("=="))
            symb_ret.__send_msg = data_comparison_eq_send_msg;
        if (msg == symbol("!="))
            symb_ret.__send_msg = data_comparison_ne_send_msg;
        if (msg == symbol("+"))
            symb_ret.__send_msg = list_concatenation_send_msg;
        if (msg == symbol("/"))
            symb_ret.__send_msg = list_slicing_send_msg;
        
        return symb_ret;
    } else if (msg.__value.type == value_t::data_t) {
        list_data *msg_d = static_cast<list_data *>(msg.__value.data);
        if (!msg_d || msg_d->size < 2)
            return empty();
        
        int msg_ind = get_index_from_obj(msg_d->arr[0]);
        
        if (!in_bounds(d, msg_ind))
            return empty();
        
        d->arr[msg_ind] = msg_d->arr[1];
        
        return empty();
    }
    
    int ind = get_index_from_obj(msg);
    if (ind < 0)
        return empty();
    
    if (!in_bounds(d, ind))
        return empty();
    
    return d->arr[ind];
}

object rbb::list(const object obj_array[], int size)
{
    list_data *d = new list_data(size);
    
    for (int i = 0; i < size; ++i)
        d->arr[i] = obj_array[i];
    
    return create_list_object(d);
}

// TODO block

// Generic object: Basically, a map from symbols to objects
struct symbol_object_pair
{
    symbol_object_pair(symbol_node *sym = 0, const object &obj = empty()) :
        sym(sym),
        obj(obj)
    {}
    
    symbol_node *sym;
    object obj;
};

class symbol_object_tree : public splay_tree<symbol_node *, symbol_object_pair>
{
public:
    symbol_object_tree() {}
    
protected:
    symbol_node *key_from_node(node *n) const { return n->item.sym; }
};

class generic_object_data : public shared_data_t
{
public:
    symbol_object_tree objtree;
};

SEND_MSG(generic_object_merge)
{
    if (msg.__value.type != value_t::data_t)
        return empty();
    
    if (!static_cast<generic_object_data *>(msg.__value.data))
        return empty();
    
    object gen_obj = rbb::generic_object(0, 0, 0);
    gen_obj.send_msg(static_cast<object_data *>(thisptr->__value.data)->obj);
    gen_obj.send_msg(msg);
    
    return gen_obj;
}

SEND_MSG(generic_object)
{
    if (msg.__value.type == value_t::symbol_t) {
        object answer;
        answer.__value.type = value_t::data_t;
        answer.__value.data = new object_data(*thisptr);
        
        if (msg == rbb::symbol("=="))
            answer.__send_msg = data_comparison_eq_send_msg;
        else if (msg == rbb::symbol("!="))
            answer.__send_msg = data_comparison_ne_send_msg;
        else if (msg == rbb::symbol("+"))
            answer.__send_msg = generic_object_merge_send_msg;
        else if (msg == rbb::symbol("?:")) {
            generic_object_data *d = static_cast<generic_object_data *>(thisptr->__value.data);
            
            linked_list<symbol_node *> *fields = d->objtree.keys();
            linked_list<symbol_node *> *old_fields = fields;
            int size = fields->count();
            object *l_el = new object[size];
            
            for (int i = 0; i < size; ++i) {
                object sym;
                sym.__value.type = value_t::symbol_t;
                sym.__value.symbol = fields->value;
                
                l_el[i] = sym;
                
                linked_list<symbol_node *> *old_fields = fields;
                fields = fields->next;
            }
            
            delete old_fields;
            
            return list(l_el, size);
        } else {
            generic_object_data *d = static_cast<generic_object_data *>(thisptr->__value.data);
            
            const symbol_object_pair &pair = d->objtree.at(msg.__value.symbol);
            return pair.obj;
        }
            
        return answer;
    } else if (msg.__value.type == value_t::data_t) {
        generic_object_data *d = static_cast<generic_object_data *>(thisptr->__value.data);
        generic_object_data *msg_d =
            static_cast<generic_object_data *>(msg.__value.data);
        if (!msg_d)
            return empty();
        
        linked_list<symbol_node *> *msg_fields = msg_d->objtree.keys();
        linked_list<symbol_node *> *msg_fields_head = msg_fields;
        
        for (; msg_fields; msg_fields = msg_fields->next) {
            d->objtree.insert(msg_fields->value, msg_d->objtree.at(msg_fields->value));
        }
        
        delete msg_fields_head;
    }
    
    return empty();
}

object rbb::generic_object(const object symbol_array[], const object obj_array[], int size)
{
    generic_object_data *d = new generic_object_data;
    
    for (int i = 0; i < size; ++i) {
        if (symbol_array[i].__value.type != value_t::symbol_t)
            continue;
        
        symbol_node *sym = symbol_array[i].__value.symbol;
        d->objtree.insert(sym, symbol_object_pair(sym, obj_array[i]));
    }
    
    object gen_obj;
    gen_obj.__value.type = value_t::data_t;
    gen_obj.__value.data = d;
    gen_obj.__send_msg = generic_object_send_msg;
    
    return gen_obj;
}
