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

#include "block.hpp"

#include "block_private.hpp"

using namespace rbb;

void block_statement::add_expr(expr* e)
{
    expressions_tail =
        expressions.insert_after(expressions_tail, e);
}

object block_statement::eval(literal::block* parent_block)
{
    auto cur_expr = expressions.begin();
    auto cur_obj = (*cur_expr)->eval(parent_block);
    
    cur_expr++;

    for (; cur_expr != expressions.end(); cur_expr++)
    {
        cur_obj = cur_obj << (*cur_expr)->eval(parent_block);
    }

    return cur_obj;
}

literal::array::array(expr* obj_array[], int size) :
    _obj_array(obj_array),
    _size(size)
{}

literal::array::~array()
{
    for (int i = 0; i < _size; ++i)
        delete _obj_array[i];
}

object literal::array::eval(literal::block* parent_block)
{
    std::vector<object> arr;
    for (int i = 0; i < _size; ++i)
        arr.push_back(_obj_array[i]->eval(parent_block));

    return rbb::array(arr);
}

literal::table::table(expr* symbol_array[], expr* obj_array[], int size) :
    _symbol_array(symbol_array),
    _obj_array(obj_array),
    _size(size)
{}

literal::table::~table()
{
    for (int i = 0; i < _size; ++i) {
        delete _symbol_array[i];
        delete _obj_array[i];
    }
}

object literal::table::eval(literal::block* parent_block)
{
    std::vector<object> symbols;
    std::vector<object> objects;

    for (int i = 0; i < _size; ++i) {
        symbols.push_back(_symbol_array[i]->eval(parent_block));
        objects.push_back(_obj_array[i]->eval(parent_block));
    }

    return rbb::table(symbols, objects);
}

object literal::self_ref::eval(literal::block* parent_block)
{
    return parent_block->eval();
}

object literal::context::eval(literal::block* parent_block)
{
    return parent_block->_context;
}

object literal::message::eval(literal::block* parent_block)
{
    return parent_block->_message;
}

literal::block::block() :
    _p(new block_private)
{}

literal::block::block(const block& other) :
    _p(other._p)
{}

void literal::block::add_statement(block_statement* stm)
{
    _p->statements_tail =
        _p->statements.insert_after(_p->statements_tail, stm);
}

void literal::block::set_return_expression(expr* expr)
{
    _p->set_return_expression(expr);
}

void literal::block::set_block_context(const object& context)
{
    _context = context;
}

void literal::block::set_block_message(const object& msg)
{
    _message = msg;
}

// eval() is at object.cpp

object literal::block::run()
{
    for (auto cur_stm : _p->statements)
        cur_stm->eval(this);

    return _p->return_expression()->eval(this);
}
