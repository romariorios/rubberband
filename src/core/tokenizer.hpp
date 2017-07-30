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

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <map>
#include <string>
#include <stack>
#include <vector>

#include "object.hpp"

namespace rbb
{

class base_master;

struct token
{
    enum class t {
        invalid,
        
        // Delimiters
        //  Openers
        start_of_input,
        curly_open,
        parenthesis_open,

        //  Closers
        end_of_input,
        curly_close,
        parenthesis_close,

        // Separators
        arrow,
        dot,
        comma,
        equals,
        exclamation,
        stm_sep,

        // Literals
        number,
        number_f,
        boolean,
        symbol,
        dollar,
        tilde,
        at,
        percent,
        custom_literal,

        // Triggers
        bar,
        colon
    } type;
    
    struct custom_literal_data
    {
        custom_literal_data() = default;
        custom_literal_data(const custom_literal_data &) = default;
        
        object obj;
        object post_evaluator;
        std::vector<object> parsed_exprs;
    };

    union {
        long integer;
        double floating;
        bool boolean;
        std::string *str;
        custom_literal_data *data;
    } lexem;

    token(t type) :
        type{type}
    {}
    
    token(const token &other)
    {
        *this = other;
    }
    
    token &operator=(const token &other);

    token(token &&other);
    ~token();

    static token number(long integer);
    static token number_f(double floating);
    static token boolean(bool val);
    static token symbol(const std::string &str);
    static token custom_literal(custom_literal_data *d);

    bool operator==(const token &other) const;

    inline bool operator!=(const token &other) const
    {
        return !(*this == other);
    }

    // Defined in to_string.cpp
    std::string to_string() const;
};

class tokenizer
{
    struct _look_token_args
    {
        _look_token_args() = default;
        _look_token_args(_look_token_args &&) = delete;

        _look_token_args(const _look_token_args &other)
        {
            *this = other;
        }

        _look_token_args &operator=(const _look_token_args &other)
        {
            line = other.line;
            col = other.col;
            par_depth = other.par_depth;

            return *this;
        }

        long length;
        long line = 1;
        long col = 1;
        std::stack<int> par_depth{{0}};
    } _cur_state;

public:
    tokenizer(
        const std::string &str,
        const std::map<unsigned char, std::pair<rbb::object, rbb::object>> &literals = {}) :
        _remaining{str},
        _literals{literals}
    {}

    tokenizer(
        std::string &&str = {},
        const std::map<unsigned char, std::pair<rbb::object, rbb::object>> &literals = {}) :
        _remaining{str},
        _literals{literals}
    {}

    token next();
    token look_next() const;
    std::vector<token> all();
    std::vector<token> look_all() const;
    
    void set_master(base_master *master);
    
    inline long cur_line() const { return _cur_state.line; }
    inline long cur_col() const { return _cur_state.col; }
    void _par_depth(int arg1);

private:
    enum class _state {
        start,
        comment,
        question_mark_or_boolean,
        dash,
        number_integer_part,
        number_fractional_part_or_dot,
        number_fractional_part,
        special_symbol,
        eq_symbol,
        gt_char,
        lt_char,
        slash_char,
        inverted_slash_char,
        alphanumeric_symbol,
        double_lt
    };

    static void _rewind(_look_token_args &args, char ch, long prevcol);
    token _look_token(_look_token_args &args) const;
    std::string _get_substr_until(_look_token_args &args, char ch) const;

    token _previous_token = token{token::t::start_of_input};
    std::string _remaining;
    const std::map<unsigned char, std::pair<rbb::object, rbb::object>> &_literals;
    base_master *_master = nullptr;
};

}

#endif

