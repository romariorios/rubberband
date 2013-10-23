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

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

namespace rbb
{

struct symbol_node;

struct symbol_downtree_node
{
    symbol_downtree_node(symbol_node *sym) :
        sym(sym),
        left(0),
        right(0),
        parent(0)
    {}
    ~symbol_downtree_node();
    
    symbol_node *sym;
    symbol_downtree_node *left, *right, *parent;
    
    void set_left(symbol_downtree_node *n)
    {
        left = n;
        if (left)
            left->parent = this;
    }
    
    void set_right(symbol_downtree_node *n)
    {
        right = n;
        if (right)
            right->parent = this;
    }
};

struct symbol_node
{
    char ch;
    symbol_node *up;
    symbol_downtree_node *down_root;
    
    static symbol_node *retrieve(char *string);
    inline static symbol_node *retrieve(const char *string)
        { return retrieve(const_cast<char *>(string)); }
    symbol_node(char ch, symbol_node *up);
    ~symbol_node();
    symbol_node *down_symbol(char ch);
};

}

#endif
