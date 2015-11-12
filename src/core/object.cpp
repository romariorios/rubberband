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

#include "object.hpp"

#include "block.hpp"
#include "error.hpp"
#include "interfaces.hpp"
#include "object_private.hpp"
#include "symbol.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#define SEND_MSG(typename)\
static object typename##_send_msg(object *thisptr, const object &msg)

#define IFACES(typename) \
auto typename##_iface_collection =\
    mk_interface_collection

#define SELECT_RESPONSE_FOR(typename)\
SEND_MSG(typename)\
{\
    if (msg != symbol("<<") && *thisptr << symbol("<<") << msg != boolean(true))\
        throw message_not_recognized_error{*thisptr, msg};\
\
    return typename##_iface_collection.select_response(thisptr, msg);\
}

using namespace rbb;

// Helper create_object functions
static object create_object(value_t::type_t type, send_msg_function send_msg = 0)
{
    object ret;
    ret.__value.type = type;
    ret.__send_msg = send_msg;

    return ret;
}

object object::create_data_object(
    shared_data_t *data,
    send_msg_function send_msg,
    value_t::type_t extra_type)
{
    object ret = create_object(static_cast<value_t::type_t>(value_t::data_t | extra_type), send_msg);
    ret.__value.shared_data = new shared_ptr<shared_data_t>(data);

    return ret;
}

class object_data : public shared_data_t
{
public:
    object_data(const object &obj) :
        obj(obj)
    {}

    object obj;
};

// object: The base for everything
SEND_MSG(empty);

object::object() :
    __send_msg(empty_send_msg)
{}

object::object(const object &other)
{
    *this = other;
}

object::object(object&& other) :
    __value{other.__value},
    __send_msg{other.__send_msg}
{
    other.__value = value_t{};
}

object& object::operator=(const object& other)
{
    this->~object();

    if (other.__value.type & value_t::data_t) {
        __value.shared_data = new shared_ptr<shared_data_t>(*other.__value.shared_data);
        __value.type = other.__value.type;
    } else
        __value = other.__value;

    __send_msg = other.__send_msg;

    return *this;
}

object::~object()
{
    if (__value.type & value_t::data_t)
        delete __value.shared_data;
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
        return __value.data() == other.__value.data();
    }

    return false;
}

object object::operator<<(const object &msg)
{
    return __send_msg(this, msg);
}

// empty: Empty object
SEND_MSG(empty_cmp_eq) { return boolean(msg == empty()); }
SEND_MSG(empty_cmp_ne) { return boolean(msg != empty()); }

IFACES(empty)
(
    iface::comparable{
        empty_cmp_eq_send_msg,
        empty_cmp_ne_send_msg
    }
);

SELECT_RESPONSE_FOR(empty)

object rbb::empty()
{
    return object{};
}

// number: Numeric object
double rbb::number_to_double(const object &num)
{
    auto num_copy = num;
    if (!is_numeric(num_copy))
        return NAN;

    return num.__value.type & value_t::floating_t?
        num.__value.floating : (double) num.__value.integer;
}

static object num_operation(const object &thisobj, const object &msg,
                            object (*int_operation)(long long, long long),
                            object (*float_operation)(double, double))
{
    object obj = static_cast<object_data *>(thisobj.__value.data())->obj;

    if (!is_numeric(obj))
        return empty();

    if (obj.__value.type & value_t::integer_t &&
        msg.__value.type & value_t::integer_t
    ) {
        return int_operation(obj.__value.integer, msg.__value.integer);
    }

    double this_val = number_to_double(obj);
    double msg_val = number_to_double(msg);

    return float_operation(this_val, msg_val);
}

#define NUM_OPERATION(operation, symbol, ret_obj)\
object num_int_operation_##operation(long long a, long long b)\
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

SEND_MSG(array);

static object create_array_object(array_data *d)
{
    return object::create_data_object(d, array_send_msg);
}

IFACES(number)
(
    iface::arith{
        num_op_add_send_msg,
        num_op_sub_send_msg,
        num_op_mul_send_msg,
        num_op_div_send_msg
    },
    iface::comparable{
        num_op_eq_send_msg,
        num_op_ne_send_msg
    },
    iface::ordered{
        num_op_lt_send_msg,
        num_op_gt_send_msg,
        num_op_le_send_msg,
        num_op_ge_send_msg
    },
    iface::numeric{
        array_send_msg
    }
);

SELECT_RESPONSE_FOR(number)

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
    return static_cast<object_data *>(thisptr->__value.data())->obj == msg?
        boolean(eq) : boolean(!eq);
}

SEND_MSG(symbol_comp_eq) { return symbol_comp_send_msg(thisptr, msg, true); }

