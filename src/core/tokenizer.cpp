// Rubberband language
// Copyright (C) 2014--2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "common_syms.hpp"
#include "error.hpp"
#include "parse.hpp"
#include "shared_data_t.hpp"

#include <algorithm>
#include <functional>
#include <vector>

using namespace rbb;
using namespace std;

token tokenizer::next()
{
    _look_token_args args{_cur_state};
    auto ret = _previous_token = _look_token(args);

    _remaining.erase(_remaining.begin(), _remaining.begin() + args.length);
    _cur_state = args;
    return ret;
}

token tokenizer::look_next() const
{
    _look_token_args args{_cur_state};
    return _look_token(args);
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
    tokenizer tok{_remaining, _literals};

    return tok.all();
}

void tokenizer::set_master(base_master *master)
{
    _master = master;
}

string tokenizer::_get_substr_until(_look_token_args &args, char ch) const
{
    const auto begin_ind = args.length - 1;
    const auto it = find(_remaining.cbegin() + begin_ind, _remaining.cend(), ch);
    if (it == _remaining.cend())
        return {};

    const auto new_len = it - _remaining.cbegin();
    args.length = new_len;
    return _remaining.substr(begin_ind, new_len - 1);
}

void tokenizer::_rewind(_look_token_args& args, char ch, long int prevcol)
{
    --args.length;
    if (ch == '\n') {
        --args.line;
        args.col = prevcol;
    }
}

static void throw_invalid_interface_error(
    long line,
    long col,
    const string &remaining,
    int ignore_offset,
    int length)
{
    throw tokenization_error{
        line,
        col,
        remaining.substr(ignore_offset, length - ignore_offset),
        "Invalid interface name"};
}

class tokenizer_data : public shared_data_t
{
public:
    tokenizer_data(
        function<char(int)> &&increment_by,
        function<string(char)> &&substr_until,
        const string &remaining,
        vector<object> &parsed_exprs,
        base_master *master) :

        _increment_by{increment_by},
        _substr_until{substr_until},
        _remaining{remaining},
        _parsed_exprs{parsed_exprs},
        _master{master}
    {}

    tokenizer_data(const tokenizer_data &other) = default;

    function<char(int)> _increment_by;
    function<string(char)> _substr_until;
    const string &_remaining;
    vector<object> &_parsed_exprs;
    base_master *_master;
};

object tokenizer_append_expr_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.try_data_as<tokenizer_data>();

    // TODO check if the message actually is a character
    const auto ch = static_cast<unsigned char>(msg.__value.integer());
    const auto expr = d->_substr_until(ch);

    // FIXME HACK this only works if the expression does not contain a self-reference (@)
    d->_parsed_exprs.emplace_back(d->_master->parse("{!" + expr + "}"));

    return {};
}

object tokenizer_send_msg(object *thisptr, object &msg)
{
    auto d = thisptr->__value.try_data_as<tokenizer_data>();
    if (msg == SY_LDF_CHVAL)
        return number(static_cast<unsigned char>(d->_increment_by(0)));

    if (msg == SY_LDF_SKIP)
        d->_increment_by(1);
    else if (msg == SY_LDF_BACK)
        d->_increment_by(-1);
    else if (msg == SY_LDF_PUNTIL)
        return object{value_t{
            new tokenizer_data{*d}},
            tokenizer_append_expr_send_msg};
    // TODO make tokenizer throw on error

    return {};
}

