// Rubberband language
// Copyright (C) 2014, 2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef PARSE_HPP
#define PARSE_HPP

#include "block.hpp"
#include "object.hpp"
#include "tokenizer.hpp"

#include "error.hpp"
#include "shared_data_t.hpp"

#include <memory>
#include <string>

namespace rbb
{
    
    namespace ____rbb_internal
    {
    
    template <typename master_t>
    class load_data : public shared_data_t
    {
    public:
        load_data(const object &ctx, master_t &master) :
            context{ctx},
            master{master}
        {}

        object context;
        master_t &master;
    };

    template <class master_t>
    object master_load_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<load_data<master_t> *>(thisptr->__value.data());

        if (msg.__value.type != value_t::symbol_t)
            throw semantic_error{"Symbol expected", *thisptr, msg};

        return d->master.load(d->context, msg.to_string());
    }

    template <class master_t>
    class master_data : public shared_data_t
    {
    public:
        master_data(master_t &master) :
            master{master}
        {}

        master_t &master;
    };
    
    template <class master_t>
    object master_set_context_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<master_data<master_t> *>(thisptr->__value.data());

        return functor(new load_data<master_t>{msg, d->master}, master_load_send_msg<master_t>);
    }

    template <typename master_t>
    class custom_operation_data : public shared_data_t
    {
    public:
        explicit custom_operation_data(const object &sym, master_t &master) :
            symbol{sym},
            master{master}
        {}

        object symbol;
        master_t &master;
    };

    template <class master_t>
    object master_custom_operation_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<custom_operation_data<master_t> *>(thisptr->__value.data());

        return d->master.custom_operation(d->symbol.to_string(), msg);
    }

    template <class master_t>
    object master_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<master_data<master_t> *>(thisptr->__value.data());

        if (msg == symbol("^"))
            return object::create_data_object(
                new master_data<master_t>{d->master},
                master_set_context_send_msg<master_t>,
                value_t::functor_t);

        if (msg.__value.type == value_t::symbol_t)
            return functor(
                new custom_operation_data<master_t>{msg, d->master},
                master_custom_operation_send_msg<master_t>);

        throw semantic_error{
            "Unknown operation by master object",
            *thisptr,
            msg};
    }
    
    class lemon_parser
    {
    public:
        lemon_parser(const object &master_object);
        ~lemon_parser();
        void parse(token tok);
        object result();

        struct extra
        {
            extra(const object &master) :
                master{master}
            {}

            literal::block *result = nullptr;
            object master;
        } _extra;

    private:
        void *_p;
    };

    }

template <class master_t>
object parse(const string &code, master_t &master)
{
    tokenizer tokenizer{code};

    ____rbb_internal::lemon_parser p{
        object::create_data_object(
            new ____rbb_internal::master_data<master_t>{master},
            ____rbb_internal::master_send_msg<master_t>,
            value_t::functor_t)};
    
    try {
        for (
            auto tok = tokenizer.next();
            tok.type != token::t::end_of_input;
            tok = tokenizer.next()
        ) {
            p.parse(tok);
        }
    } catch (syntax_error e) {
        e.line = tokenizer.cur_line();
        e.column = tokenizer.cur_col();
        throw e;
    }

    p.parse(token::t::end_of_input);

    return p.result();
}

}

#endif // PARSE_HPP
