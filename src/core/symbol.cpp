// Rubberband language symbol (constructs a symbol from a string)
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

#include <cstring>

using namespace rbb;

symbol_downtree_node::~symbol_downtree_node()
{
    delete sym;
    if (smaller)
        delete smaller;
    if (bigger)
        delete bigger;
}

symbol_node::symbol_node(char ch, symbol_node* up) :
    ch(ch),
    up(up),
    down_root(0)
{
    if (up) {
        up->down_symbol(ch);
    }
}

symbol_node::~symbol_node()
{
    delete down_root;
}

symbol_node* symbol_node::down_symbol(char ch)
{
    // TODO tree balancing
    if (!down_root) {
        symbol_node *sym = new symbol_node(ch, 0);
        down_root = new symbol_downtree_node(sym);
        return sym;
    }
    
    symbol_downtree_node *prev = 0;
    for (symbol_downtree_node *cur = down_root; cur;
         cur = ch < cur->sym->ch? cur->smaller : cur->bigger) {
        if (cur->sym->ch == ch) {
            return cur->sym;
        }
        
        prev = cur;
    }
    
    symbol_node *sym = new symbol_node(ch, 0);
    
    if (ch < prev->sym->ch)
        prev->smaller = new symbol_downtree_node(sym);
    else
        prev->bigger = new symbol_downtree_node(sym);
    
    return sym;
}

static symbol_node *trie_head = 0;

symbol_node *rbb::symbol_node::retrieve(char *string)
{
    const int length = strlen(string);
    
    if (!trie_head)
        trie_head = new symbol_node('\0', 0);

    symbol_node *node = trie_head;
    
    for (int i = 0; i < length; ++i) {
        char ch = string[i];
        symbol_node *down = node->down_symbol(ch);
        node = down;
    }
    
    return node;
}