token tokenizer::_look_token(_look_token_args &args) const
{
    args.length = 1;
    int ignore_offset = 0;
    auto cur_state = _state::start;

    for (; args.length <= _remaining.size() + 1; ++args.length, ++args.col) {
        const auto &ch = args.length <= _remaining.size()?
            _remaining[args.length - 1] :
            '\0';
        const auto prevcol = args.col;

        if (ch == '\n') {
            args.col = 1;
            ++args.line;
        }

        // Start state machine
        switch (cur_state) {
        case _state::start:
            {
                const auto uch = static_cast<unsigned char>(ch);
                const auto cur_literal = _literals.find(uch);

                if (cur_literal != _literals.end()) {
                    auto data = new token::custom_literal_data;
                    
                    auto msg = object{value_t{
                        new tokenizer_data{
                            [this, &args](int increment)
                            {
                                args.col += increment;
                                args.length += increment;
                                return _remaining[args.length - 1];
                            },
                            [this, &args](char ch) { return _get_substr_until(args, ch); },
                            _remaining,
                            data->parsed_exprs,
                            _master}},
                        tokenizer_send_msg};

                    auto evaluators = cur_literal->second;
                    auto evaluator = evaluators.first;
                    data->obj = evaluator << msg;
                    data->post_evaluator = evaluators.second;
                    return token::custom_literal(data);
                }
            }

            // Ignore whitespace
            switch (ch) {
            case ' ':
            case '\t':
                ++ignore_offset;
                continue;
            case '\n':
                if (args.par_depth.top() != 0) {
                    ++ignore_offset;
                    continue;
                }

                switch (_previous_token.type) {
                case token::t::start_of_input:
                case token::t::curly_open:
                case token::t::comma:
                case token::t::equals:
                case token::t::exclamation:
                case token::t::stm_sep:
                case token::t::bar:
                case token::t::colon:
                    ++ignore_offset;
                    continue;
                default:
                {
                    // FIXME maybe this is a bit overkill?
                    tokenizer tok{_remaining.substr(args.length), _literals};
                    const auto next_tok = tok.look_next();

                    switch (next_tok.type) {
                    case token::t::curly_close:
                    case token::t::exclamation:
                    case token::t::end_of_input:
                        ++ignore_offset;
                        continue;
                    default:
                        return token::t::stm_sep;
                    }
                }
            }
            // Comment
            case '#':
                cur_state = _state::comment;
                continue;
            // Single-char tokens
            case '.':
                return token::t::dot;
            case '{':
                args.par_depth.push(0);
                return token::t::curly_open;
            case '(':
                args.par_depth.top()++;
                return token::t::parenthesis_open;
            case '[':
                cur_state = _state::special_symbol;
                continue;
            case '}':
                args.par_depth.pop();
                return token::t::curly_close;
            case ')':
                args.par_depth.top()--;
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
            case '%':
                return token::t::percent;
            case '|':
                return token::t::bar;
            case ':':
                return token::t::colon;
            // Special symbols
            case '?':
                cur_state = _state::question_mark_or_boolean;
                continue;
            case '+':
            case '*':
            case '^':
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
            // Dash
            case '-':
                if (
                    _previous_token.type == token::t::number ||
                    _previous_token.type == token::t::number_f
                )
                    return token::symbol("-");

                cur_state = _state::dash;
                continue;
            case '\0':
                _rewind(args, ch, prevcol);
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
                    args.length = _remaining.size();
                    return token::t::invalid;
                }

                continue;
            }
        case _state::comment:
            if (ch != '\n')
                continue;

            _rewind(args, ch, prevcol);
            cur_state = _state::start;
            continue;
        case _state::question_mark_or_boolean:
            switch (ch) {
            case '0':
                return token::boolean(false);
            case '1':
                return token::boolean(true);
            default:
                _rewind(args, ch, prevcol);
                return token::symbol("?");
            }
        case _state::dash:
            if (ch >= '0' && ch <= '9') {
                cur_state = _state::number_integer_part;
                continue;
            }

            _rewind(args, ch, prevcol);
            return token::symbol("-");
        case _state::number_integer_part:
            switch (ch) {
            case '.':
                cur_state = _state::number_fractional_part_or_dot;
                continue;
            default:
                if (!(ch >= '0' && ch <= '9')) {
                    _rewind(args, ch, prevcol);

                    return token::number(
                        std::stol(
                            _remaining.substr(0, args.length + 1)));
                }
                continue;
            }
        case _state::number_fractional_part_or_dot:
            if (!(ch >= '0' && ch <= '9')) {
                _rewind(args, ch, prevcol);
                _rewind(args, ch, prevcol);

                return token::number(
                    std::stol(
                        _remaining.substr(0, args.length + 1)));
            }

            cur_state = _state::number_fractional_part;
            continue;
        case _state::number_fractional_part:
            if (!(ch >= '0' && ch <= '9')) {
                _rewind(args, ch, prevcol);

                return token::number_f(
                    std::stod(
                        _remaining.substr(0, args.length + 1)));
            }
            continue;
        case _state::special_symbol:
            if (ch == ']')
                return token::symbol(
                    _remaining.substr(ignore_offset, args.length - ignore_offset));

            continue;
        case _state::gt_char:
            switch (ch) {
            case '<':
                return token::symbol("><");
            case '=':
                return token::symbol(">=");
            }

            _rewind(args, ch, prevcol);
            return token::symbol(">");
        case _state::lt_char:
            switch (ch) {
            case '=':
                return token::symbol("<=");
            case '<':
                cur_state = _state::double_lt;
                continue;
            }

            _rewind(args, ch, prevcol);
            return token::symbol("<");
        case _state::eq_symbol:
            if (ch == '=')
                return token::symbol("==");

            _rewind(args, ch, prevcol);
            return token::t::equals;
        case _state::slash_char:
            switch (ch) {
            case '=':
                return token::symbol("/=");
            case '\\':
                return token::symbol("/\\");
            }

            _rewind(args, ch, prevcol);
            return token::symbol("/");
        case _state::inverted_slash_char:
            if (ch == '/')
                return token::symbol("\\/");

            _rewind(args, ch, prevcol);
            return token::symbol("\\");
        case _state::alphanumeric_symbol:
            if (!(
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= '0' && ch <= '9') ||
                ch == '_'
            )) {
                _rewind(args, ch, prevcol);

                return token::symbol(
                    _remaining.substr(ignore_offset, args.length - ignore_offset));
            }
            continue;
        case _state::double_lt:
            if (ch == '?')
                return token::symbol("<<?");
            
            _rewind(args, ch, prevcol);
            return token::symbol("<<");
        }
    }

    --args.length;
    return token::t::end_of_input;
}

