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

#include <cstring>

using namespace rbb;

char symbol_downtree::key_from_node(rbb::splay_tree<char, symbol_node*>::node* n) const
{
    return n->item->ch;
}

symbol_downtree::~symbol_downtree()
{
    delete_tree(p_root);
}

void symbol_downtree::delete_tree(rbb::splay_tree< char, symbol_node* >::node* n)
{
    if (!n) return;    
    if (n->item) delete n->item;
    if (n->left) delete_tree(n->left);
    if (n->right) delete_tree(n->right);
}

symbol_node::symbol_node(char ch) :
    ch(ch),
    up(0)
{}

symbol_node::~symbol_node()
{}

static symbol_node *trie_head = 0;

symbol_node *rbb::symbol_node::retrieve(char *string)
{
    const int length = strlen(string);
    
    if (!trie_head)
        trie_head = new symbol_node('\0');

    symbol_node *node = trie_head;
    
    for (int i = 0; i < length; ++i) {
        char ch = string[i];
        symbol_node *new_d = new symbol_node(ch);
        symbol_node *d = node->down.insert_if_not_found(ch, new_d);
        if (d != new_d)
            delete new_d;
            
        node = d;
    }
    
    return node;
}
