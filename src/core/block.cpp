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

#include "block.hpp"

#include "data_templates.hpp"

using namespace rbb;

class rbb::block_statement_private
{
public:
    block_statement_private() :
        expressions(0),
        expressions_tail(0)
    {}
    
    linked_list<expr *> *expressions;
    linked_list<expr *> *expressions_tail;
};

block_statement::block_statement() :
    _p(new block_statement_private)
{}

block_statement::~block_statement()
{
    delete _p->expressions;
    delete _p;
}

void block_statement::set_symbol_table(const object& sym_table)
{
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_expr->value->set_symbol_table(sym_table);
}

void block_statement::set_arg(const object& arg)
{
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_expr->value->set_arg(arg);
}

void block_statement::add_expr(expr* e)
{
    if (!_p->expressions) {
        _p->expressions = new linked_list<expr *>(e);
        _p->expressions_tail = _p->expressions;
        return;
    }
    
    _p->expressions_tail->next = new linked_list<expr *>(e);
    _p->expressions_tail = _p->expressions_tail->next;
}

object block_statement::eval()
{
    object cur_obj = empty();
    
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_obj = cur_expr->value->eval();
    
    return cur_obj;
}
