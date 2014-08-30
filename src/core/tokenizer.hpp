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

#include <vector>
#include <string>

namespace rbb
{

struct token
{
    enum class type_e {
        start_of_input,
        end_of_input,
        curly_open,
        stm_sep,
        exclamation,
        curly_close,
        number,
        number_f
    } type;

    union {
        long integer;
        double floating;
    } lexem;

    token(type_e type) :
        type{type}
    {}

    static token number(long integer)
    {
        token ret{type_e::number};
        ret.lexem.integer = integer;

        return ret;
    }

    static token number_f(double floating)
    {
        token ret{type_e::number_f};
        ret.lexem.floating = floating;

        return ret;
    }

    bool operator==(const token &other) const
    {
        if (type != other.type)
            return false;

        switch (type) {
        case type_e::number:
            return lexem.integer == other.lexem.integer;
        case type_e::number_f:
            return lexem.floating == other.lexem.floating;
        default:
            return true;
        }
    }

    inline bool operator!=(const token &other) const
    {
        return !(*this == other);
    }
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

private:
    enum class _state {
        start,
        number_integer_part,
        number_fractional_part
    };

    token _look_token(int &length) const;

    token _previous_token = token{token::type_e::start_of_input};
    std::string _remaining;
};

}

#endif