token &token::operator=(const token &other)
{
    type = other.type;

    if (other.type == t::symbol)
        lexem.str = new std::string{*other.lexem.str};
    else if (other.type == t::custom_literal)
        lexem.data = new custom_literal_data{*other.lexem.data};
    else
        lexem = other.lexem;

    return *this;
}

token::token(token &&other) :
    type{other.type}
{
    other.type = t::invalid;
    lexem = other.lexem;
}

token::~token()
{
    if (type == t::symbol)
        delete lexem.str;
    else if (type == t::custom_literal)
        delete lexem.data;
}

token token::number(long integer)
{
    token ret{t::number};
    ret.lexem.integer = integer;

    return ret;
}

token token::number_f(double floating)
{
    token ret{t::number_f};
    ret.lexem.floating = floating;

    return ret;
}

token token::boolean(bool val)
{
    token ret{t::boolean};
    ret.lexem.boolean = val;

    return ret;
}

token token::symbol(const string &str)
{
    token ret{t::symbol};
    ret.lexem.str = new std::string{str};

    return ret;
}

token token::custom_literal(custom_literal_data *d)
{
    token ret{token::t::custom_literal};
    ret.lexem.data = d;

    return ret;
}

bool token::operator==(const token &other) const
{
    if (type != other.type)
        return false;

    switch (type) {
    case t::number:
        return lexem.integer == other.lexem.integer;
    case t::number_f:
        return lexem.floating == other.lexem.floating;
    case t::symbol:
        return *lexem.str == *other.lexem.str;
    case t::custom_literal:
        return lexem.data == other.lexem.data;
    default:
        return true;
    }
}
