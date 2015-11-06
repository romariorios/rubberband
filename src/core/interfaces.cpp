// Rubberband language
// Copyright (C) 2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "block.hpp"
#include "interfaces.hpp"
#include "object_private.hpp"

using namespace rbb;
using namespace std;

class object_data : public shared_data_t
{
public:
    object_data(const object &obj) :
        obj{obj}
    {}

    object obj;
};

inline object create_response(object *thisptr, send_msg_function f)
{
    return object::create_data_object(new object_data{*thisptr}, f);
}

#define DEFAULT_SELECT_FUNCTION(__class) \
send_msg_function __class::select_function(object *thisptr, const object &msg) const\
{\
    return nullptr;\
}

#define DEFAULT_SELECT_RESPONSE(__class) \
object __class::select_response(object *thisptr, const object &msg) const\
{\
    auto f = select_function(thisptr, msg);\
    return f? create_response(thisptr, f) : object{};\
}

#define DEFAULT_RESPONDS_TO(__class) \
bool __class::responds_to(object *thisptr, const object &msg) const\
{\
    return select_function(thisptr, msg);\
}

iface::arith::arith(
    send_msg_function add_function,
    send_msg_function sub_function,
    send_msg_function mul_function,
    send_msg_function div_function) :
    _add_function{add_function},
    _sub_function{sub_function},
    _mul_function{mul_function},
    _div_function{div_function}
{}

send_msg_function iface::arith::select_function(object *, const object &msg) const
{
    if (msg == symbol("+"))
        return _add_function;
    if (msg == symbol("-"))
        return _sub_function;
    if (msg == symbol("*"))
        return _mul_function;
    if (msg == symbol("/"))
        return _div_function;

    return nullptr;
}

DEFAULT_SELECT_RESPONSE(iface::arith)
DEFAULT_RESPONDS_TO(iface::arith)

iface::comparable::comparable(send_msg_function eq_function, send_msg_function ne_function) :
    _eq_function{eq_function},
    _ne_function{ne_function}
{}

send_msg_function iface::comparable::select_function(object *, const object& msg) const
{
    if (msg == symbol("=="))
        return _eq_function;
    if (msg == symbol("/="))
        return _ne_function;

    return nullptr;
}

DEFAULT_SELECT_RESPONSE(iface::comparable)
DEFAULT_RESPONDS_TO(iface::comparable)

iface::ordered::ordered(
    send_msg_function lt_function,
    send_msg_function gt_function,
    send_msg_function le_function,
    send_msg_function ge_function) :
    _lt_function{lt_function},
    _gt_function{gt_function},
    _le_function{le_function},
    _ge_function{ge_function}
{}

send_msg_function iface::ordered::select_function(object *, const object& msg) const
{
    if (msg == symbol("<"))
        return _lt_function;
    if (msg == symbol(">"))
        return _gt_function;
    if (msg == symbol("<="))
        return _le_function;
    if (msg == symbol(">="))
        return _ge_function;

    return nullptr;
}

DEFAULT_SELECT_RESPONSE(iface::ordered)
DEFAULT_RESPONDS_TO(iface::ordered)

DEFAULT_SELECT_FUNCTION(iface::symbolic)
DEFAULT_SELECT_RESPONSE(iface::symbolic)
DEFAULT_RESPONDS_TO(iface::symbolic)

DEFAULT_SELECT_FUNCTION(iface::numeric)

iface::numeric::numeric(send_msg_function array_send_msg) :
    _array_send_msg{array_send_msg}
{}

object iface::numeric::select_response(object *thisptr, const object &msg) const
{
    if (!responds_to(thisptr, msg))
        return {};

    auto msg_copy = msg;
    int msg_size = number_to_double(msg_copy << symbol("*"));
    auto new_d = new array_data{get_index_from_obj(*thisptr)};

    for (auto i = 0; i < min(msg_size, new_d->size); ++i)
        new_d->arr[i] = msg_copy << number(i);

    return object::create_data_object(new_d, _array_send_msg);
}

bool iface::numeric::responds_to(object *, const object &msg) const
{
    return const_cast<object &>(msg) << symbol("<<?") << symbol("--|") == boolean(true);
}

iface::booleanoid::booleanoid(
    send_msg_function and_send_msg,
    send_msg_function or_send_msg,
    send_msg_function get_context_send_msg,
    send_msg_function raise_send_msg) :
    _and_send_msg{and_send_msg},
    _or_send_msg{or_send_msg},
    _get_context_send_msg{get_context_send_msg},
    _raise_send_msg{raise_send_msg}
{}

send_msg_function iface::booleanoid::select_function(object *, const object &msg) const
{
    if (msg == symbol("/\\"))
        return _and_send_msg;
    if (msg == symbol("\\/"))
        return _or_send_msg;
    if (msg == symbol("?"))
        return _get_context_send_msg;
    if (msg == symbol("^"))
        return _raise_send_msg;

    return nullptr;
}

bool iface::booleanoid::responds_to(object *thisptr, const object &msg) const
{
    return select_function(thisptr, msg) || msg == symbol("><");
}

object iface::booleanoid::select_response(object *thisptr, const object &msg) const
{
    if (!responds_to(thisptr, msg))
        return {};

