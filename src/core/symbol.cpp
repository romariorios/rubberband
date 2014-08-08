// Rubberband language
// Copyright (C) 2013  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include "symbol.hpp"

#include <algorithm>
#include <cstring>

using namespace rbb;

symbol_node::symbol_node(char ch) :
    ch {ch}
{}

static symbol_node *trie_head = 0;

symbol_node *rbb::symbol_node::retrieve(const std::string &string)
{
    if (!trie_head)
        trie_head = new symbol_node('\0');

    symbol_node *node = trie_head;

    for (auto ch : string) {
        auto result = std::find_if(
            node->down.begin(),
            node->down.end(),
            [ch](symbol_node *node) -> bool {
                return node->ch == ch;
            });
        
        if (result == node->down.end()) {
            auto sym = new symbol_node {ch};
            sym->up = node;
            
            node->down.insert(sym);
            node = sym;
        } else {
            node = *result;
        }
    }

    return node;
}
