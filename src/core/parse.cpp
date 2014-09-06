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
    _parse_stack.push(_state::start_beg);
}

object parser::parse()
{
    block_statement *cur_expr = nullptr;
    
    while (_tokenizer.look_next() != token::t::end_of_input) {
        switch (_parse_stack.top()) {
        case _state::start_beg:
            switch (_tokenizer.look_next().type) {
            case token::t::exclamation:
                _push_state(_state::block_answer_beg);
                cur_expr = &_main_block.return_statement();
                continue;
            }
        case _state::block_answer_beg:
            switch (_tokenizer.look_next().type) {
            case token::t::number:
                cur_expr->add_expr<literal::number>(
                    _tokenizer.next().lexem.integer);
                continue;
            case token::t::number_f:
                cur_expr->add_expr<literal::number>(
                    _tokenizer.next().lexem.floating);
                continue;
            }
        }
    }
    
    return _main_block.eval();
}

void parser::_push_state(parser::_state s)
{
    _parse_stack.push(s);
    _token_stack.push(_tokenizer.next());
}
