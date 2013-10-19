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

// In the following order, each index of down represents a char:
// abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/><=:_

using namespace rbb;

/* 26 * 2 letters + 10 algarisms + 8 special symbols + underscore */
static const short rbb_alphabet_size = 71;
static const short rbb_small_letters_offset = 0;
static const short rbb_big_letters_offset = 26;
static const short rbb_numbers_offset = 52;
static const short rbb_special_symbols_offset = 62;
static const short rbb_underscore_index = 70;

static const short ch_to_i(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return ((short) ch - 'a') + rbb_small_letters_offset;
    
    if (ch >= 'A' && ch <= 'Z')
        return ((short) ch - 'A') + rbb_big_letters_offset;
    
    if (ch >= '0' && ch <= '9')
        return ((short) ch - '0') + rbb_numbers_offset;
    
    switch (ch) {
    case '+':
        return rbb_special_symbols_offset;
    case '-':
        return rbb_special_symbols_offset + 1;
    case '*':
        return rbb_special_symbols_offset + 2;
    case '/':
        return rbb_special_symbols_offset + 3;
    case '>':
        return rbb_special_symbols_offset + 4;
    case '<':
        return rbb_special_symbols_offset + 5;
    case '=':
        return rbb_special_symbols_offset + 6;
    case ':':
        return rbb_special_symbols_offset + 7;
    }
    
    return rbb_underscore_index;
}

symbol_node::symbol_node(char ch, symbol_node* up) :
    ch(ch),
    up(up),
    down(new symbol_node *[rbb_alphabet_size])
{
    if (up)
        this->up->down[ch_to_i(ch)] = this;
}

symbol_node::~symbol_node()
{
    for (short i = 0; i < rbb_alphabet_size; ++i)
        delete down[i];
    
    delete[] down;
}

symbol_node* symbol_node::down_symbol(char ch) const
{
    return down[ch_to_i(ch)];
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
        if (!down)
            down = new symbol_node(ch, node);

        node = down;
    }
    
    return node;
}
