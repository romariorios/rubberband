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