SEND_MSG(symbol_comp_ne) { return symbol_comp_send_msg(thisptr, msg, false); }

IFACES(symbol)
(
    iface::symbolic{},
    iface::comparable{
        symbol_comp_eq_send_msg,
        symbol_comp_ne_send_msg
    }
);

SELECT_RESPONSE_FOR(symbol)

object rbb::symbol(const std::string &val)
{
    object symb;
    symb.__send_msg = symbol_send_msg;
    symb.__value.type = value_t::symbol_t;
    symb.__value.symbol = symbol_node::retrieve(val);

    return symb;
}

bool has_iface(const object &obj, const char *iface)
{
    return const_cast<object &>(obj) << symbol("<<?") << symbol(iface) == boolean(true);
}

// boolean: Boolean object
SEND_MSG(boolean_comp)
{
    if (!has_iface(msg, "[?]"))
        return boolean(false);

    auto thisval =
        static_cast<object_data *>(thisptr->__value.data())->obj.__value.boolean;

    if (msg.__value.boolean != thisval)
        return boolean(false);

    return boolean(true);
}

SEND_MSG(boolean_comp_ne)
{
    return boolean(!boolean_comp_send_msg(thisptr, msg).__value.boolean);
}

class boolean_decision_data : public shared_data_t
{
public:
    boolean_decision_data(const object &symt, const object &bool_obj) :
        context(symt),
        boolean_obj(bool_obj)
    {}

    object context;
    object boolean_obj;
    object true_result;
};

static constexpr const char block_name[] = "Block";

SEND_MSG(boolean_get_iffalse_block)
{
    boolean_decision_data *d = static_cast<boolean_decision_data *>(thisptr->__value.data());
    if (!d)
        return empty();

    object block = msg;
    if (!has_iface(msg, "[{}]"))
        throw wrong_type_error<block_name>{*thisptr, msg};

    return d->boolean_obj.__value.boolean?
        d->true_result :
        block << d->context << empty();
}

SEND_MSG(boolean_get_iftrue_block)
{
    boolean_decision_data *d = static_cast<boolean_decision_data *>(thisptr->__value.data());
    if (!d)
        return empty();

    boolean_decision_data *d_ret = new boolean_decision_data(d->context, d->boolean_obj);

    object block = msg;
    if (!has_iface(msg, "[{}]"))
        throw wrong_type_error<block_name>{*thisptr, msg};

    if (d->boolean_obj.__value.boolean)
        d_ret->true_result = block << d->context << empty();

    return object::create_data_object(d_ret, boolean_get_iffalse_block_send_msg);
}

SEND_MSG(boolean_get_context)
{
    object_data *d = static_cast<object_data *>(thisptr->__value.data());
    if (!d)
        return empty();

    return object::create_data_object(
        new boolean_decision_data(msg, d->obj),
        boolean_get_iftrue_block_send_msg);
}

#define BOOLEAN_DO(op, sym)\
SEND_MSG(boolean_do_##op)\
{\
    if (!has_iface(msg, "[?]"))\
        return empty();\
\
    object_data *d = static_cast<object_data *>(thisptr->__value.data());\
    if (!d)\
        return empty();\
\
    return rbb::boolean(d->obj.__value.boolean sym msg.__value.boolean);\
}

BOOLEAN_DO(AND, &&)
BOOLEAN_DO(OR, ||)

SEND_MSG(boolean_raise)
{
    auto boolean_obj = static_cast<object_data *>(thisptr->__value.data())->obj;

    if (boolean_obj == boolean(true))
        throw rbb::runtime_error{msg};

    return {};
}

IFACES(boolean)
(
    iface::comparable{
        boolean_comp_send_msg,
        boolean_comp_ne_send_msg
    },
    iface::booleanoid{
        boolean_do_AND_send_msg,
        boolean_do_OR_send_msg,
        boolean_get_context_send_msg,
        boolean_raise_send_msg
    }
);

SELECT_RESPONSE_FOR(boolean)

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
    object_data *d = static_cast<object_data *>(thisptr->__value.data());

    if (!d)
        return empty();

    return rbb::boolean((d->obj == msg) == eq);
}

SEND_MSG(data_comparison_eq) { return data_comparison_send_msg(thisptr, msg, true); }

SEND_MSG(data_comparison_ne) { return data_comparison_send_msg(thisptr, msg, false); }

// array: Array of objects
SEND_MSG(array);

static constexpr const char array_name[] = "Array";

