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

#include <stack>
#include <string>

namespace rbb
{
    
    namespace ____rbb_internal
    {
    
    class load_data : public shared_data_t
    {
    public:
        load_data(const object &ctx) :
            context{ctx}
        {}

        object context;
    };

    template <class master_t>
    object master_load_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<load_data *>(thisptr->__value.data);

        if (msg.__value.type != value_t::symbol_t)
            throw semantic_error{"Symbol expected", *thisptr, msg};

        return master_t::load(d->context, msg.to_string());
    }
    
    template <class master_t>
    object master_set_context_send_msg(object *thisptr, const object &msg)
    {
        object load;
        load.__value.type = value_t::data_t;
        load.__value.data = new load_data{msg};
        load.__send_msg = master_load_send_msg<master_t>;
        
        return load;
    }

    class custom_operation_data : public shared_data_t
    {
    public:
        explicit custom_operation_data(const object &sym) :
            symbol{sym}
        {}

        object symbol;
    };

    template <class master_t>
    object master_custom_operation_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<custom_operation_data *>(thisptr->__value.data);

        return master_t::custom_operation(d->symbol.to_string(), msg);
    }

    template <class master_t>
    object master_send_msg(object *thisptr, const object &msg)
    {
        if (msg == symbol("^")) {
            object load;
            load.__value.type = value_t::no_data_t;
            load.__value.data = nullptr;
            load.__send_msg = master_set_context_send_msg<master_t>;

            return load;
        } else if (msg.__value.type == value_t::symbol_t) {
            object custom;
            custom.__value.type = value_t::data_t;
            custom.__value.data = new custom_operation_data{msg};
            custom.__send_msg = master_custom_operation_send_msg<master_t>;

            return custom;
        } else {
            throw semantic_error{
                "Unknown operation by master object",
                *thisptr,
                msg};
        }
    }
    
    class lemon_parser
    {
    public:
        lemon_parser();
        ~lemon_parser();
        void parse(token tok);
        object result(const object &master);

    private:
        literal::block *_result = nullptr;
        void *_p;
    };

    }

template <class master_t>
object parse(const string &code)
{
    tokenizer tokenizer{code};
    
    ____rbb_internal::lemon_parser p;
    
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

    object master_object;
    master_object.__value.type = value_t::no_data_t;
    master_object.__value.data = nullptr;
    master_object.__send_msg = ____rbb_internal::master_send_msg<master_t>;

    return p.result(master_object);
}

}

#endif // PARSE_HPP
