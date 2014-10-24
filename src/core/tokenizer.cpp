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
    long line = _cur_line, col = _cur_col;
    auto ret = _previous_token = _look_token(length, line, col);
    ret.line = _cur_line;
    ret.column = _cur_col;

    _remaining.erase(_remaining.begin(), _remaining.begin() + length);
    _cur_line = line;
    _cur_col = col;
    return ret;
}

token tokenizer::look_next() const
{
    int l; // unused
    long li, col;
    
    auto tok = _look_token(l, li, col);
    tok.line = _cur_line;
    tok.column = _cur_col;
    
    return tok;
}

std::vector<token> tokenizer::all()
{
    std::vector<token> result;

    for (auto tok = next(); tok.type != token::t::end_of_input; tok = next())
        result.push_back(tok);

    return result;
}

std::vector<token> tokenizer::look_all() const
{
    tokenizer tok{_remaining};

    return tok.all();
}

bool tokenizer::_dot_ahead(int &ignore_offset, int &length, long &line, long &col) const
{
    for (auto &ch : _remaining.substr(length)) {
        ++length;
        ++ignore_offset;
        ++col;
        
        switch (ch) {
        case ' ':
        case '\t':
            continue;
        case '\n':
            col = 1;
            ++line;
            continue;
        case '.':
            return true;
        default:
            --length;
            --ignore_offset;
            return false;
        }
    }
}

token tokenizer::_look_token(int& length, long &line, long &col) const
{
    length = 1;
    int ignore_offset = 0;
    auto cur_state = _state::start;

    for (; length <= _remaining.size() + 1; ++length, ++col) {
        const auto &ch = length <= _remaining.size()?
            _remaining[length - 1] :
            '\0';

        // Start state machine
        switch (cur_state) {
        case _state::start:
            // Ignore whitespace
            switch (ch) {
            case ' ':
            case '\t':
                ++ignore_offset;
                continue;
            case '\n':
                col = 1;
                ++line;
                switch (_previous_token.type) {
                case token::t::start_of_input:
                case token::t::curly_open:
                case token::t::exclamation:
                case token::t::stm_sep:
                    ++ignore_offset;
                    continue;
                default:
                {
                    // FIXME maybe this is a bit overkill?
                    tokenizer tok{_remaining.substr(length)};
                    const auto next_tok = tok.look_next();

                    switch (next_tok.type) {
                    case token::t::exclamation:
                    case token::t::curly_close:
                    case token::t::end_of_input:
                        ++ignore_offset;
                        continue;
                    default:
                        if (_dot_ahead(ignore_offset, length, line, col)) {
                            cur_state = _state::merge_lines;
                            continue;
                        }
                        
                        return token::t::stm_sep;
                    }
                }
            }
            // Comment
            case '#':
                cur_state = _state::comment;
                continue;
            // Merge lines
            case '.':
                cur_state = _state::merge_lines;
                continue;
            // Single-char tokens
            case '[':
                return token::t::bracket_open;
            case '{':
                return token::t::curly_open;
            case '(':
                return token::t::parenthesis_open;
            case ']':
                return token::t::bracket_close;
            case '}':
                return token::t::curly_close;
            case ')':
                return token::t::parenthesis_close;
            case ',':
                return token::t::comma;
            case '!':
                return token::t::exclamation;
            case ';':
                if (_previous_token.type != token::t::stm_sep)
                    return token{token::t::stm_sep};
                
                ++ignore_offset;
                continue;
            case '$':
                return token::t::dollar;
            case '~':
                return token::t::tilde;
            case '@':
                return token::t::at;
            case '|':
                return token::t::bar;
            case ':':
                return token::t::colon;
            // Special symbols
            case '?':
            case '+':
            case '*':
                return token::symbol(std::string{ch});
            case '>':
                cur_state = _state::gt_char;
                continue;
            case '<':
                cur_state = _state::lt_char;
                continue;
            case '=':
                cur_state = _state::eq_symbol;
                continue;
            case '/':
                cur_state = _state::slash_char;
                continue;
            case '\\':
                cur_state = _state::inverted_slash_char;
                continue;
            // Arrow or number
            case '-':
                if (
                    _previous_token.type == token::t::number ||
                    _previous_token.type == token::t::number_f
                )
                    return token::symbol("-");
                
                cur_state = _state::arrow_or_negative_number;
                continue;
            case '\0':
                --length;
                return token::t::end_of_input;
            default:
                // Number
                if (ch >= '0' && ch <= '9')
                    cur_state = _state::number_integer_part;
                // Symbol
                else if (
                    (ch >= 'a' && ch <= 'z') ||
                    (ch >= 'A' && ch <= 'Z') ||
                    ch == '_'
                )
                    cur_state = _state::alphanumeric_symbol;
                else {
                    length = _remaining.size();
                    return token::t::invalid;
                }
                
                continue;
            }
        case _state::comment:
            if (ch != '\n')
                continue;
            
            --length;
            cur_state = _state::start;
        case _state::merge_lines:
            switch (ch) {
            case ' ':
            case '\t':
            case '\n':
                continue;
            default:
                --length;
                cur_state = _state::start;
                continue;
            }
        case _state::arrow_or_negative_number:
            if (ch == '>')
                return token::t::arrow;
            if (ch >= '0' && ch <= '9') {
                cur_state = _state::number_integer_part;
                continue;
            }
            
            --length;
            return token::symbol("-");
        case _state::number_integer_part:
            switch (ch) {
            case '.':
                cur_state = _state::number_fractional_part;
                continue;
            default:
                if (!(ch >= '0' && ch <= '9')) {
                    --length;

                    return token::number(
                        std::stol(
                            _remaining.substr(0, length + 1)));
                }
                continue;
            }
        case _state::number_fractional_part:
            if (!(ch >= '0' && ch <= '9')) {
                --length;

                return token::number_f(
                    std::stod(
                        _remaining.substr(0, length + 1)));
            }
            continue;
        case _state::gt_char:
            switch (ch) {
            case '<':
                return token::symbol("><");
            case '=':
                return token::symbol(">=");
            }
            
            --length;
            return token::symbol(">");
        case _state::lt_char:
            if (ch == '=')
                return token::symbol("<=");
            
            --length;
            return token::symbol("<");
        case _state::eq_symbol:
            if (ch == '=')
                return token::symbol("==");
            
            --length;
            return token::symbol("=");
        case _state::slash_char:
            switch (ch) {
            case '=':
                return token::symbol("/=");
            case '\\':
                return token::symbol("/\\");
            }
            
            --length;
            return token::symbol("/");
        case _state::inverted_slash_char:
            if (ch == '/')
                return token::symbol("\\/");
            
            --length;
            return token::symbol("\\");
        case _state::alphanumeric_symbol:
            if (!(
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= '0' && ch <= '9') ||
                ch == '_'
            )) {
                --length;
                
                return token::symbol(
                    _remaining.substr(ignore_offset, length - ignore_offset));
            }
            continue;
        }
    }

    --length;
    return token::t::end_of_input;
}