SEND_MSG(array_concatenation)
{
    object d_obj = static_cast<object_data *>(thisptr->__value.data())->obj;
    array_data *d = static_cast<array_data *>(d_obj.__value.data());

    if (has_iface(msg, "[|]"))
        throw wrong_type_error<array_name>{*thisptr, msg};

    array_data *msg_d = static_cast<array_data *>(msg.__value.data());
    array_data *new_d = new array_data(d->size + msg_d->size);

    for (int i = 0; i < d->size; ++i)
        new_d->arr[i] = d->arr[i];

    for (int i = d->size, j = 0; j < msg_d->size; ++i, ++j)
        new_d->arr[i] = msg_d->arr[j];

    return create_array_object(new_d);
}

static constexpr const char number_name[] = "Number";

SEND_MSG(array_slicing)
{
    object d_obj = static_cast<object_data *>(thisptr->__value.data())->obj;
    array_data *d = static_cast<array_data *>(d_obj.__value.data());

    if (!has_iface(msg, "[|]"))
        throw wrong_type_error<array_name>{*thisptr, msg};

    array_data *msg_d = static_cast<array_data *>(msg.__value.data());

    if (msg_d->size < 2)
        throw semantic_error{"Wrong number of arguments for array slicing", *thisptr, msg};

    if (
        !is_numeric(msg_d->arr[0]) ||
        !is_numeric(msg_d->arr[1])
    )
        throw wrong_type_error<number_name>{*thisptr, msg};

    auto pos = get_index_from_obj(msg_d->arr[0]);
    auto size = get_index_from_obj(msg_d->arr[1]);
    auto this_len = d->size;

    if (pos + size > this_len)
        return empty();

    array_data *new_d = new array_data(size);

    for (int i = 0, j = pos; i < size; ++i, ++j)
        new_d->arr[i] = d->arr[j];

    return create_array_object(new_d);
}

IFACES(array)
(
    iface::comparable{
        data_comparison_eq_send_msg,
        data_comparison_ne_send_msg
    },
    iface::listable{
        array_concatenation_send_msg,
        array_slicing_send_msg
    }
);

SELECT_RESPONSE_FOR(array)

object rbb::array(const std::vector<object> &objects)
{
    array_data *d = new array_data(objects.size());
    int i = 0;

    for (auto obj : objects)
        d->arr[i++] = obj;

    return create_array_object(d);
}

// Table: Basically, a map from symbols to objects
SEND_MSG(table_merge)
{
    if (!has_iface(msg, "[:]"))
        throw wrong_type_error<block_name>{*thisptr, msg};

    object table = rbb::table();
    table << static_cast<object_data *>(thisptr->__value.data())->obj;
    table << msg;

    return table;
}

static constexpr const char symbol_name[] = "Symbol";

SEND_MSG(del_element)
{
    if (!has_iface(msg, "[a]"))
        throw wrong_type_error<symbol_name>{*thisptr, msg};

    auto &objtree =
        static_cast<table_data *>(
            static_cast<object_data *>(
                thisptr->__value.data())
            ->obj.__value.data())
        ->objtree;

    objtree.erase(msg.__value.symbol);

    return {};
}

IFACES(table)
(
    iface::comparable{
        data_comparison_eq_send_msg,
        data_comparison_ne_send_msg
    },
    iface::mapped{
        table_merge_send_msg,
        del_element_send_msg
    }
);

SELECT_RESPONSE_FOR(table)

object rbb::table(
    const std::vector<object> &symbols,
    const std::vector<object> &objects)
{
    auto d = new table_data;
    const auto size = std::min(symbols.size(), objects.size());

    for (int i = 0; i < size; ++i) {
        if (!has_iface(symbols[i], "[a]"))
            continue;

        auto sym = symbols[i].__value.symbol;
        d->objtree[sym] = objects[i];
    }

    return object::create_data_object(d, table_send_msg);
}

// Block: A sequence of instructions ready to be executed
SEND_MSG(block_instance_get_metainfo)
{
    auto d = static_cast<object_data *>(thisptr->__value.data());

    object ans;
    try {
        ans = d->obj << msg;
    } catch (message_not_recognized_error) {
        return boolean(false);
    }

    return boolean(ans == boolean(true));
}

SEND_MSG(block_instance)
{
    block_data *d = static_cast<block_data *>(thisptr->__value.data());
    d->block_l->set_message(msg);

    auto &&ans = d->block_l->run();
    if (msg == symbol("<<") || msg == symbol("<<?"))
        return object::create_data_object(
            new object_data{ans},
            block_instance_get_metainfo_send_msg);

    return ans;
}

IFACES(block)
(
    iface::comparable{
        data_comparison_eq_send_msg,
        data_comparison_ne_send_msg
    },
    iface::executable{
        block_instance_send_msg
    }
);

SELECT_RESPONSE_FOR(block)

object rbb::literal::block::eval(literal::block *parent)
{
    auto block_l = new block{*this};
    if (parent)
        block_l->set_master(parent->_master);

    return object::create_data_object(new block_data(block_l), block_send_msg);
}
