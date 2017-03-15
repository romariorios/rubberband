// Rubberband language
// Copyright (C) 2013--2014, 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
using namespace std;

static unordered_set<string> *all_symbols = nullptr;

struct __symbols_lifetime
{
    ~__symbols_lifetime()
    {
        delete all_symbols;
    }
} __lifetime;

symbol_node rbb::retrieve_symbol(const string &sym)
{
    if (!all_symbols)
        all_symbols = new unordered_set<string>;
    
    auto found = all_symbols->find(sym);
    if (found != all_symbols->end())
        return &*found;
    
    return &*all_symbols->insert(sym).first;
}
