// Rubberband language
// Copyright (C) 2014--2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "object.hpp"

#include <map>
#include <string>
#include <utility>

namespace rbb
{

class base_master
{
public:
    virtual object parse(const std::string &code) final;
    virtual object declare_literal(
        unsigned char trigger,
        const object &evaluator,
        const object &post_evaluator) final;

    virtual object load(const std::string &mod_name) = 0;
    virtual object custom_operation(const std::string &name, object &obj) = 0;

private:
    std::map<unsigned char, std::pair<object, object>> _literals;
};

}

#endif // PARSE_HPP
