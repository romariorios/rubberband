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

#include "interfaces.hpp"

using namespace rbb;
using namespace std;

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

send_msg_function iface::arith::select_function(const object &msg) const
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

iface::comparable::comparable(send_msg_function eq_function, send_msg_function ne_function) :
    _eq_function{eq_function},
    _ne_function{ne_function}
{}

send_msg_function iface::comparable::select_function(const object& msg) const
{
    // TODO << and <<? are part of this interface
    if (msg == symbol("=="))
        return _eq_function;
    if (msg == symbol("/="))
        return _ne_function;

    return nullptr;
}

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

send_msg_function iface::ordered::select_function(const object& msg) const
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
