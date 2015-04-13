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

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

using namespace std;

namespace rbb
{

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
        comma,
        exclamation,
        stm_sep,

        // Literals
        number,
        number_f,
        symbol,
        dollar,
        tilde,
        at,
        percent,

        // Triggers
        bar,
        colon
    } type;

    union {
        long integer;
        double floating;
        std::string *str;
    } lexem;

    token(t type) :
        type{type}
    {}
    
    token(const token &other)
    {
        *this = other;
    }
    
    token &operator=(const token &other)
    {
        type = other.type;
        
        if (other.type == t::symbol)
            lexem.str = new std::string{*other.lexem.str};
        else
            lexem = other.lexem;

        return *this;
    }
    
    token(token &&other) :
        type{other.type}
    {
        other.type = t::invalid;
        lexem = other.lexem;
    }

    ~token()
    {
        if (type == t::symbol)
            delete lexem.str;
    }

    static token number(long integer)
    {
        token ret{t::number};
        ret.lexem.integer = integer;

        return ret;
    }

    static token number_f(double floating)
    {
        token ret{t::number_f};
        ret.lexem.floating = floating;

        return ret;
    }

    static token symbol(const std::string &str)
    {
        token ret{t::symbol};
        ret.lexem.str = new std::string{str};

        return ret;
    }

    bool operator==(const token &other) const
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
        default:
            return true;
        }
    }

    inline bool operator!=(const token &other) const
    {
        return !(*this == other);
    }

    // Defined in to_string.cpp
    string to_string() const;
};

class tokenizer
{
public:
    tokenizer(const std::string &str) :
        _remaining{str}
    {}

    tokenizer(std::string &&str = std::string{}) :
        _remaining{str}
    {}

    token next();
    token look_next() const;
    std::vector<token> all();
    std::vector<token> look_all() const;
    
    inline long cur_line() const { return _cur_line; }
    inline long cur_col() const { return _cur_col; }

private:
    enum class _state {
        start,
        comment,
        merge_lines,
        arrow_or_negative_number,
        number_integer_part,
        number_fractional_part,
        eq_symbol,
        gt_char,
        lt_char,
        slash_char,
        inverted_slash_char,
        alphanumeric_symbol,
        double_lt,
        left_arrow,
        left_arrow_open_par,
        left_arrow_open_curly
    };

    long _cur_line = 1;
    long _cur_col = 1;
    
    bool _dot_ahead(int &ignore_offset, int& length, long &line, long &col) const;
    token _look_token(int &length, long &line, long &col) const;

    token _previous_token = token{token::t::start_of_input};
    std::string _remaining;
};

}

#endif

