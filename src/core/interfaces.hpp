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
#include "shared_data_t.hpp"
#include "symbol.hpp"
#include "utils/tuple_utils.hpp"

namespace rbb
{

template <typename... Interfaces>
class interface_collection;

template <typename... Interfaces>
class metainfo_data : public shared_data_t
{
public:
    metainfo_data(const interface_collection<Interfaces...> &interfaces) :
        interfaces{interfaces}
    {}

    const interface_collection<Interfaces...> &interfaces;
};

template <typename... Interfaces>
object iface_collection_follows_interface(object *thisptr, const object &msg)
{
    auto data = static_cast<metainfo_data<Interfaces...> *>(thisptr->__value.data());

    return boolean(data->interfaces.follows_interface(msg));
}

template <typename... Interfaces>
object iface_collection_responds_to(object *thisptr, const object &msg)
{
    auto data = static_cast<metainfo_data<Interfaces...> *>(thisptr->__value.data());

    return boolean(data->interfaces.responds_to(thisptr, msg));
}

template <typename... Interfaces>
class interface_collection
{
public:
    interface_collection(Interfaces... interfaces) :
        _interfaces{std::make_tuple(interfaces...)}
    {}

    bool follows_interface(const object &name) const
    {
        bool follows = false;

        for_each(_interfaces, [&](const auto &interface)
        {
            follows = name == symbol(interface.interface_name());

            if (follows)
                return control::break_loop;
        });

        return follows;
    }

    object select_response(object *thisptr, const object &msg) const
    {
        if (msg == symbol("<<"))
            return object::create_data_object(
                new metainfo_data<Interfaces...>{*this},
                iface_collection_responds_to<Interfaces...>);

        if (msg == symbol("<<?"))
            return object::create_data_object(
                new metainfo_data<Interfaces...>{*this},
                iface_collection_follows_interface<Interfaces...>);

        object f;

        for_each(_interfaces, [&](const auto &interface)
        {
            f = interface.select_response(thisptr, msg);

            if (f != empty())
                return control::break_loop;
        });

        return f;
    }

    inline bool responds_to(object *thisptr, const object &msg) const
    {
        return select_response(thisptr, msg) != empty();
    }

private:
    std::tuple<Interfaces...> _interfaces;
};

template <typename... Interfaces>
interface_collection<Interfaces...> mk_interface_collection(Interfaces... interfaces)
{
    return {interfaces...};
}

#define RBB_IFACE(__name) \
public:\
    inline const char *interface_name() const { return __name; }\
    object select_response(object *thisptr, const object &msg) const;\
private:

namespace iface
{

class arith
{
    RBB_IFACE("--+")

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

class comparable
{
    RBB_IFACE("--=")

public:
    comparable(send_msg_function eq_function, send_msg_function ne_function);

private:
    send_msg_function
        _eq_function,
        _ne_function;
};

class ordered
{
    RBB_IFACE("--<")

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
