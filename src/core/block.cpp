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
#include "error.hpp"

#include "block_private.hpp"

using namespace rbb;

void expr_list::append_ptr(expr *e)
{
    emplace_after(list_end, e);
}

block_statement::block_statement(block_statement &&other) :
    expressions{std::move(other.expressions)}
{}

void block_statement::add_expr_ptr(expr *e)
{
    expressions.append_ptr(e);
}

object block_statement::eval(literal::block* parent_block)
{
    if (expressions.empty())
        return empty();

    auto cur_expr = expressions.begin();
    auto cur_obj = (*cur_expr)->eval(parent_block);

    cur_expr++;

    for (; cur_expr != expressions.end(); cur_expr++)
    {
        cur_obj = cur_obj << (*cur_expr)->eval(parent_block);
    }

    return cur_obj;
}

object literal::array::eval(literal::block* parent_block)
{
    std::vector<object> arr;
    for (auto &el : _objects)
        arr.push_back(el->eval(parent_block));

    return rbb::array(arr);
}

void literal::array::add_element_ptr(expr *e)
{
    _objects.append_ptr(e);
}

object literal::table::eval(literal::block* parent_block)
{
    std::vector<object> symbols;
    for (auto &sym : _symbols)
        symbols.push_back(sym->eval(parent_block));

    std::vector<object> objects;
    for (auto &obj : _objects)
        objects.push_back(obj->eval(parent_block));

    return rbb::table(symbols, objects);
}

void literal::table::add_symbol_ptr(expr *e)
{
    _symbols.append_ptr(e);
}

void literal::table::add_object_ptr(expr *e)
{
    _objects.append_ptr(e);
}

object literal::master::eval(literal::block* parent_block)
{
    return parent_block->_master;
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
    _p{other._p},
    _master{other._master}
{}

block_statement &literal::block::add_statement()
{
    _p->statements_tail =
        _p->statements.emplace_after(_p->statements_tail);

    return *_p->statements_tail;
}

void literal::block::add_statement_ptr(block_statement *stm)
{
    _p->statements_tail =
        _p->statements.emplace_after(_p->statements_tail, std::move(*stm));
}

block_statement &literal::block::return_statement()
{
    return *_p->return_statement;
}

void literal::block::set_return_statement_ptr(block_statement *stm)
{
    _p->return_statement.reset(stm);
}

void literal::block::set_context(const object& context)
{
    _context = context;
}

void literal::block::set_message(const object& msg)
{
    _message = msg;
}

void literal::block::set_master(const object &master)
{
    _master = master;
}

// eval() is at object.cpp

object literal::block::run()
{
    for (auto &cur_stm : _p->statements)
        cur_stm.eval(this);

    return _p->return_statement->eval(this);
}
