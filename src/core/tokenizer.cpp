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

#include "tokenizer.hpp"

using namespace rbb;

token tokenizer::next()
{
    int length;
    auto ret = _previous_token = _look_token(length);

    _remaining.erase(_remaining.begin(), _remaining.begin() + length);
    return ret;
}

token tokenizer::look_next() const
{
    int l; // unused
    return _look_token(l);
}

std::vector<token> tokenizer::all()
{
    std::vector<token> result;

    for (auto tok = next(); tok.type == token::type_e::end_of_input; tok = next())
        result.push_back(tok);

    return result;
}

std::vector<token> tokenizer::look_all() const
{
    tokenizer tok{_remaining};

    return tok.all();
}

token tokenizer::_look_token(int& length) const
{
    length = 1;
    auto cur_state = _state::start;

    for (; length <= _remaining.size(); ++length) {
        auto &ch = _remaining[length - 1];

        // Start state machine
        switch (cur_state) {
        case _state::start:
            // Ignore whitespace
            switch (ch) {
            case ' ':
            case '\t':
                continue;
            case '\n':
                switch (_previous_token.type) {
                case token::type_e::start_of_input:
                case token::type_e::curly_open:
                case token::type_e::exclamation:
                case token::type_e::stm_sep:
                    continue;
                default:
                {
                    // TODO maybe this is a bit overkill?
                    tokenizer tok{_remaining.substr(length)};
                    const auto next_tok = tok.look_next();

                    switch (next_tok.type) {
                    case token::type_e::exclamation:
                    case token::type_e::curly_close:
                        continue;
                    default:
                        return token{token::type_e::stm_sep};
                    }
                }
            }

            // Single-char symbols
            case '{':
                return token{token::type_e::curly_open};
            case '}':
                return token{token::type_e::curly_close};
            case '!':
                return token{token::type_e::exclamation};

            // Number
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                cur_state = _state::number_integer_part;
                continue;
            }
        case _state::number_integer_part:
            switch (ch) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                continue;
            case '.':
                cur_state = _state::number_fractional_part;
                continue;
            default:
                --length;

                return token::number(
                    std::stol(
                        _remaining.substr(0, length + 1)));
            }
        case _state::number_fractional_part:
            switch (ch) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                continue;
            default:
                --length;

                return token::number_f(
                    std::stod(
                        _remaining.substr(0, length + 1)));
            }
        }
    }

    return token{token::type_e::end_of_input};
}
