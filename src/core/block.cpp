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
    ~block_statement_private()
    {
        if (expressions)
            delete expressions;
    }

    linked_list<expr *> *expressions;
    linked_list<expr *> *expressions_tail;
};

block_statement::block_statement() :
    _p(new block_statement_private)
{}

block_statement::~block_statement()
{
    delete _p;
}

void block_statement::set_context(const object& context)
{
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_expr->value->set_context(context);
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

    _p->expressions_tail = _p->expressions_tail->append(e);
}

object block_statement::eval()
{
    object cur_obj = _p->expressions->value->eval();

    for (linked_list<expr *> *cur_expr = _p->expressions->next;
         cur_expr; cur_expr = cur_expr->next)
    {
        cur_obj = cur_obj << cur_expr->value->eval();
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

void literal::array::set_context(const object& context)
{
    for (int i = 0; i < _size; ++i)
        _obj_array[i]->set_context(context);
}

void literal::array::set_arg(const object& arg)
{
    for (int i = 0; i < _size; ++i)
        _obj_array[i]->set_context(arg);
}

object literal::array::eval()
{
    object *arr = new object[_size];
    for (int i = 0; i < _size; ++i)
        arr[i] = _obj_array[i]->eval();

    object l = rbb::array(arr, _size);

    delete[] arr;
    return l;
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

void literal::table::set_context(const object &context)
{
    for (int i = 0; i < _size; ++i) {
        _symbol_array[i]->set_context(context);
        _obj_array[i]->set_context(context);
    }
}

void literal::table::set_arg(const object &arg)
{
    for (int i = 0; i < _size; ++i) {
        _symbol_array[i]->set_arg(arg);
        _obj_array[i]->set_arg(arg);
    }
}

object literal::table::eval()
{
    object *symbols = new object[_size];
    object *objects = new object[_size];

    for (int i = 0; i < _size; ++i) {
        symbols[i] = _symbol_array[i]->eval();
        objects[i] = _obj_array[i]->eval();
    }

    object e = rbb::table(symbols, objects, _size);

    delete[] symbols;
    delete[] objects;
    return e;
}

class rbb::literal::block_private
{
public:
    block_private() :
        statements(0),
        statements_tail(0),
        _return_expression(new literal::empty)
    {}

    ~block_private()
    {
        if (statements)
            delete statements;

        delete _return_expression;
    }

    void set_return_expression(expr *ret_exp)
    {
        delete _return_expression;
        _return_expression = ret_exp;
    }

    expr *return_expression() const
    {
        return _return_expression;
    }

    linked_list<block_statement *> *statements;
    linked_list<block_statement *> *statements_tail;

private:
    expr *_return_expression;
};

literal::block::block() :
    _p(new block_private)
{}

literal::block::~block()
{
    delete _p;
}

void literal::block::add_statement(block_statement* stm)
{
    if (!_p->statements) {
        _p->statements = new linked_list<block_statement *>(stm);
        _p->statements_tail = _p->statements;

        return;
    }

    _p->statements_tail = _p->statements_tail->append(stm);
}

void literal::block::set_return_expression(expr* expr)
{
    _p->set_return_expression(expr);
}

void literal::block::set_block_context(const object& context)
{
    for (linked_list<block_statement *> *cur_stm = _p->statements;
         cur_stm; cur_stm = cur_stm->next)
        cur_stm->value->set_context(context);

    _p->return_expression()->set_context(context);
}

void literal::block::set_block_arg(const object& arg)
{
    for (linked_list<block_statement *> *cur_stm = _p->statements;
         cur_stm; cur_stm = cur_stm->next)
        cur_stm->value->set_arg(arg);

    _p->return_expression()->set_arg(arg);
}

// eval() is at object.cpp

object literal::block::run()
{
    for (linked_list<block_statement *> *cur_stm = _p->statements;
         cur_stm; cur_stm = cur_stm->next)
        cur_stm->value->eval();

    return _p->return_expression()->eval();
}
