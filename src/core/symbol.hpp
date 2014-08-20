// Rubberband language
// Copyright (C) 2013, 2014  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <set>
#include <string>

namespace rbb
{

struct symbol_node;

struct cmp_symbol_nodes
{
    bool operator()(symbol_node *const node1, symbol_node *const node2) const;
};

struct symbol_node
{
    char ch;
    symbol_node *up = nullptr;
    std::set<symbol_node *, cmp_symbol_nodes> down;

    static symbol_node *retrieve(const std::string &string);
    symbol_node(char ch);
};

}

#endif