    auto f = select_function(thisptr, msg);
    return f? create_response(thisptr, f) : boolean(!thisptr->__value.boolean);
}

iface::listable::listable(
    send_msg_function concat_send_msg,
    send_msg_function slice_send_msg) :
    _concat_send_msg{concat_send_msg},
    _slice_send_msg{slice_send_msg}
{}

send_msg_function iface::listable::select_function(object *, const object &msg) const
{
    if (msg == symbol("+"))
        return _concat_send_msg;
    if (msg == symbol("/"))
        return _slice_send_msg;

    return nullptr;
}

bool iface::listable::responds_to(object *thisptr, const object &msg) const
{
    if (select_function(thisptr, msg) ||
        msg == symbol("*"))
        return true;

    auto d = static_cast<array_data *>(thisptr->__value.data());

    auto msg_copy = msg;
    if (msg_copy << symbol("<<?") << symbol("--|") == boolean(true))
        return
            msg_copy << symbol("*") == number(2) &&
            in_bounds(d, get_index_from_obj(msg_copy << number(0)));

    return in_bounds(d, get_index_from_obj(msg));
}

object iface::listable::select_response(object *thisptr, const object &msg) const
{
    if (!responds_to(thisptr, msg))
        return {};

    auto d = static_cast<array_data *>(thisptr->__value.data());

    if (msg == symbol("*"))
        return number(d->size);

    if (is_numeric(msg))
        return d->arr[get_index_from_obj(msg)];

    auto msg_copy = msg;
    if (msg_copy << symbol("<<?") << symbol("--|") == boolean(true)) {
        auto index = get_index_from_obj(msg_copy << number(0));
        d->arr[index] = msg_copy << number(1);

        return {};
    }

    return create_response(thisptr, select_function(thisptr, msg));
}

iface::mapped::mapped(
    send_msg_function merge_send_msg,
    send_msg_function del_send_msg) :
    _merge_send_msg{merge_send_msg},
    _del_send_msg{del_send_msg}
{}

send_msg_function iface::mapped::select_function(object *, const object &msg) const
{
    if (msg == symbol("+"))
        return _merge_send_msg;
    if (msg == symbol("-"))
        return _del_send_msg;

    return nullptr;
}

bool iface::mapped::responds_to(object *thisptr, const object &msg) const
{
    if (select_function(thisptr, msg) ||
        msg == symbol("*"))
        return true;

    auto d = static_cast<table_data *>(thisptr->__value.data());

    auto msg_copy = msg;
    if (msg_copy << symbol("<<?") << symbol("--:") == boolean(true))
        return true;

    if (msg_copy << symbol("<<?") << symbol("--|") == boolean(true)) {
        int size = number_to_double(msg_copy << symbol("*"));
        for (int i = 0; i < size; ++i)
            if (!((msg_copy << number(i)).__value.type & value_t::symbol_t))
                return false;

        return true;
    }

    return
        msg_copy << symbol("<<?") << symbol("--:") == boolean(true) ||
        table_contains_symbol(d, msg);
}

object iface::mapped::select_response(object *thisptr, const object &msg) const
{
    if (!responds_to(thisptr, msg))
        return {};

    auto f = select_function(thisptr, msg);
    if (f)
        return create_response(thisptr, f);

    auto d = static_cast<table_data *>(thisptr->__value.data());

    if (msg == symbol("*")) {
        vector<object> l_el;

        for (auto pair : d->objtree) {
            object sym;
            sym.__value.type = value_t::symbol_t;
            sym.__value.symbol = pair.first;

            l_el.push_back(sym);
        }

        return rbb::array(l_el);
    }

    auto msg_copy = msg;
    if (msg_copy << symbol("<<?") << symbol("--:") == boolean(true)) {
        auto sym_array = msg_copy << symbol("*");
        int sym_array_len = number_to_double(sym_array << symbol("*"));

        for (int i = 0; i < sym_array_len; ++i) {
            auto cur_sym = sym_array << number(i);
            auto cur_sym_ptr = cur_sym.__value.symbol;
            d->objtree[cur_sym_ptr] = msg_copy << cur_sym;
        }

        return *thisptr;
    }

    if (msg_copy << symbol("<<?") << symbol("--|") == boolean(true)) {
        int size = number_to_double(msg_copy << symbol("*"));
        auto new_table = table();
        auto new_table_d = static_cast<table_data *>(new_table.__value.data());

        for (int i = 0; i < size; ++i) {
            auto obj = msg_copy << number(i);
            auto sym = obj.__value.symbol;

            if (d->objtree.find(sym) == d->objtree.end())
                continue;

            new_table_d->objtree[sym] = d->objtree.at(sym);
        }

        return new_table;
    }

    return d->objtree.at(msg.__value.symbol);
}

iface::executable::executable(send_msg_function execute) :
    _execute{execute}
{}

DEFAULT_SELECT_FUNCTION(iface::executable)

bool iface::executable::responds_to(object *, const object &) const
{
    return true;
}

object iface::executable::select_response(object *thisptr, const object &msg) const
{
    block_data *d = static_cast<block_data *>(thisptr->__value.data());
    auto block_l = new literal::block(*d->block_l);
    block_l->set_context(msg);

    return object::create_data_object(new block_data(block_l), _execute);
}
