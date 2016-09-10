// Rubberband language
// Copyright (C) 2015--2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
    metainfo_data(
        const interface_collection<Interfaces...> &interfaces,
        const object &obj) :
        interfaces{interfaces},
        obj{obj}
    {}

    const interface_collection<Interfaces...> &interfaces;
    object obj;
};

template <typename... Interfaces>
object iface_collection_follows_interface(object *thisptr, object &msg)
{
    auto data = static_cast<metainfo_data<Interfaces...> *>(thisptr->__value.data());

    return boolean(data->interfaces.follows_interface(msg));
}

template <typename... Interfaces>
object iface_collection_responds_to(object *thisptr, object &msg)
{
    auto data = static_cast<metainfo_data<Interfaces...> *>(thisptr->__value.data());

    return boolean(data->interfaces.responds_to(&data->obj, msg));
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
            auto iface_name = interface.interface_name();
            if (!iface_name)
                return control::break_loop;

            follows = name == symbol(iface_name);

            if (follows)
                return control::break_loop;
        });

        return follows;
    }

    send_msg_function select_function(object *thisptr, object &msg) const
    {
        if (msg == symbol("<<"))
            return iface_collection_responds_to<Interfaces...>;

        if (msg == symbol("<<?"))
            return iface_collection_follows_interface<Interfaces...>;

        send_msg_function f;

        for_each(_interfaces, [&](const auto &interface)
        {
            f = interface.select_function(thisptr, msg);

            if (f)
                return control::break_loop;
        });

        return f;
    }

    object select_response(object *thisptr, object &msg) const
    {
        if (msg == symbol("<<") || msg == symbol("<<?"))
            return object::create_data_object(
                new metainfo_data<Interfaces...>{*this, *thisptr},
                select_function(thisptr, msg));

        object f;

        for_each(_interfaces, [&](const auto &interface)
        {
            f = interface.select_response(thisptr, msg);

            if (f != empty())
                return control::break_loop;
        });

        return f;
    }

    inline bool responds_to(object *thisptr, object &msg) const
    {
        if (msg == symbol("<<") || msg == symbol("<<?"))
            return select_function(thisptr, msg);

        bool responds;

        for_each(_interfaces, [&](const auto &interface)
        {
            responds = interface.responds_to(thisptr, msg);

            if (responds)
                return control::break_loop;
        });

        return responds;
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
    send_msg_function select_function(object *thisptr, object &msg) const;\
    bool responds_to(object *thisptr, object &msg) const;\
    object select_response(object *thisptr, object &msg) const;\
private:

namespace iface
{

class arith
{
    RBB_IFACE("[+]")

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
    RBB_IFACE("[=]")

public:
    comparable(send_msg_function eq_function, send_msg_function ne_function);

private:
    send_msg_function
        _eq_function,
        _ne_function;
};

class ordered
{
    RBB_IFACE("[<]")

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

// TODO come up with some way for an interface to require others

// name-only interface
class symbolic
{
    RBB_IFACE("[a]")
};

class numeric
{
    RBB_IFACE("[0]")

public:
    numeric(send_msg_function array_send_msg);

private:
    send_msg_function _array_send_msg;
};

class booleanoid
{
    RBB_IFACE("[?]")

public:
    booleanoid(
        send_msg_function and_send_msg,
        send_msg_function or_send_msg,
        send_msg_function get_context_send_msg,
        send_msg_function raise_send_msg);

private:
    send_msg_function
        _and_send_msg,
        _or_send_msg,
        _get_context_send_msg,
        _raise_send_msg;
};

class listable
{
    RBB_IFACE("[|]")

public:
    listable(send_msg_function concat_send_msg, send_msg_function slice_send_msg);

private:
    send_msg_function
        _concat_send_msg,
        _slice_send_msg;
};

class mapped
{
    RBB_IFACE("[:]")
    
public:
    mapped(
        send_msg_function merge_send_msg,
        send_msg_function del_send_msg);
    
private:
    send_msg_function
        _merge_send_msg,
        _del_send_msg;
};

class executable
{
    RBB_IFACE("[{}]")

public:
    executable(send_msg_function execute);

private:
    send_msg_function _execute;
};

}

}

#endif
