// Rubberband language
// Copyright (C) 2015--2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "common_syms.hpp"
#include "error.hpp"
#include "object.hpp"
#include "shared_data_t.hpp"
#include "symbol.hpp"
#include "utils/tuple_utils.hpp"

#define IFACES(typename) \
auto typename##_iface_collection =\
    rbb::mk_interface_collection

#define SELECT_RESPONSE_FOR(typename)\
rbb::object typename##_send_msg(rbb::object *thisptr, rbb::object &msg)\
{\
    if (\
        msg != SY_RESP_TO &&\
        *thisptr << SY_RESP_TO << rbb::array({msg}) != rbb::boolean(true))\
\
        throw rbb::message_not_recognized_error{*thisptr, msg};\
\
    return typename##_iface_collection.select_response(thisptr, msg);\
}

namespace rbb
{

class object_data : public shared_data_t
{
public:
    object_data(const object &obj) :
        obj{obj}
    {}

    object obj;
};

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
    auto data = thisptr->__value.data_as<metainfo_data<Interfaces...>>();

    return boolean(data->interfaces.follows_interface(msg));
}

// FIXME shouldn't be necessary
bool __is_array(const object &obj);
object __get_fst(const object &obj);

template <typename... Interfaces>
object iface_collection_responds_to(object *thisptr, object &msg)
{
    // TODO access msg with messages instead
    if (!__is_array(msg))
        throw message_not_recognized_error{
            *thisptr, msg,
            "Argument of responds_to must be wrapped with an array"};

    auto data = thisptr->__value.data_as<metainfo_data<Interfaces...>>();
    auto unwrapped = __get_fst(msg);

    return boolean(data->interfaces.responds_to(&data->obj, unwrapped));
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
            follows = name == interface.interface_sym();

            return continue_unless(follows);
        });

        return follows;
    }

    send_msg_function select_function(object *thisptr, object &msg) const
    {
        if (msg == SY_RESP_TO)
            return iface_collection_responds_to<Interfaces...>;

        if (msg == SY_HAS_IFACE)
            return iface_collection_follows_interface<Interfaces...>;

        send_msg_function f;

        for_each(_interfaces, [&](const auto &interface)
        {
            f = interface.select_function(thisptr, msg);

            return continue_unless(f);
        });

        return f;
    }

    object select_response(object *thisptr, object &msg) const
    {
        if (msg == SY_RESP_TO || msg == SY_HAS_IFACE)
            return object{value_t{
                new metainfo_data<Interfaces...>{*this, *thisptr}},
                select_function(thisptr, msg)};

        object f;

        for_each(_interfaces, [&](const auto &interface)
        {
            f = interface.select_response(thisptr, msg);

            return continue_unless(f != empty());
        });

        return f;
    }

    inline bool responds_to(object *thisptr, object &msg) const
    {
        if (msg == SY_RESP_TO || msg == SY_HAS_IFACE)
            return select_function(thisptr, msg);

        bool responds;

        for_each(_interfaces, [&](const auto &interface)
        {
            responds = interface.responds_to(thisptr, msg);

            return continue_unless(responds);
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
    inline const object &interface_sym() const { return _iface; }\
    send_msg_function select_function(object *thisptr, object &msg) const;\
    bool responds_to(object *thisptr, object &msg) const;\
    object select_response(object *thisptr, object &msg) const;\
private:\
    object _iface = symbol(__name);

namespace iface
{

class arith
{
    RBB_IFACE("arith")

public:
    arith(
        send_msg_function add_function,
        send_msg_function sub_function,
        send_msg_function mul_function,
        send_msg_function div_function,
        send_msg_function mod_function);

private:
    send_msg_function
        _add_function,
        _sub_function,
        _mul_function,
        _div_function,
        _mod_function;
};

class comparable
{
    RBB_IFACE("comparable")

public:
    comparable(send_msg_function eq_function, send_msg_function ne_function);

private:
    send_msg_function
        _eq_function,
        _ne_function;
};

class ordered
{
    RBB_IFACE("ordered")

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
    RBB_IFACE("symbol")
};

class numeric
{
    RBB_IFACE("numeric")

public:
    numeric(send_msg_function array_send_msg);

private:
    send_msg_function _array_send_msg;
};

class booleanoid
{
    RBB_IFACE("boolean")

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
        _if_true_send_msg,
        _raise_send_msg;
};

class listable
{
    RBB_IFACE("listable")

public:
    listable(
        send_msg_function concat_send_msg,
        send_msg_function slice_send_msg,
        int (*get_size)(object *),
        object (*get_element)(object *, int),
        void (*set_element)(object *, int, object));

private:
    send_msg_function
        _concat_send_msg,
        _slice_send_msg;
    int (*_get_size)(object *);
    object (*_get_element)(object *, int);
    void (*_set_element)(object *, int, object);
};

class mapped
{
    RBB_IFACE("table")
    
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
    RBB_IFACE("block")

public:
    executable(send_msg_function execute);

private:
    send_msg_function _execute;
};

}

}

#endif
