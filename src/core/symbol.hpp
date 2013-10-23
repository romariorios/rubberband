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

#include "splay_tree.hpp"

namespace rbb
{

struct symbol_node;

class symbol_downtree : public splay_tree<char, symbol_node *>
{
public:
    symbol_downtree() {}
    ~symbol_downtree();

protected:
    char key_from_node(node *n) const;
    
private:
    void delete_tree(node *n);
};

struct symbol_node
{
    char ch;
    symbol_node *up;
    symbol_downtree down;
    
    static symbol_node *retrieve(char *string);
    inline static symbol_node *retrieve(const char *string)
        { return retrieve(const_cast<char *>(string)); }
    symbol_node(char ch);
    ~symbol_node();
};

}

#endif
