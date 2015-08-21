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

#include "parse.hpp"

#include "block.hpp"
#include "error.hpp"
#include "lemon_parser.h"

using namespace rbb;

extern void *LemonCParserAlloc(void *(*mallocProc)(size_t));
extern void LemonCParser(
    void *p,
    int tok_num,
    token *tok,
    ____rbb_internal::lemon_parser::extra *extra_args);
extern void *LemonCParserFree(void *p, void (*freeProc)(void *));

inline int token_to_tokcode(token t)
{
    switch (t.type) {
    case token::t::invalid:
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
        return ARROW;
    case token::t::dot:
        return DOT;
    case token::t::comma:
        return COMMA;
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
    case token::t::bar:
        return BAR;
    case token::t::colon:
        return COLON;
    }
}

____rbb_internal::lemon_parser::lemon_parser(const object &master_object) :
    _extra{master_object},
    _p{LemonCParserAlloc(malloc)}
{}

____rbb_internal::lemon_parser::~lemon_parser()
{
    LemonCParserFree(_p, free);
}

void ____rbb_internal::lemon_parser::parse(token tok)
{
    LemonCParser(_p, token_to_tokcode(tok), new token{tok}, &_extra);
}

object ____rbb_internal::lemon_parser::result()
{
    if (!_extra.result)
        return empty();

    return _extra.result->eval();
}
