// Rubberband language
// Copyright (C) 2014--2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "parse.hpp"
#include "parse_private.hpp"

#include "block.hpp"
#include "common_syms.hpp"
#include "error.hpp"
#include "tokenizer.hpp"
#include "object_private.hpp"

#include "shared_data_t.hpp"

#include <memory>

using namespace rbb;
using namespace std;

// lemon externals
#include "lemon_parser.h"

extern void *LemonCParserAlloc(void *(*mallocProc)(size_t));
extern void LemonCParser(
    void *p,
    int tok_num,
    token *tok,
    extra_lemon_args *extra_args);
extern void *LemonCParserFree(void *p, void (*freeProc)(void *));

inline int token_to_tokcode(token t)
{
    switch (t.type) {
    case token::t::invalid:
    case token::t::start_of_input:  // not a valid token either
        return -1;
    case token::t::curly_open:
        return CURLY_OPEN;
    case token::t::parenthesis_open:
        return PARENTHESIS_OPEN;
    case token::t::end_of_input:
        return 0;
    case token::t::curly_close:
        return CURLY_CLOSE;
    case token::t::parenthesis_close:
        return PARENTHESIS_CLOSE;
    case token::t::arrow:
        return -1;
    case token::t::dot:
        return DOT;
    case token::t::comma:
        return COMMA;
    case token::t::equals:
        return EQUALS;
    case token::t::exclamation:
        return EXCLAMATION;
    case token::t::stm_sep:
        return SEP;
    case token::t::number:
    case token::t::number_f:
        return NUMBER;
    case token::t::boolean:
        return BOOLEAN;
    case token::t::symbol:
        return SYMBOL;
    case token::t::dollar:
        return DOLLAR;
    case token::t::tilde:
        return TILDE;
    case token::t::at:
        return AT;
    case token::t::percent:
        return PERCENT;
    case token::t::custom_literal:
        return CUSTOM_LITERAL;
    case token::t::bar:
        return BAR;
    case token::t::colon:
        return COLON;
    }
}

// lemon wrapper class
class lemon_parser
{
public:
    lemon_parser(const object &master_object) :
        _extra{master_object},
        _p{LemonCParserAlloc(malloc)}
    {}

    ~lemon_parser()
    {
        LemonCParserFree(_p, free);
    }

    void parse(token tok)
    {
        LemonCParser(_p, token_to_tokcode(tok), new token{tok}, &_extra);
    }

    object result()
    {
        return _extra.result;
    }

    extra_lemon_args _extra;

private:
    void *_p;
};

// master object functions and data structures
class master_data : public shared_data_t
{
public:
    master_data(base_master &master) :
        master{master}
    {}

    base_master &master;
};

object master_load_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<master_data>();

    if (msg.__value.type != value_t::symbol_t)
        throw semantic_error{"Symbol expected", *thisptr, msg};

    return d->master.load(msg.to_string());
}

object master_declare_literal_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<master_data>();

    if (msg.__value.type != value_t::data_t)
        throw semantic_error{"Table expected", *thisptr, msg};

    if (!msg.__value.try_data_as<table_data>())
        throw semantic_error{"Table expected", *thisptr, msg};

    auto trigger_obj = msg << SY_LDF_TRIG;
    unsigned char trigger;
    auto trigger_error_msg =
        "The trigger should be numeric";

    if (trigger_obj.__value.type == value_t::integer_t)
        trigger = static_cast<unsigned char>(trigger_obj.__value.integer());
    else if (trigger_obj.__value.type == value_t::floating_t)
        trigger = static_cast<unsigned char>(trigger_obj.__value.floating());
    else
        throw semantic_error{trigger_error_msg, *thisptr, msg};

    return d->master.declare_literal(
        trigger,
        msg << SY_LDF_EVAL,
        msg << SY_DLT << SY_LDF_REVAL == boolean(true)?
            msg << SY_LDF_REVAL : empty());
}

class custom_operation_data : public shared_data_t
{
public:
    explicit custom_operation_data(const object &sym, base_master &master) :
        symbol{sym},
        master{master}
    {}

    object symbol;
    base_master &master;
};

object master_custom_operation_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<custom_operation_data>();

    return d->master.custom_operation(d->symbol.to_string(), msg);
}

object master_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<master_data>();

    // load
    if (msg == SY_LOAD)
        return object{value_t{
            new master_data{d->master}},
            master_load_send_msg};

    // declare literal
    if (msg == SY_LDF_LIT)
        return object{value_t{
            new master_data{d->master}},
            master_declare_literal_send_msg};

    // custom operation
    if (msg.__value.type == value_t::symbol_t)
        return object{value_t{
            new custom_operation_data{msg, d->master}},
            master_custom_operation_send_msg};

    throw semantic_error{
        "Unknown operation by master object",
        *thisptr,
        msg};
}

// base_master methods
object base_master::parse(const string &code)
{
    tokenizer tokenizer{code, _literals};
    tokenizer.set_master(this);

    lemon_parser p{
        object{value_t{
            new master_data{*this}},
            master_send_msg}};

    try {
        for (
            auto tok = tokenizer.next();
            tok.type != token::t::end_of_input;
            tok = tokenizer.next()
        ) {
            p.parse(tok);
            continue;
        }
    } catch (const internal_syntax_error &e) {
        throw syntax_error{tokenizer.cur_line(), tokenizer.cur_col(), e.t};
    }

    p.parse(token::t::end_of_input);

    return p.result();
}

object base_master::declare_literal(
    unsigned char trigger,
    const object &evaluator,
    const object &post_evaluator)
{
    _literals[trigger] = {evaluator, post_evaluator};

    return {};
}
