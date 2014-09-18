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

parser::parser(const std::string &code) :
    _tokenizer{code}
{
    _block_stack.push(&_main_block);
}

object parser::parse()
{
    while (_tokenizer.look_next() != token::t::end_of_input) {
        switch (_cur_state) {
        case _state::start0:
            if (_next_tok_is(token::t::exclamation)) {
                _cur_state = _state::ret_stm0;
                _tokenizer.next();

                auto &cur_block = *_block_stack.top();
                _statement_stack.push(&cur_block.return_statement());
            }
            continue;
        case _state::ret_stm0:
            if (_next_tok_is(token::t::number) ||
                _next_tok_is(token::t::number_f)
            ) {
                auto &cur_stm = *_statement_stack.top();
                auto cur_tok = _tokenizer.next();
                cur_stm.add_expr<literal::number>(
                    cur_tok.type == token::t::number?
                        cur_tok.lexem.integer :
                        cur_tok.lexem.floating);

                _cur_nonterminal = _nonterminal::literal;
            }
            continue;
        }
    }

    return _main_block.eval();
}

bool parser::_next_tok_is(token::t type) const
{
    return _tokenizer.look_next().type == type;
}
