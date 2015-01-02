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
    
    template <class master_t>
    class master_data : public shared_data_t
    {
    public:
        master_data() = default;
        master_data(const master_data<master_t> &other) = default;

        master_t master;
    };

    template <class master_t>
    object master_load_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<master_data<master_t> *>(thisptr->__value.data);

        if (msg.__value.type != value_t::symbol_t)
            throw semantic_error{"Symbol expected", *thisptr, msg};

        return d->master.load(msg.to_string());
    }
    
    template <class master_t>
    object master_set_context_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<master_data<master_t> *>(thisptr->__value.data);
        
        d->master.set_context(msg);
        
        object load;
        load.__value.type = value_t::data_t;
        load.__value.data = new master_data<master_t>{*d};
        load.__send_msg = master_load_send_msg<master_t>;
        
        return load;
    }

    template <class master_t>
    object master_send_msg(object *thisptr, const object &msg)
    {
        auto d = static_cast<master_data<master_t> *>(thisptr->__value.data);

        if (msg == symbol("^")) {
            object load;
            load.__value.type = value_t::data_t;
            load.__value.data = new master_data<master_t>{*d};
            load.__send_msg = master_set_context_send_msg<master_t>;

            return load;
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
class parser
{
public:
    parser(const std::string &code) :
        _tokenizer{code}
    {}

    object parse()
    {
        ____rbb_internal::lemon_parser p;
        
        for (
            auto tok = _tokenizer.next();
            tok.type != token::t::end_of_input;
            tok = _tokenizer.next()
        ) {
            p.parse(tok);
        }

        p.parse(token::t::end_of_input);

        object master_object;
        master_object.__value.type = value_t::data_t;
        master_object.__value.data = new ____rbb_internal::master_data<master_t>;
        master_object.__send_msg = ____rbb_internal::master_send_msg<master_t>;

        return p.result(master_object);
    }

private:    
    tokenizer _tokenizer;
    master_t _master;
};

}

#endif // PARSE_HPP
