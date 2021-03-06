// Rubberband language
// Copyright (C) 2015, 2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef PARSE_PRIVATE_HPP
#define PARSE_PRIVATE_HPP

#include "object.hpp"
#include "tokenizer.hpp"

namespace rbb
{

struct extra_lemon_args
{
    explicit extra_lemon_args(const object &master) :
        master{master}
    {}

    object master;
    object result;
};

class internal_syntax_error
{
public:
    internal_syntax_error(const token &t) :
        t{t}
    {}

    const token t;
};

}

#endif
