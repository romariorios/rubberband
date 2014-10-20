// Rubberband language
// Copyright (C) 2014  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
#include "lemon_parser.h"

using namespace rbb;

extern void *LemonCParserAlloc(void *(*mallocProc)(size_t));
extern void LemonCParser(void *p, int tok_num, token *tok, literal::block **bl);
extern void *LemonCParserFree(void *p, void (*freeProc)(void *));

inline int token_to_tokcode(token t)
{
    switch (t.type) {
    case token::t::invalid:
        return -1;
    case token::t::bracket_open:
        return BRACKET_OPEN;
    case token::t::curly_open:
        return CURLY_OPEN;
    case token::t::parenthesis_open:
        return PARENTHESIS_OPEN;
    case token::t::end_of_input:
        return 0;
    case token::t::bracket_close:
        return BRACKET_CLOSE;
    case token::t::curly_close:
        return CURLY_CLOSE;
    case token::t::parenthesis_close:
        return PARENTHESIS_CLOSE;
    case token::t::arrow:
        return ARROW;
    case token::t::comma:
        return COMMA;
    case token::t::exclamation:
        return EXCLAMATION;
    case token::t::stm_sep:
        return SEP;
    case token::t::number:
    case token::t::number_f:
        return NUMBER;
    case token::t::symbol:
        return SYMBOL;
    case token::t::dollar:
        return DOLLAR;
    case token::t::tilde:
        return TILDE;
    case token::t::at:
        return AT;
    case token::t::bar:
        return BAR;
    case token::t::colon:
        return COLON;
    }
}

class lemon_parser
{
public:
    ~lemon_parser()
    {
        LemonCParserFree(_p, free);
    }

    inline void parse(token tok)
    {
        LemonCParser(_p, token_to_tokcode(tok), &tok, &_result);
    }

    inline object result()
    {
        if (!_result)
            return empty();

        return _result->eval();
    }

private:
    literal::block *_result = nullptr;
    void *_p = LemonCParserAlloc(malloc);
};

parser::parser(const std::string &code) :
    _tokenizer{code}
{}

object parser::parse()
{
    lemon_parser p;

    for (
        auto tok = _tokenizer.next();
        tok.type != token::t::end_of_input;
        tok = _tokenizer.next()
    ) {
        p.parse(tok);
    }

    p.parse(token::t::end_of_input);

    return p.result();
}
