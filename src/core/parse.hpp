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

#ifndef PARSE_HPP
#define PARSE_HPP

#include "block.hpp"
#include "object.hpp"
#include "tokenizer.hpp"

#include <stack>
#include <string>

namespace rbb
{

class parser
{
public:
    parser(const std::string &code);

    object parse();

private:
    enum class _state {
        start0,
        ret_stm0
    };

    enum class _nonterminal {
        none,
        literal
    };

    inline bool _next_tok_is(token::t type) const;

    _state _cur_state = _state::start0;
    _nonterminal _cur_nonterminal = _nonterminal::none;
    std::stack<literal::block *> _block_stack;
    std::stack<block_statement *> _statement_stack;
    tokenizer _tokenizer;
    literal::block _main_block;
};

}

#endif // PARSE_HPP
