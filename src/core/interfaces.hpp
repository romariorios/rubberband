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

#ifndef INTERFACES_HPP
#define INTERFACES_HPP

#include "object.hpp"
#include "symbol.hpp"

namespace rbb
{

class interface
{
public:
    virtual const char *interface_name() const = 0;

    // returns 0 if no appropriate function is found
    virtual send_msg_function select_function(const object &msg) const = 0;
};

template <int InterfacesCount>
class interface_collection
{
    using interfaces_t = std::array<std::unique_ptr<interface>, InterfacesCount>;

public:
    interface_collection(interfaces_t &&interfaces) :
        _interfaces(std::move(interfaces))
    {}

    bool follows_interface(const object &name) const
    {
        for (auto &&iface : _interfaces)
            if (name == symbol(iface->interface_name()))
                return true;

        return false;
    }

    send_msg_function select_function(const object &msg) const
    {
        for (auto &&iface : _interfaces) {
            auto f = iface->select_function(msg);

            if (f)
                return f;
        }

        return nullptr;
    }

    inline bool responds_to(const object &msg) const
    {
        return select_function(msg);
    }

private:
    interfaces_t _interfaces;
};

#define RBB_IFACE(__name) public interface \
{\
public:\
    inline const char *interface_name() const { return __name; }\
    send_msg_function select_function(const object &msg) const;\
private:

namespace iface
{

class arith : RBB_IFACE("--+")

public:
    arith(
        send_msg_function add_function,
        send_msg_function sub_function,
        send_msg_function mul_function,
        send_msg_function div_function);

private:
    send_msg_function
        _add_function,
        _sub_function,
        _mul_function,
        _div_function;
};

class comparable : RBB_IFACE("--=")

public:
    comparable(send_msg_function eq_function, send_msg_function ne_function);

private:
    send_msg_function
        _eq_function,
        _ne_function;
};

class ordered : RBB_IFACE("--<")

public:
    ordered(
        send_msg_function lt_function,
        send_msg_function gt_function,
        send_msg_function le_function,
        send_msg_function ge_function);

private:
    send_msg_function
        _lt_function,
        _gt_function,
        _le_function,
        _ge_function;
};

}

}

#endif
