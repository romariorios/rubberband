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

using namespace rbb;

object parser::parse()
{
    auto tok = _tokenizer.next();

    if (tok == token{token::type_e::exclamation}) {
        _cur_stm = &_main_block.return_statement();

        for (
            auto ret_token = _tokenizer.next();
            ret_token != token{token::type_e::curly_close} &&
            ret_token != token{token::type_e::end_of_input};
            ret_token = _tokenizer.next()
        )
            _tok_to_literal(ret_token);
    }

    return _main_block.eval();
}

void parser::_tok_to_literal(const token &tok)
{
    switch (tok.type) {
    case token::type_e::number:
        _cur_stm->add_expr<literal::number>(tok.lexem.integer);
        break;
    case token::type_e::number_f:
        _cur_stm->add_expr<literal::number>(tok.lexem.floating);
        break;
    }
}
