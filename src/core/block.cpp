// Rubberband language
// Copyright (C) 2013--2015, 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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
#include "common_syms.hpp"
#include "object_private.hpp"
#include "shared_data_t.hpp"

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

    try {
        auto cur_obj = (*cur_expr)->eval(parent_block);

        cur_expr++;

        for (; cur_expr != expressions.end(); cur_expr++)
        {
            cur_obj = cur_obj << (*cur_expr)->eval(parent_block);
        }

        return cur_obj;
    } catch (const std::logic_error &e) {
        throw in_statement_error{to_string(), e};
    }
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

object literal::self_ref::eval(literal::block* parent_block)
{
    if (!parent_block)
        throw use_without_parent_block{'@'};

    return parent_block->eval();
}

object literal::context::eval(literal::block* parent_block)
{
    if (!parent_block)
        throw use_without_parent_block{'~'};

    return parent_block->_context;
}

object literal::message::eval(literal::block* parent_block)
{
    if (!parent_block)
        throw use_without_parent_block{'$'};

    return parent_block->_message;
}

// The post-eval context should have the following:
// - result of "=":     =
// - eval current expr: [!]
// - skip expression:   >>
class post_eval_ctx_data : public shared_data_t
{
public:
    post_eval_ctx_data(
        object obj,
        std::vector<object> &parsed_exprs,
        literal::block *parent) :

        obj{obj},
        parsed_exprs{parsed_exprs},
        parent{parent}
    {}

    object obj;
    std::vector<object> &parsed_exprs;
    literal::block *parent;
    size_t current_index = 0;
};

object post_eval_ctx_fun(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<post_eval_ctx_data>();

    if (msg == SY_LDF_ERES)
        return d->obj;

    if (msg == SY_LDF_EXPVAL) {
        // FIXME HACK to extract the return expression from a block object
        auto &cur_expr_block = d->parsed_exprs[d->current_index++];
        auto block_d = cur_expr_block.__value.try_data_as<block_data>();
        auto &ret_stm = block_d->block_l->return_statement();

        return ret_stm.eval(d->parent);
    }

    if (msg == SY_LDF_EXPSKIP) {
        ++d->current_index;
        return {};
    }

    throw message_not_recognized_error{*thisptr, msg};
}

object literal::user_defined::eval(literal::block *parent)
{
    if (_post_evaluator == rbb::empty())
        return _obj;

    auto post_eval_ctx =
        object{value_t{
            new post_eval_ctx_data{_obj, _exprs, parent}},
            post_eval_ctx_fun};
    auto evaluator = _post_evaluator << post_eval_ctx;

    return evaluator << rbb::empty();
}

literal::block::block() :
    _p(new block_private)
{}

literal::block::block(const block& other) :
    _p{other._p}
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
    _context_set = true;
}

void literal::block::set_message(const object& msg)
{
    _message = msg;
}

// eval() is at object.cpp

object literal::block::run()
{
    for (auto &cur_stm : _p->statements)
        cur_stm.eval(this);

    return _p->return_statement->eval(this);
}
